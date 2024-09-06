#include <vector>
#include <cmath>

#include "system/MotorControllerStates.hpp"
#include "system/arrayCompareLogic.hpp"
#include "log.hpp"
#include "Profiling.hpp"



// Macros -----------------------------------------------------------------------
#define RESET_TIME_50ms 50 /**< Reset of Inverter Error is stopped after 50ms */
#define MAX_RESET_TIME_500ms 500 /**< Reset of Inverter Error is stopped after 500ms */
#define BUTTON_PRESS_TIME_4s 4000 /**< Time to press the button to change the mode */


// Disable State ######################################################################################################
void Disable::enterState(MotorController *motorController) { PROFILING_FUNCTION();
    motorController->getMotorControllerData()->currentState = DisableState;

    CAN_data_out_t &dataOut = *motorController->getCANOutput();
    for (int i = 0; i < 4; ++i){
        if (motorController->getSettings()->motorEnable[i]){
            asArray(&dataOut.out_targetTorque_FL)[i] = 0;
            asArray(&dataOut.out_inverterOn_FL)[i] = false;
            asArray(&dataOut.out_inverterEnable_FL)[i] = false;
            asArray(&dataOut.out_inverterDCon_FL)[i] = false;
            asArray(&dataOut.out_inverterErrorReset_FL)[i] = false;
        }
    }
    dataOut.out_torqueLimitPos = 0;
    dataOut.out_torqueLimitNeg = 0;
    dataOut.out_prechargeRelay = false;
    dataOut.out_AIRPositive = false;
    dataOut.out_AIRNegative = false;

    dataOut.out_RTDsoundOn = false;

    motorController->getMotorControllerData()->TSLed = false;
    motorController->getMotorControllerData()->RTDLed = false;

}

void Disable::stateAction(MotorController* motorController) { PROFILING_FUNCTION();
 
    if (motorController->getSensorData()->SDC_Pressed) {
        //turn off TSLed if SDCok is false
        motorController->getMotorControllerData()->TSLed = false;
        for (int i = 0; i < 4; ++i){
            if (motorController->getSettings()->motorEnable[i]){
                asArray(&motorController->getCANOutput()->out_inverterErrorReset_FL)[i] = true;
            }
        } 

        // Emergency Shut Down Button is pressed (Red Button)
        const led_button_box_t & buttonData = *motorController->getLedButtonData();

        // Drive mode selection (Modes: Slow, Endurance, Normal)
        if(buttonData.TSButtonPressed && buttonData.RTDButtonPressed){
             // Reset Modes (Normal Mode)
              motorController->getMotorControllerData()->currentDriveMode = normalMode;
              buttonPressTimer.stop();
        }else if(buttonData.RTDButtonPressed){
            // Start Timer for Button Press
            if(buttonPressTimer.isStopped()){
                buttonPressTimer.start();
            }
            // Set Slow Mode after pressing RTD Button for 4 seconds
            if (buttonPressTimer.pastTime() >= BUTTON_PRESS_TIME_4s){
                motorController->getMotorControllerData()->currentDriveMode = slowMode;
                buttonPressTimer.stop();
            }
        }else if(buttonData.TSButtonPressed){
            // Start Timer for Button Press
            if(buttonPressTimer.isStopped()){
                buttonPressTimer.start();
            }
            // Set Endurance Mode after pressing TS Button for 4 seconds
            if (buttonPressTimer.pastTime() >= BUTTON_PRESS_TIME_4s){
                motorController->getMotorControllerData()->currentDriveMode = enduranceMode;
                buttonPressTimer.stop();
            }
        }else{
            // No Button is pressed
            buttonPressTimer.stop();
        }

    }else{
        // SDCok is true emergency Shut Down Button is not pressed (Red Button)
        for (int i = 0; i < 4; ++i) {
            if (motorController->getSettings()->motorEnable[i]){
                asArray(&motorController->getCANOutput()->out_inverterErrorReset_FL)[i] = false;
            }
        }
        //check if the buttons where released after changing to Disable state
        if(!motorController->getLedButtonData()->TSButtonPressed && !motorController->getLedButtonData()->RTDButtonPressed){
            buttonReleasedFlag = true;
        }
    }

   

}

