#include "main.hpp"
#include "system/AccumulatorManagement.hpp"
#include <math.h>


#define ACCU_CHARGE_LOW 30 /**< Low charge limit of the accumulator in % at which the accu power gets halfed*/
#define ACCU_CHARGE_CRITICAL 20 /**< Critical charge limit of the accumulator in % at which the accu power is set to 0*/
#define ACCU_HIGH_TEMPERATURE 50 /**< High temperature limit of the accumulator in °C at which the accu power gets halfed*/
#define ACCU_CRITICAL_TEMPERATURE 60 /**< Critical temperature limit of the accumulator in °C  at which the accu power is set to 0*/


AccumulatorManagement::AccumulatorManagement(const sensor_data_t* sensorData, system_data_t* systemData, const settings_t* settings) { PROFILING_FUNCTION();
    this->accuData = &sensorData->accu;
    this->accuManagementData = &systemData->accuManagementBox;
    this->settings = settings;

    //powerbuffer init wert = 0
    powerBuffer.fill(0);

    updateTimer.start();
}

void AccumulatorManagement::update(int updateRate) { PROFILING_FUNCTION();
    if (updateTimer.pastTime() >= updateRate) {
        updateTimer.restart();

        //update AccumulatorManagement
        double maxDischargeCurrent;
        double accuVoltage = accuData->HVvoltage/10;
        double accuCurrent = accuData->current/1000 ; // Current in A
        accuManagementData->accuVoltage = static_cast<int>(std::round(accuVoltage));
        accuManagementData->accuPower = static_cast<int>(std::round((accuVoltage * accuCurrent)/1000));
        accuManagementData->averagePower = powerAverage();
        
        
        if(accuManagementData->averagePower > settings->maxPower){
            ECU_CRITICAL(ERROR_IS_NOT_EXISTING ,"accu Power limit exceeded {}!!", accuManagementData->averagePower);
        }
        
        if(accuManagementData->accuPower > accuManagementData->maxPower){
            accuManagementData->maxPower = accuManagementData->accuPower;
            ECU_TRACE("New Max Power: {} , voltage {}, Current {}",accuManagementData->maxPower,accuVoltage,accuCurrent);
        }

        if(accuVoltage > accuManagementData->maxVoltage){
            accuManagementData->maxVoltage = accuVoltage;
            //ECU_TRACE("new Max Voltage: {} ",accuVoltage);
        }

        if(accuCurrent > accuManagementData->maxCurrent){
            accuManagementData->maxCurrent = accuCurrent;
            //ECU_TRACE("new Max Current: {} ",accuCurrent);
        }



        maxDischargeCurrent = ( (settings->maxPower*1000) / accuVoltage);

        if(accuData->accuStateOfCharge < ACCU_CHARGE_CRITICAL){
            maxDischargeCurrent = 0;
        }else if(accuData->accuStateOfCharge < ACCU_CHARGE_LOW){
            maxDischargeCurrent /= 2;
        }

        if(accuData->accuTemperature >= ACCU_CRITICAL_TEMPERATURE) {
            maxDischargeCurrent = 0;
        }else if(accuData->accuTemperature >= ACCU_HIGH_TEMPERATURE){
            maxDischargeCurrent /= 2;
        }

        if (accuCurrent > maxDischargeCurrent) {
            accuManagementData->accuPowerLimit = true;
            //ECU_WARN("accu Power limit !!")
        }else{
            accuManagementData->accuPowerLimit = false;
        }
    }

}

int32_t AccumulatorManagement::powerAverage() { PROFILING_FUNCTION();
    powerBuffer[bufferIndex] = accuManagementData->accuPower;
    bufferIndex = (bufferIndex + 1) % powerBuffer.size();
    return std::accumulate(powerBuffer.begin(), powerBuffer.end(), 0) / powerBuffer.size();
    
}