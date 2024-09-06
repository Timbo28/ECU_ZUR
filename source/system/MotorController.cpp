#include "log.hpp"
#include "system/MotorController.hpp"
#include "system/MotorControllerStates.hpp"
#include "Profiling.hpp"
#include "system/arrayCompareLogic.hpp"

// Macros -----------------------------------------------------------------------
#define MAX_TEMP 1200 /**< Maximum temperature for motor, inverter and IGBT 120°C */

MotorController::MotorController(const sensor_data_t* sensorData, system_data_t* systemData, settings_t* settings) { PROFILING_FUNCTION();
    checkStructArrayOrder();
    this->sensorData = sensorData;
    this->inverterData = &sensorData->inverter;
    this->accuData = &sensorData->accu;
    this->accuManagementBox = &systemData->accuManagementBox;

    this->CANOutput = &systemData->CANOutput;
    this->motorControllerData = &systemData->motorControllerBox;
    this->ledButtonData = &systemData->ledButtonBox;
    this->pedalData = &systemData->pedalBox;

    this->settings = settings;

    this->currentState = &Disable::getInstance();
    currentState->enterState(this);
    ECU_INFO("MotorController: Init-state Disable");
    updateTimer.start();
}

void MotorController::update(int updateRate) {
    if (updateTimer.pastTime() >= updateRate) {
        updateTimer.restart();

        //update MotorController
        overheatCheck();
        //inverterErrorCheck();
        
        currentState->stateAction(this);
        currentState->updateState(this);
        dischargeCheck();

    }
}

void MotorController::setState(State<MotorController>& newState) { PROFILING_FUNCTION();
    ECU_INFO("MotorController: State changed to {}", newState.toString());
    currentState->exitState(this);
    currentState = &newState;
    currentState->enterState(this);
}

State<MotorController>* MotorController::getCurrentState() const { PROFILING_FUNCTION();
    return currentState;
}

CAN_data_out_t* MotorController::getCANOutput() {
    return CANOutput;
}
motor_controller_box_t* MotorController::getMotorControllerData() { PROFILING_FUNCTION();
    return motorControllerData;
}

const led_button_box_t* MotorController::getLedButtonData() { PROFILING_FUNCTION();
    return ledButtonData;
}

const pedal_box_t* MotorController::getPedalData() { PROFILING_FUNCTION();
    return pedalData;
}

const sensor_data_t* MotorController::getSensorData() const { PROFILING_FUNCTION();
    return sensorData;
}

const inverter_t* MotorController::getInverterData() const { PROFILING_FUNCTION();
    return inverterData;
}

const accu_t* MotorController::getAccuData() const { PROFILING_FUNCTION();
    return accuData;
}

const accu_management_box_t* MotorController::getAccuManagementData() const { PROFILING_FUNCTION();
    return accuManagementBox;
}

const settings_t* MotorController::getSettings() const { PROFILING_FUNCTION();
    return settings;
}