void Disable::updateState(MotorController* motorController) { PROFILING_FUNCTION();
    if(!motorController->getSensorData()->SDC_Pressed){
        // Checks if all 4 Inverter are ready                                          
        if(allElementsTrueM(asArray(&motorController->getInverterData()->inverterSystemReady_FL),motorController->getSettings()->motorEnable)){
            motorController->getMotorControllerData()->TSLed = true;
            if(buttonReleasedFlag) {
                //check if the TSButtonPressed is pressed
                
                if (motorController->getLedButtonData()->TSButtonPressed) {
                    motorController->setState(Precharge::getInstance());
                }
            }
        }else{
            // check which inverter is not ready and make a log entry
            std::vector<int> inverterPositions = getFalseElementPositionsM(asArray(&motorController->getInverterData()->inverterSystemReady_FL),motorController->getSettings()->motorEnable);
            for(int inverterPosition : inverterPositions){
                const std::string Positions[4] = {"front left", "front right", "rear left", "rear right"};
                //ECU_INFO(" Inverter {} is not ready", Positions[inverterPosition]);
            }
            // Turn off TSLed if not all Inverter are ready
            motorController->getMotorControllerData()->TSLed = false;
        }

    }
}
void Disable::exitState(MotorController *motorController) { PROFILING_FUNCTION();
    buttonReleasedFlag = false;
}

std::string Disable::toString() { PROFILING_FUNCTION();
    return "Disable";
}

State<MotorController>& Disable::getInstance() { PROFILING_FUNCTION();
    static Disable instance;
    return instance;
}

// Precharge State ######################################################################################################
void Precharge::enterState(MotorController *motorController) { PROFILING_FUNCTION();
    motorController->getMotorControllerData()->currentState = PrechargeState;

    prechargeTimer.start();

    CAN_data_out_t &dataOut = *motorController->getCANOutput();
    dataOut.out_prechargeRelay = true;
    dataOut.out_AIRNegative = true;
    for (int i = 0; i < 4; ++i) {
        if (motorController->getSettings()->motorEnable[i]){
            asArray(&dataOut.out_inverterDCon_FL)[i] = true;
        }
    }
    // Turn Led off
    motorController->getMotorControllerData()->TSLed = false;
    motorController->getMotorControllerData()->RTDLed = false;
}

void Precharge::stateAction(MotorController* motorController) { PROFILING_FUNCTION();
    //check if the buttons where released after changing to precharge state from disable state
    if(!motorController->getLedButtonData()->TSButtonPressed && !motorController->getLedButtonData()->RTDButtonPressed){
        buttonReleasedFlag = true;
    }
}

void Precharge::updateState(MotorController* motorController) { PROFILING_FUNCTION();
   
    if (motorController->getSensorData()->SDC_Pressed) {
        motorController->setState(Disable::getInstance());

    }else if(motorController->getLedButtonData()->TSButtonPressed || motorController->getLedButtonData()->RTDButtonPressed){
        // Abort Precharge if TSButtonPressed or RTDButtonPressed is pressed and change to Disable State
        if (buttonReleasedFlag){
            motorController->setState(Disable::getInstance());
        }
        
    }else if(allElementsTrueM(asArray(&motorController->getInverterData()->inverterDCon_FL),motorController->getSettings()->motorEnable)){
        // check if car has DC voltage and change to HVCoupling State
        
         if (allElementsGEM(&motorController->getInverterData()->invertersVoltage_FL, (motorController->getAccuManagementData()->accuVoltage * 0.95),motorController->getSettings()->motorEnable)) {
              motorController->setState(HVCoupling::getInstance());
         }
    }
    
    if(motorController->getAccuData()->AIRNegative  || motorController->getAccuData()->prechargeRelay ){
        //Check if Precharge circuit is closed for more than 5 seconds
        if (closedCircuitTimer.isStopped()){
            closedCircuitTimer.start();
        }
        if (closedCircuitTimer.pastTime() >= 5000){
            motorController->setState(Error::getInstance());
            ECU_ERROR(PRECHARGE_CIRCUIT_TIMEOUT_ERROR,"Precharge circuit was closed for more than 5s. Change to Error state");
        }
    }else if (prechargeTimer.pastTime() >= 7000){
        // Switches to Error State if the Precharge state takes more than 7 seconds
        motorController->setState(Error::getInstance());
        ECU_ERROR(PRECHARGE_TIMEOUT_ERROR,"The precharge state took more than 7s. Change to Error state");
    }
}

