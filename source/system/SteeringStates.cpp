#include "main.hpp"
#include "system/SteeringStates.hpp"

#define MAX_STEERING_SPEED 32767
#define MAX_STEERING_ACCELERATION 32767 

//SteeringOff ##################################################################
void SteeringOff::enterState(Steering* steering){
    steering->getCanOut().out_steeringPosition = 0;
    steering->getCanOut().out_steeringSpeed = 0;
    steering->getCanOut().out_steeringAcceleration = 0;
}

void SteeringOff::stateAction(Steering* steering){
    
}

void SteeringOff::updateState(Steering* steering){
    if(steering->DVIsOn()){
        steering->setState(SteeringOn::getInstance());
    }else if(steering->getSteeringData().steeringCurrent != 0){
        steering->setState(SteeringError::getInstance());
    }else if(steering->getSteeringData().steeringErrorCode != 0){
        steering->setState(SteeringError::getInstance());
    }
}

void SteeringOff::exitState(Steering* steering){
    
}

std::string SteeringOff::toString(){
    return "SteeringOff";
}

State<Steering>& SteeringOff::getInstance(){
    static SteeringOff instance;
    return instance;
}

//SteeringOn ##################################################################
void SteeringOn::enterState(Steering* steering){
    steering->getCanOut().out_steeringPosition = steering->getSteeringData().steeringPosition *1000;
    steering->getCanOut().out_steeringSpeed = MAX_STEERING_SPEED;
    steering->getCanOut().out_steeringAcceleration = MAX_STEERING_ACCELERATION;
}

void SteeringOn::stateAction(Steering* steering){
    if(lastSteeringPosition != steering->getDriverlessCommands().steeringAngle){
        lastSteeringPosition = steering->getDriverlessCommands().steeringAngle;
        steering->getCanOut().out_steeringPosition = steering->getDriverlessCommands().steeringAngle *10000;
    }
}

void SteeringOn::updateState(Steering* steering){
    if(!steering->DVIsOn()){
        steering->setState(SteeringOff::getInstance());
    }else if(steering->getSteeringData().steeringCurrent == 0){
        if(errorTimer.isStopped()){
            errorTimer.start();
        }
        if(errorTimer.pastTime() >= 100){
            steering->setState(SteeringError::getInstance());
        }
    }else if(steering->getSteeringData().steeringErrorCode != 0){
        steering->setState(SteeringError::getInstance());
    }else{
        errorTimer.stop();
    }
}

void SteeringOn::exitState(Steering* steering){
    errorTimer.stop();
}

std::string SteeringOn::toString(){
    return "SteeringOn";
}

State<Steering>& SteeringOn::getInstance(){
    static SteeringOn instance;
    return instance;
}

//SteeringError ##################################################################

void SteeringError::enterState(Steering* steering){
    steering->getCanOut().out_steeringPosition = 0;
    steering->getCanOut().out_steeringSpeed = 0;
    steering->getCanOut().out_steeringAcceleration = 0;
}

void SteeringError::stateAction(Steering* steering){
    
}

void SteeringError::updateState(Steering* steering){
    if(steering->scdIsPressed()){
        steering->setState(SteeringOff::getInstance());
    }
}

void SteeringError::exitState(Steering* steering){
    
}

std::string SteeringError::toString(){
    return "SteeringError";
}

State<Steering>& SteeringError::getInstance(){
    static SteeringError instance;
    return instance;
}

