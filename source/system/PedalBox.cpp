#include "log.hpp"
#include "system/PedalBox.hpp"
#include "system/PedalBoxStates.hpp"
#include "Profiling.hpp"

#define MAX_BREAK_PRESSURE 500
#define MIN_BREAK_PRESSURE 168

PedalBox::PedalBox(const sensor_data_t* sensorData, system_data_t* systemData, const settings_t* settings){ PROFILING_FUNCTION();
    /* PedalOff state set as init state */
    this->sensorData = sensorData;
    this->pedalData = &sensorData->pedal;
    this->pedalBoxData = &systemData->pedalBox;
    this->accuManagementData = &systemData->accuManagementBox;
    this->canOutput = &systemData->CANOutput;
    this->settings = settings;

    this->currentState = &PedalOff::getInstance();
    ECU_INFO("PedalBox: Init-state PedalOff");

    updateTimer.start();
}


void PedalBox::setState(State<PedalBox> &newState) { PROFILING_FUNCTION();
    ECU_INFO("PedalBox: State changed to {}", newState.toString());
    currentState->exitState(this);  // exit current state
    currentState = &newState;      // change state
    currentState->enterState(this); // enter new state
}

State<PedalBox> *PedalBox::getCurrentState() const { PROFILING_FUNCTION();
    return currentState;
}

void PedalBox::update(int updateRate) { PROFILING_FUNCTION();
    if(updateTimer.pastTime() >= updateRate){
        updateTimer.restart();

        //update PedalBox
        //? Brake error does not need to be checked, the 2024 EV has only one break pressure sensor
        //? checkBreakForErrors();
        checkIfBreakPressed();
        currentState->stateAction(this);
        currentState->updateState(this);
        
    
    }

}

void PedalBox::checkBreakForErrors(){ PROFILING_FUNCTION();
    if(!pedalBoxData->breakError) {
        // Check if break pressure is out of range
        if (pedalData->breakPressureFront > MAX_BREAK_PRESSURE || pedalData->breakPressureFront < MIN_BREAK_PRESSURE) {
            pedalBoxData->breakError = true;
            ECU_ERROR(BREAK_FRONT_OUT_OF_RANGE_ERROR,"Front break pressure sensor is out of range");
        }
        if (pedalData->breakPressureRear > MAX_BREAK_PRESSURE || pedalData->breakPressureRear < MIN_BREAK_PRESSURE) {
            pedalBoxData->breakError = true;
            ECU_ERROR(BREAK_REAR_OUT_OF_RANGE_ERROR,"Rear break pressure sensor is out of range");
        }

        // Convert break pressure to percentage
        double breakPercentageFront = (pedalData->breakPressureFront - MIN_BREAK_PRESSURE) /
                                   ((MAX_BREAK_PRESSURE - MIN_BREAK_PRESSURE) / 100.0);
        double breakPercentageRear =(pedalData->breakPressureRear - MIN_BREAK_PRESSURE) /
                                    ((MAX_BREAK_PRESSURE - MIN_BREAK_PRESSURE) / 100.0);
        pedalBoxData->breakPercentage = (breakPercentageFront + breakPercentageRear) / 2;

        // Check for break Pressure difference
        if (breakPercentageFront < (0.8 * breakPercentageRear) || breakPercentageFront > (1.2 * breakPercentageRear)) {
            pedalBoxData->breakError = true;
            ECU_ERROR(BREAK_PRESSURE_DEVIATION_ERROR,"Break pressure difference is too high. Check break system!");
        }
    }else if(sensorData->SDC_Pressed){
        pedalBoxData->breakError = false;
    }
}

void PedalBox::checkIfBreakPressed(){ PROFILING_FUNCTION();

//? Brake error does not need to be checked, the 2024 EV has only one break pressure sensor
//?    if(!pedalBoxData->breakError){
//?        
//?        if(pedalBoxData->breakPercentage > 10){
//?            pedalBoxData->breakPressed = true;
//?            canOutput->out_breakPressed = true;
//?        } else{
//?            pedalBoxData->breakPressed = false;
//?            canOutput->out_breakLightOn = false;
//?        }
//?    }else{
//?        pedalBoxData->breakPressed = false;
//?        canOutput->out_breakLightOn = false;
//?        pedalBoxData->breakPercentage = 0;
//?    }

    //? The 2024 EV has only one break pressure sensor this code is only needet for one sensor
    //break presure to percentage
    int8_t breakPercentageFront =(pedalData->breakPressureFront - MIN_BREAK_PRESSURE) /
                                    ((MAX_BREAK_PRESSURE - MIN_BREAK_PRESSURE) / 100);
 
    if(breakPercentageFront < 0){
        breakPercentageFront = 0;
    }  //? for debugging
       //ECU_INFO("breakPercentageFront: {}, breakPresure {}", breakPercentageFront,pedalData->breakPressureFront);
    if(breakPercentageFront > 10){
        pedalBoxData->breakPressed = true;
        canOutput->out_breakLightOn = true;
        
    } else{
        pedalBoxData->breakPressed = false;
        canOutput->out_breakLightOn = false;
    }
}

const sensor_data_t* PedalBox::getSensorData() const{ PROFILING_FUNCTION();
    return sensorData;
}

const pedal_t* PedalBox::getPedalData() const{ PROFILING_FUNCTION();
    return pedalData;
}


pedal_box_t* PedalBox::getPedalBoxDate(){ PROFILING_FUNCTION();
    return pedalBoxData;
}

const accu_management_box_t* PedalBox::getAccuManagementData() const{ PROFILING_FUNCTION();
    return accuManagementData;
}

const settings_t* PedalBox::getSettings() const{ PROFILING_FUNCTION();
    return settings;
}