void Precharge::exitState(MotorController *motorController) { PROFILING_FUNCTION();
    prechargeTimer.stop();
    buttonReleasedFlag = false;
    closedCircuitTimer.stop();
}

std::string Precharge::toString() { PROFILING_FUNCTION();
    return "Precharge";
}

State<MotorController>& Precharge::getInstance() { PROFILING_FUNCTION();
    static Precharge instance;
    return instance;
}

// HVCoupling State ######################################################################################################
void HVCoupling::enterState(MotorController *motorController) { PROFILING_FUNCTION();
    motorController->getMotorControllerData()->currentState = HVCouplingState;
    couplingTimer.start();

    CAN_data_out_t &dataOut = *motorController->getCANOutput();
    dataOut.out_prechargeRelay = false;
    dataOut.out_AIRPositive = true;
    dataOut.out_AIRNegative = true;

    // Turn TSLed off
    motorController->getMotorControllerData()->TSLed = false;
    motorController->getMotorControllerData()->RTDLed = false;
}

void HVCoupling::stateAction(MotorController* motorController) { PROFILING_FUNCTION();

}

void HVCoupling::updateState(MotorController* motorController) { PROFILING_FUNCTION();
    if (motorController->getSensorData()->SDC_Pressed) {
        motorController->setState(Disable::getInstance());
        
    }else if((motorController->getAccuData()->prechargeRelay) || !(motorController->getAccuData()->AIRPositive)){
        // Switches to Error State if the HVCoupling state takes more than 3 seconds
        if(couplingTimer.pastTime() >= 3000){
            motorController->setState(Error::getInstance());
            ECU_ERROR(HVCOUPLING_TIMEOUT_ERROR,"HVCoupling state took more than 3s. Change to Error state");
        }

    } else if(motorController->getPedalData()->breakPressed && motorController->getPedalData()->throttle == 0) {
       
            motorController->getMotorControllerData()->RTDLed = true;
            if (motorController->getLedButtonData()->RTDButtonPressed) {
                motorController->setState(ReadyToDrive::getInstance());
            }
        
    }else{
        motorController->getMotorControllerData()->RTDLed = false;
        if (motorController->getLedButtonData()->RTDButtonPressed || motorController->getLedButtonData()->TSButtonPressed) {
            motorController->setState(Disable::getInstance());
        }
    }
}

void HVCoupling::exitState(MotorController *motorController) { PROFILING_FUNCTION();
    couplingTimer.stop();
}

std::string HVCoupling::toString() { PROFILING_FUNCTION();
    return "HVCoupling";
}

State<MotorController>& HVCoupling::getInstance() { PROFILING_FUNCTION();
    static HVCoupling instance;
    return instance;
}

// ReadyToDrive State ######################################################################################################
void ReadyToDrive::enterState(MotorController *motorController) { PROFILING_FUNCTION();
    motorController->getMotorControllerData()->currentState = ReadyToDriveState;

    for (int i = 0; i < 4; ++i) {
        if (motorController->getSettings()->motorEnable[i]){
            asArray(&motorController->getCANOutput()->out_inverterEnable_FL)[i] = true;
            asArray(&motorController->getCANOutput()->out_inverterOn_FL)[i] = true;
        }
    }

    // Turn TSLed off
    motorController->getMotorControllerData()->TSLed = false;
    motorController->getMotorControllerData()->RTDLed = false;
}

void ReadyToDrive::stateAction(MotorController* motorController) { PROFILING_FUNCTION();

}

void ReadyToDrive::updateState(MotorController* motorController) { PROFILING_FUNCTION();
    if (motorController->getSensorData()->SDC_Pressed) {
        motorController->setState(Disable::getInstance());
       
    }else if(!motorController->getLedButtonData()->RTDButtonPressed){
        if(allElementsTrueM(asArray(&motorController->getInverterData()->inverterOn_FL),motorController->getSettings()->motorEnable)){ 
            motorController->setState(Drive::getInstance());
        }
        
    }
}

