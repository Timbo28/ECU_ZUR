#include "System.hpp"



System::System(Data* data): 
        dvOn(data->getSensorData()->DVon),
        data(data),
        motorController(std::make_unique<MotorController>(data->getSensorData(), &systemData, data->getSettings())),
        pedalBox(std::make_unique<PedalBox>(data->getSensorData(), &systemData, data->getSettings())),
        dvPedalBox(std::make_unique<DV_PedalBox>(data->getDriverlessCommands(), data->getSettings(), &systemData)),
        accumulatorManagement(std::make_unique<AccumulatorManagement>(data->getSensorData(), &systemData, data->getSettings())),
        ledButtonBox(std::make_unique<LedButtonBox>(data->getSensorData(), &systemData, data->getSettings())),
        steering(std::make_unique<Steering>(data->getSensorData(),data->getDriverlessCommands(), &systemData))   { PROFILING_FUNCTION();

    // Register the CanOutputStruct
    data->registerCanMappingStruct(systemData.CANOutput);
    data->registerDataMappingStruct(systemData,true);
}

void System::update() { PROFILING_FUNCTION();
    /* update methods of all ECU system blocks */
    //* IMPORTANT: The order of the update methods is crucial for the correct functionality of the system.
    accumulatorManagement->update();
    pedalBox->update();
    if(dvOn){
        dvPedalBox->update();
    }
    ledButtonBox->update();
    motorController->update();
    steering->update();

    data->CANTransmit("InverterFrontLeft_out");
    data->CANTransmit("InverterFrontRight_out");
    data->CANTransmit("InverterRearLeft_out");
    data->CANTransmit("InverterRearRight_out");
    data->CANTransmit("HVCB_out");   
    data->CANTransmit("accumulator_out");
    data->CANTransmit("cockpitButtons_out");
    data->CANTransmit("ASSI_out");

    if(steering->getCurrentState() == &SteeringOn::getInstance()){
        data->CANTransmit("steering_out");
    }
}














