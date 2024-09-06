#include "system/Steering.hpp"


Steering::Steering(const sensor_data_t* sensorData, const driverless_commands_t* dvCommands , system_data_t* systemData):
    dvOn(sensorData->DVon),
    dvCommands(*dvCommands),
    canOut(systemData->CANOutput),
    steeringData(sensorData->steering),
    scdPressed(sensorData->SDC_Pressed){ 
    
    currentState = &SteeringOff::getInstance();
    currentState->enterState(this);
    ECU_INFO("MotorController: Init-state SteeringOff");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    updateTimer.start();
}

void Steering::update(int updateRate){
    if (updateTimer.pastTime() >= updateRate) {
        updateTimer.restart();

        currentState->stateAction(this);
        currentState->updateState(this);
       
    }
}

void Steering::setState(State<Steering>& newState){
    ECU_INFO("Steering: State changed to {}", newState.toString());
    currentState->exitState(this);  // exit current state
    currentState = &newState;      // change state
    currentState->enterState(this); // enter new state
}

State<Steering>* Steering::getCurrentState() const{
    return currentState;
}   

CAN_data_out_t& Steering::getCanOut(void){
    return canOut;
}

const driverless_commands_t& Steering::getDriverlessCommands(void){
    return dvCommands;
}

const steering_t& Steering::getSteeringData(void){
    return steeringData;
}

const bool& Steering::DVIsOn(void){
    return dvOn;
}

const bool& Steering::scdIsPressed(void){
    return scdPressed;
}