void ReadyToDrive::exitState(MotorController *motorController) { PROFILING_FUNCTION();

}

std::string ReadyToDrive::toString() { PROFILING_FUNCTION();
    return "ReadyToDrive";
}

State<MotorController>& ReadyToDrive::getInstance() { PROFILING_FUNCTION();
    static ReadyToDrive instance;
    return instance;
}

// Drive State ######################################################################################################
void Drive::enterState(MotorController *motorController) { PROFILING_FUNCTION();
    motorController->getMotorControllerData()->currentState = DriveState;

    inverterStartupTimer.start();

    CAN_data_out_t &dataOut = *motorController->getCANOutput();
    dataOut.out_torqueLimitPos = motorController->getSettings()->maxTorque;
    dataOut.out_torqueLimitNeg = 0;
    previousThrottle = 0;   



    // Turn TSLed off
    motorController->getMotorControllerData()->TSLed = false;
    motorController->getMotorControllerData()->RTDLed = false;
}

void Drive::stateAction(MotorController* motorController) { PROFILING_FUNCTION();
    if (allElementsTrueM(asArray(&motorController->getInverterData()->inverterOn_FL),motorController->getSettings()->motorEnable)){
        // Wait until Ready to Drive sound is finished
        if(RTDsoundRunningFlag){
            if(RTDsoundTimer.isStopped()){
                RTDsoundTimer.start();
                motorController->getCANOutput()->out_RTDsoundOn = true;
            }
            if (RTDsoundTimer.pastTime() >= 2000){
                RTDsoundTimer.stop();
                motorController->getCANOutput()->out_RTDsoundOn = false;
                RTDsoundRunningFlag = false;
            }
        }else{
            // calculate target velocity
            if(motorController->getPedalData()->breakPressed ||
                  motorController->getPedalData()->APPSError ||
                  motorController->getPedalData()->breakError){
                previousThrottle = 0;
                for (int i = 0; i < 4; ++i) {
                    if (motorController->getSettings()->motorEnable[i]){
                        asArray(&motorController->getCANOutput()->out_targetTorque_FL)[i] = 0;
                    }
                }
            }else{
                    // get  filtere wheele Target torque
                    uint16_t filteredThrottle = throttleLP(motorController->getPedalData()->throttle,motorController->getSettings()->alpha);
                    int16_t targetTorque = calculateTorque(filteredThrottle,motorController->getCANOutput()->out_torqueLimitPos);
                
                    if(motorController->getMotorControllerData()->currentDriveMode == slowMode){
                        //Only use 20% of throttle input when in slow mode
                        for (int i = 0; i < 4; ++i) {
                            if (motorController->getSettings()->motorEnable[i]){
                                asArray(&motorController->getCANOutput()->out_targetTorque_FL)[i] = targetTorque / 100;
                            }
                        }
                    }else{
                        for (int i = 0; i < 4; ++i) {
                            if (motorController->getSettings()->motorEnable[i]){
                                asArray(&motorController->getCANOutput()->out_targetTorque_FL)[i] = targetTorque * motorController->getSettings()->motorLoad [i] / 100.0;
                            }
                        }
                    }
                    
                    // save max RPM and max Torque
                    if (motorController->getPedalData()->throttle > 0){
                        for(int i = 0; i < 4; i++){
                            if(asArray(&motorController->getInverterData()->inverterActualTorque_FL)[i] > asArray(&motorController->getMotorControllerData()->maxTorque_FL)[i]){
                                asArray(&motorController->getMotorControllerData()->maxTorque_FL)[i] = asArray(&motorController->getInverterData()->inverterActualTorque_FL)[i];
                                //ECU_TRACE("New max Torque {}: {}", Positions[i] ,asArray(&motorController->getMotorControllerData()->maxTorque_FL)[i]);
                                if(asArray(&motorController->getMotorControllerData()->maxTorque_FL)[i] > motorController->getMotorControllerData()->maxTorque){
                                    motorController->getMotorControllerData()->maxTorque = asArray(&motorController->getMotorControllerData()->maxTorque_FL)[i];
                                    ECU_TRACE("New max Torque: {}", motorController->getMotorControllerData()->maxTorque);
                                }
                            }
                            if(asArray(&motorController->getInverterData()->inverterActualRPM_FL)[i] > asArray(&motorController->getMotorControllerData()->maxRPM_FL)[i]){
                                asArray(&motorController->getMotorControllerData()->maxRPM_FL)[i] = asArray(&motorController->getInverterData()->inverterActualRPM_FL)[i];
                                //ECU_TRACE("New max RPM {}: {}", Positions[i] ,asArray(&motorController->getMotorControllerData()->maxRPM_FL)[i]);
                                if(asArray(&motorController->getMotorControllerData()->maxRPM_FL)[i] > motorController->getMotorControllerData()->maxRPM){
                                    motorController->getMotorControllerData()->maxRPM = asArray(&motorController->getMotorControllerData()->maxRPM_FL)[i];
                                    //ECU_TRACE("New max RPM: {}", motorController->getMotorControllerData()->maxRPM);
                                }
                            }
                        }
                    }
                    // check if inverter is derating
                    for(int i = 0; i < 4; i++){
                        static bool deratingFlag[4] = {false,false,false,false};
                        static bool deratingOffFlag[4] = {true,true,true,true};
                        if(asArray(&motorController->getInverterData()->inverterDerating_FL)[i] ){
                            if(!deratingFlag[i]){
                                ECU_WARN("Inverter {} is derating", Positions[i]);
                                deratingFlag[i] = true;
                            }
                            deratingOffFlag[i] = false;
                        }else{

                            if(!deratingOffFlag[i]){
                                ECU_INFO("Inverter {} is no longer derating", Positions[i]);
                                deratingOffFlag[i] = true;
                            }
                            deratingFlag[i] = false;
                        }
                    }


            }

        }
    }

}

