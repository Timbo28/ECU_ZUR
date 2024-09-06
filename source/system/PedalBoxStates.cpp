#include "log.hpp"
#include "system/PedalBoxStates.hpp"
#include "system/throttleLogic.hpp"
#include "Profiling.hpp"


//class PedalOff ########################################################

void PedalOff::enterState(PedalBox *pedalBox) { PROFILING_FUNCTION();
    pedalBox->getPedalBoxDate()->throttle = 0;
    pedalBox->getPedalBoxDate()->APPSError = false;

}

void PedalOff::stateAction(PedalBox *pedalBox) { PROFILING_FUNCTION();

}


void PedalOff::updateState(PedalBox *pedalBox) { PROFILING_FUNCTION();
    if(!pedalBox->getSensorData()->SDC_Pressed){
        pedalBox->setState(PedalOn::getInstance());
    }
}

void PedalOff::exitState(PedalBox *pedalBox) { PROFILING_FUNCTION();

}


State<PedalBox> &PedalOff::getInstance() { PROFILING_FUNCTION();
    static PedalOff singleton;
    return singleton;
}


std::string PedalOff::toString() { PROFILING_FUNCTION();
    return "PedalOff";
}


//class PedalOn ##########################################################
void PedalOn::enterState(PedalBox *pedalBox) { PROFILING_FUNCTION();

}

void PedalOn::stateAction(PedalBox *pedalBox) { PROFILING_FUNCTION();

    try{
        checkForShortage(pedalBox);
        throttle = calculateThrottle(pedalBox);
        pedalBox->getPedalBoxDate()->throttle = throttle;
    
    }catch (const std::exception& exception) {
        failureActive = true;
        ECU_WARN("Throttle has read a wrong value. '{}'",exception.what());
    }
    

}


void PedalOn::updateState(PedalBox *pedalBox) { PROFILING_FUNCTION();
    if(pedalBox->getSensorData()->SDC_Pressed) {
        pedalBox->setState(PedalOff::getInstance());
    }
    if(failureActive){
        failureActive = false;
        pedalBox->setState(PedalFailure::getInstance());
    }
}

void PedalOn::exitState(PedalBox *pedalBox) { PROFILING_FUNCTION();

}


State<PedalBox> &PedalOn::getInstance() { PROFILING_FUNCTION();
    static PedalOn singleton;
    return singleton;
}


std::string PedalOn::toString() { PROFILING_FUNCTION();
    return "PedalOn";
}

//class PedalFailure ######################################################

void PedalFailure::enterState(PedalBox *pedalBox) { PROFILING_FUNCTION();
    failureStateTimer.start();
}
void PedalFailure::stateAction(PedalBox *pedalBox) { PROFILING_FUNCTION();


}


void PedalFailure::updateState(PedalBox *pedalBox) { PROFILING_FUNCTION();
    if(pedalBox->getSensorData()->SDC_Pressed) {
        pedalBox->setState(PedalOff::getInstance());
    }
    try{
        checkForShortage(pedalBox);
        calculateThrottle(pedalBox);

        pedalBox->setState(PedalOn::getInstance());
    }catch (const ecu::EcuException& exception){
        if(failureStateTimer.pastTime() >= APPS_ERROR_TIME){

            pedalBox->setState(PedalError::getInstance());
            ECU_ERROR(exception.getErrorCode() ,"{}",exception.what());
        }
    }catch (const std::exception& exception) {
        ECU_ERROR(ERROR_IS_NOT_EXISTING,"{}",exception.what());
    }

}

void PedalFailure::exitState(PedalBox *pedalBox) { PROFILING_FUNCTION();
    failureStateTimer.stop();
}

State<PedalBox> &PedalFailure::getInstance() { PROFILING_FUNCTION();
    static PedalFailure singleton;
    return singleton;
}


std::string PedalFailure::toString() { PROFILING_FUNCTION();
    return "PedalFailure";
}

//class PedalError ###########################################################
void PedalError::enterState(PedalBox *pedalBox) { PROFILING_FUNCTION();
    pedalBox->getPedalBoxDate()->throttle = 0;
    pedalBox->getPedalBoxDate()->APPSError = true;
}

void PedalError::stateAction(PedalBox *pedalBox) { PROFILING_FUNCTION();

}


void PedalError::updateState(PedalBox *pedalBox) { PROFILING_FUNCTION();
    if(pedalBox->getSensorData()->SDC_Pressed){
        pedalBox->setState(PedalOff::getInstance());
    }
}

void PedalError::exitState(PedalBox *pedalBox) { PROFILING_FUNCTION();

}

State<PedalBox> &PedalError::getInstance() { PROFILING_FUNCTION();
    static PedalError singleton;
    return singleton;
}


std::string PedalError::toString() { PROFILING_FUNCTION();
    return "PedalError";
}