void MotorController::overheatCheck() { PROFILING_FUNCTION();

    if(getCurrentState() != &Error::getInstance() && getCurrentState()!= &Disable::getInstance()){
        bool overheat = false;
        // check if Temperature of motorTemp, inverterTemp or IGBTTemp is above 120°C and if detect which one.
        for (int i = 0; i < 4; ++i) {
            if(asArray(&inverterData->motorTemp_FL)[i] > MAX_TEMP) {
                overheat = true;
                ECU_ERROR(MOTOR_OVERHEAT_ERROR,"Motor {} temperature is too high ({} °C, MAX Temp = 120 °C).", Positions[i], asArray(&inverterData->motorTemp_FL)[i]/10.)
            }
            if(asArray(&inverterData->inverterTemp_FL)[i] > MAX_TEMP) {
                overheat = true;
                ECU_ERROR(INVERTER_OVERHEAT_ERROR,"Inverter {} temperature is too high ({} °C, MAX Temp = 120 °C).", Positions[i], asArray(&inverterData->inverterTemp_FL)[i]/10.);
            }
            if(asArray(&inverterData->IGBTTemp_FL)[i] > MAX_TEMP) {
                overheat = true;
                ECU_ERROR(IGBT_OVERHEAT_ERROR,"IGBT {} temperature is too high ({} °C, MAX Temp = 120 °C).", Positions[i], asArray(&inverterData->IGBTTemp_FL)[i]/10.);
            }
        }
        if(overheat) {
            setState(Error::getInstance());
        }
    }

    // log the highest temperature of the motor, inverter and IGBT
    for(int i = 0; i < 4; ++i) {
        if(asArray(&motorControllerData->maxMotorTemp_FL)[i] < asArray(&inverterData->motorTemp_FL)[i]) {
            asArray(&motorControllerData->maxMotorTemp_FL)[i] = asArray(&inverterData->motorTemp_FL)[i];
            if(asArray(&motorControllerData->maxMotorTemp_FL)[i] > motorControllerData->maxMotorTemp) {
                motorControllerData->maxMotorTemp = asArray(&motorControllerData->maxMotorTemp_FL)[i];
                //ECU_TRACE("New max motor temperature {}: {} °C",Positions[i], motorControllerData->maxMotorTemp/10.);
            }
        }
        if(asArray(&motorControllerData->maxInverterTemp_FL)[i] < asArray(&inverterData->inverterTemp_FL)[i]) {
            asArray(&motorControllerData->maxInverterTemp_FL)[i] = asArray(&inverterData->inverterTemp_FL)[i];
            if(asArray(&motorControllerData->maxInverterTemp_FL)[i] > motorControllerData->maxInverterTemp) {
                motorControllerData->maxInverterTemp = asArray(&motorControllerData->maxInverterTemp_FL)[i];
                //ECU_TRACE("New max inverter temperature {}: {} °C",Positions[i], motorControllerData->maxInverterTemp/10.);
            }
        }
        if(asArray(&motorControllerData->maxIGBTTemp_FL)[i] < asArray(&inverterData->IGBTTemp_FL)[i]) {
            asArray(&motorControllerData->maxIGBTTemp_FL)[i] = asArray(&inverterData->IGBTTemp_FL)[i];
            if(asArray(&motorControllerData->maxIGBTTemp_FL)[i] > motorControllerData->maxIGBTTemp) {
                motorControllerData->maxIGBTTemp = asArray(&motorControllerData->maxIGBTTemp_FL)[i];
                //ECU_TRACE("New max IGBT temperature {}: {} °C",Positions[i], motorControllerData->maxIGBTTemp/10.);
            }
        }
    }



}

void MotorController::inverterErrorCheck() { PROFILING_FUNCTION();
    // check if inverter has an error
    if (getCurrentState() != &InverterErrorReset::getInstance() && getCurrentState() != &Error::getInstance()) {
        try {
            for (int i = 0; i < 4; ++i) {
                if (asArray(&inverterData->inverterError_FL)[i] && getSettings()->motorEnable[i]) {
                    throw std::runtime_error("Inverter " + Positions[i] + " detected an error and is reset.");
                }
            }
        } catch (std::exception &e) {
            ECU_WARN("MotorController: {}", e.what());
            saveOldState(getCurrentState());
            setState(InverterErrorReset::getInstance());
        }
    }
}
//TODO change old state to a pointer not a string
void MotorController::saveOldState(State<MotorController> *oldState) { PROFILING_FUNCTION();
    oldStateName = oldState->toString();
}

State<MotorController>* MotorController::getOldState() const { PROFILING_FUNCTION();
    if(oldStateName == "Disable") {
        return &Disable::getInstance();
    } else if(oldStateName == "Precharge") {
        return &Precharge::getInstance();
    } else if(oldStateName == "HVCoupling") {
        return &HVCoupling::getInstance();
    } else if(oldStateName == "ReadyToDrive") {
        return &ReadyToDrive::getInstance();
    } else if(oldStateName == "Drive") {
        return &Drive::getInstance();
    }else if(oldStateName == "Recuperate") {
        return &Recuperate::getInstance();
    } else if(oldStateName == "Overheat") {
        return &Overheat::getInstance();
    } else if(oldStateName == "InverterErrorReset") {
        return &InverterErrorReset::getInstance();
    } else if(oldStateName == "Error") {
        return &Error::getInstance();
    } else {
        throw std::runtime_error("MotorController: Old state not found.");
    }
}

void MotorController::dischargeCheck() { PROFILING_FUNCTION();
    // checks if the discharge relay needs to be closed
    accu_t accu = sensorData->accu;
    //ECU_WARN("{} {} {}",accu.prechargeRelay ,accu.AIRPositive,accu.AIRNegative);
    if(!accu.prechargeRelay && !accu.AIRPositive && !accu.AIRNegative) {
        CANOutput->out_dischargeRelay = false;
        
    } else {
        CANOutput->out_dischargeRelay = true;
        //ECU_INFO("no discharge");
    }

}