void Drive::updateState(MotorController* motorController) { PROFILING_FUNCTION();
    if (motorController->getSensorData()->SDC_Pressed) {
        motorController->setState(Disable::getInstance());
    }else if(motorController->getPedalData()->APPSError){
        ECU_ERROR(MOTOR_PEDAL_ERROR,"APPS error detected in Pedalbox. Change to Error state")
        motorController->setState(Error::getInstance());
    }else if(motorController->getPedalData()->breakError){
        ECU_ERROR(MOTOR_BREAK_ERROR,"Break error detected in Pedalbox. Change to Error state")
        motorController->setState(Error::getInstance());
    }else if(motorController->getLedButtonData()->RTDButtonPressed || motorController->getLedButtonData()->TSButtonPressed){
        motorController->setState(Disable::getInstance());
    }else if(!allElementsTrueM(asArray(&motorController->getInverterData()->inverterDCon_FL),motorController->getSettings()->motorEnable)){
        std::vector<int> inverterPositions = getFalseElementPositionsM(asArray(&motorController->getInverterData()->inverterDCon_FL),
                                                                        motorController->getSettings()->motorEnable);
        for(int inverterPosition : inverterPositions){
            ECU_WARN("No DC voltage on the {} inverter (Inverter DC = off). Change to Disable state", Positions[inverterPosition]);
        }
    
        motorController->setState(Error::getInstance());

        
    }else if(!allElementsTrueM(asArray(&motorController->getInverterData()->inverterSystemReady_FL),motorController->getSettings()->motorEnable)) {
        std::vector<int> inverterPositions = getFalseElementPositionsM(asArray(&motorController->getInverterData()->inverterSystemReady_FL),
                                                                                motorController->getSettings()->motorEnable);
        for(int inverterPosition : inverterPositions){
            ECU_WARN("Inverter system {} no longer ready for operation (Inverter System not Ready). Change to Disable state", Positions[inverterPosition]);
        }

        motorController->setState(Error::getInstance());

    }else if(!allElementsTrueM(asArray(&motorController->getInverterData()->inverterOn_FL),motorController->getSettings()->motorEnable)){
        if(!inverterStartupTimer.isActive() || inverterStartupTimer.isActive() && inverterStartupTimer.pastTime() >= 5000){
            std::vector<int> inverterPositions = getFalseElementPositionsM(asArray(&motorController->getInverterData()->inverterOn_FL),
                                                                                    motorController->getSettings()->motorEnable);
            for(int inverterPosition : inverterPositions){
                ECU_INFO(" Inverter {} turned off. Change to Disable state", Positions[inverterPosition]);
            }

            motorController->setState(Error::getInstance());
   
            if(inverterStartupTimer.isActive()){
                inverterStartupTimer.stop();
            }
        }
    }else if(motorController->getSettings()->recuperationEnabled && !RTDsoundRunningFlag){
        if (motorController->getPedalData()->throttle <= motorController->getSettings()->recuperationLevel){
            motorController->setState(Recuperate::getInstance());
            RTDsoundRunningFlag = false;
        }
    }
}

void Drive::exitState(MotorController *motorController) { PROFILING_FUNCTION();
    RTDsoundRunningFlag = true;
}

std::string Drive::toString() { PROFILING_FUNCTION();
    return "Drive";
}

State<MotorController>& Drive::getInstance() { PROFILING_FUNCTION();
    static Drive instance;
    return instance;
}

uint16_t Drive::throttleLP(uint16_t throttle, uint8_t alpha){
    float alphaDez = alpha / 100.0;
    uint16_t filteredThrottle; 
    previousThrottle = alphaDez * throttle + (1-alphaDez) * previousThrottle;
    filteredThrottle = previousThrottle;
    //ECU_WARN("Throttle: {}, Filtered Throttle: {}, alphaDez {}", throttle, filteredThrottle, alphaDez);
    return filteredThrottle;
}

int16_t Drive::calculateTorque(uint16_t throttle, int32_t maxTorque){
    double normalisedThrottle = throttle / 1000.0;
    return maxTorque * normalisedThrottle;
}
// Recuperate State ######################################################################################################
void Recuperate::enterState(MotorController *motorController) { PROFILING_FUNCTION();
    motorController->getMotorControllerData()->currentState = RecuperateState;

    inverterStartupTimer.start();
    CAN_data_out_t &dataOut = *motorController->getCANOutput();

    for (int i = 0; i < 4; ++i) {
        if (motorController->getSettings()->motorEnable[i]){
            asArray(&dataOut.out_targetTorque_FL)[i] = 0;
            asArray(&dataOut.out_inverterEnable_FL)[i] = true;
        }
    }
    dataOut.out_torqueLimitPos = 0;

    if (motorController->getMotorControllerData()->currentDriveMode == enduranceMode) {
        dataOut.out_torqueLimitNeg = motorController->getSettings()->maxRecuperrationTorque;
    } else {
        dataOut.out_torqueLimitNeg = 0;
    }
    // Turn TSLed off
    motorController->getMotorControllerData()->TSLed = false;
    motorController->getMotorControllerData()->RTDLed = false;
}

void Recuperate::stateAction(MotorController* motorController) { PROFILING_FUNCTION();
 // calculate target velocity
            if(motorController->getPedalData()->breakPressed ||
                  motorController->getPedalData()->APPSError ||
                  motorController->getPedalData()->breakError){
                for (int i = 0; i < 4; ++i) {
                    if (motorController->getSettings()->motorEnable[i]){
                        asArray(&motorController->getCANOutput()->out_targetTorque_FL)[i] = 0;
                    }
                }
            }else{
                    for (int i = 0; i < 4; ++i) {
                        if (motorController->getSettings()->motorEnable[i]){
                            asArray(&motorController->getCANOutput()->out_targetTorque_FL)[i] =
                                        motorController->getCANOutput()->out_torqueLimitNeg * (1 - motorController->getPedalData()->throttle/10) ;
                        }
                    }
            }

}

void Recuperate::updateState(MotorController* motorController) { PROFILING_FUNCTION();
    if (motorController->getSensorData()->SDC_Pressed) {
        motorController->setState(Disable::getInstance());
    }else if(motorController->getLedButtonData()->RTDButtonPressed || motorController->getLedButtonData()->TSButtonPressed) {
        motorController->setState(Disable::getInstance());
    }else if(!allElementsTrueM(asArray(&motorController->getInverterData()->inverterDCon_FL),motorController->getSettings()->motorEnable)){
        motorController->setState(Disable::getInstance());
        std::vector<int> inverterPositions = getFalseElementPositionsM(asArray(&motorController->getInverterData()->inverterDCon_FL),
                                                                        motorController->getSettings()->motorEnable);
        for(int inverterPosition : inverterPositions){
            ECU_WARN(" No DC voltage on the {} inverter (Inverter DC = off). Change to Disable state", Positions[inverterPosition]);
        }
    }else if(!allElementsTrueM(asArray(&motorController->getInverterData()->inverterSystemReady_FL),motorController->getSettings()->motorEnable)) {
        motorController->setState(Disable::getInstance());
        std::vector<int> inverterPositions = getFalseElementPositionsM(asArray(&motorController->getInverterData()->inverterSystemReady_FL),motorController->getSettings()->motorEnable);
        for(int inverterPosition : inverterPositions){
            ECU_WARN(" Inverter system {} no longer ready for operation (Inverter System not Ready). Change to Disable state", Positions[inverterPosition]);
        }
    }else if(!allElementsTrueM(asArray(&motorController->getInverterData()->inverterOn_FL),motorController->getSettings()->motorEnable)){
        if(!inverterStartupTimer.isActive() || inverterStartupTimer.isActive() && inverterStartupTimer.pastTime() >= 5000){
            motorController->setState(Disable::getInstance());
            std::vector<int> inverterPositions = getFalseElementPositionsM(asArray(&motorController->getInverterData()->inverterOn_FL),motorController->getSettings()->motorEnable);
            for(int inverterPosition : inverterPositions){
                ECU_WARN(" Inverter {} turned off. Change to Disable state", Positions[inverterPosition]);
            }
            if(inverterStartupTimer.isActive()){
                inverterStartupTimer.stop();
            }
        }
    }else if(motorController->getPedalData()->throttle > motorController->getSettings()->recuperationLevel && !motorController->getPedalData()->breakPressed){
        motorController->setState(Drive::getInstance());
    }
}

void Recuperate::exitState(MotorController *motorController) { PROFILING_FUNCTION();

}

std::string Recuperate::toString() { PROFILING_FUNCTION();
    return "Recuperate";
}

State<MotorController>& Recuperate::getInstance() { PROFILING_FUNCTION();
    static Recuperate instance;
    return instance;
}

// Error State ######################################################################################################
void Error::enterState(MotorController *motorController) { PROFILING_FUNCTION();
    motorController->getMotorControllerData()->currentState = ErrorState;

    CAN_data_out_t& dataOut = *motorController->getCANOutput();
    for (int i = 0; i < 4; ++i) {
        if (motorController->getSettings()->motorEnable[i]){
            asArray(&dataOut.out_targetTorque_FL)[i] = 0;
            asArray(&dataOut.out_inverterEnable_FL)[i] = false;
            asArray(&dataOut.out_inverterOn_FL)[i] = false;
            asArray(&dataOut.out_inverterDCon_FL)[i] = false;
        }
    }
    dataOut.out_torqueLimitPos = 0;
    dataOut.out_torqueLimitNeg = 0;
    dataOut.out_prechargeRelay = false;
    dataOut.out_AIRPositive = false;
    dataOut.out_AIRNegative = false;

    motorController->getMotorControllerData()->TSLed = false;
    motorController->getMotorControllerData()->RTDLed = false;
}
void Error::stateAction(MotorController* motorController) { PROFILING_FUNCTION();

}

void Error::updateState(MotorController* motorController) { PROFILING_FUNCTION();
    if (motorController->getSensorData()->SDC_Pressed) {
        motorController->setState(Disable::getInstance());
    }else if(motorController->getLedButtonData()->RTDButtonPressed || motorController->getLedButtonData()->TSButtonPressed) {
        if(errorResetTimer.isStopped()){
            errorResetTimer.start();
            for (int i = 0; i < 4; ++i){
                if (motorController->getSettings()->motorEnable[i]){
                    asArray(&motorController->getCANOutput()->out_inverterErrorReset_FL)[i] = true;
                }
            }
        }
        if(errorResetTimer.pastTime() >= 4000){
            motorController->setState(Disable::getInstance());
        }
    }else{
        errorResetTimer.stop();
        for (int i = 0; i < 4; ++i) {
            if (motorController->getSettings()->motorEnable[i]){
                asArray(&motorController->getCANOutput()->out_inverterErrorReset_FL)[i] = false;
            }
        }
    }
}

void Error::exitState(MotorController *motorController) { PROFILING_FUNCTION();
    errorResetTimer.stop();
}

std::string Error::toString() { PROFILING_FUNCTION();
    return "Error";
}

State<MotorController>& Error::getInstance() { PROFILING_FUNCTION();
    static Error instance;
    return instance;
}

// Overheat State ######################################################################################################
void Overheat::enterState(MotorController *motorController) { PROFILING_FUNCTION();
    motorController->getMotorControllerData()->currentState = OverheatState;

    CAN_data_out_t& dataOut = *motorController->getCANOutput();
    for (int i = 0; i < 4; ++i) {
        if (motorController->getSettings()->motorEnable[i]){
            asArray(&dataOut.out_targetTorque_FL)[i] = 0;
        }
    }
    dataOut.out_torqueLimitPos = 0;
    dataOut.out_torqueLimitNeg = 0;

    motorController->getMotorControllerData()->TSLed = false;
    motorController->getMotorControllerData()->RTDLed = false;
}
void Overheat::stateAction(MotorController* motorController) { PROFILING_FUNCTION();
    CAN_data_out_t& dataOut = *motorController->getCANOutput();
    for (int i = 0; i < 4; ++i){
        if (motorController->getSettings()->motorEnable[i]){
            //asArray(&dataOut.out_inverterErrorReset_FL)[i] = true;
        }
    } 
    dataOut.out_torqueLimitPos = 0;
    dataOut.out_torqueLimitNeg = 0;

    motorController->getMotorControllerData()->TSLed = false;
    motorController->getMotorControllerData()->RTDLed = false;

}

void Overheat::updateState(MotorController* motorController) { PROFILING_FUNCTION();
    if (motorController->getSensorData()->SDC_Pressed) {
        motorController->setState(Disable::getInstance());
    }else if(motorController->getLedButtonData()->RTDButtonPressed|| motorController->getLedButtonData()->TSButtonPressed) {
        //motorController->setState(Disable::getInstance());
    }
}

void Overheat::exitState(MotorController *motorController) { PROFILING_FUNCTION();

}

std::string Overheat::toString() { PROFILING_FUNCTION();
    return "Overheat";
}

State<MotorController>& Overheat::getInstance() { PROFILING_FUNCTION();
    static Overheat instance;
    return instance;
}

// InverterErrorReset State ######################################################################################################
void InverterErrorReset::enterState(MotorController *motorController) { PROFILING_FUNCTION();
    motorController->getMotorControllerData()->currentState = InverterErrorResetState;

    resetTimer.start();

}
void InverterErrorReset::stateAction(MotorController* motorController) { PROFILING_FUNCTION();
    CAN_data_out_t& dataOut = *motorController->getCANOutput();
    resetSuccessFlag = true;
    for (int i = 0; i < 4; ++i) {
        if (asArray(&motorController->getInverterData()->inverterError_FL)[i]){
            asArray(&dataOut.out_targetTorque_FL)[i] = 0;
            asArray(&dataOut.out_inverterErrorReset_FL)[i] = true;
            resetSuccessFlag = false;
        }
    }
}

void InverterErrorReset::updateState(MotorController* motorController) { PROFILING_FUNCTION();
    if (motorController->getSensorData()->SDC_Pressed) {
        motorController->setState(Disable::getInstance());
    }else if(resetTimer.pastTime() >= MAX_RESET_TIME_500ms){
        ECU_ERROR(INVERTER_RESET_ERROR," Inverter error reset was not successful. Change to Error state");
        motorController->setState(Error::getInstance());
        resetTimer.stop();
    } else if(resetSuccessFlag){
        try{
            motorController->setState(*motorController->getOldState());

        }catch (std::exception& e){
            ECU_ERROR(MOTOR_STATE_ERROR,"Error reset was successful but could not change in to old state: {}. change to Disable State", e.what());
            motorController->setState(Disable::getInstance());
        }
        resetTimer.stop();
    }
}

void InverterErrorReset::exitState(MotorController *motorController) { PROFILING_FUNCTION();

}

std::string InverterErrorReset::toString() { PROFILING_FUNCTION();
    return "InverterErrorReset";
}

State<MotorController>& InverterErrorReset::getInstance() { PROFILING_FUNCTION();
    static InverterErrorReset instance;
    return instance;
}

