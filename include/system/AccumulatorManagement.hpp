

#ifndef ACCUMULATORMANAGEMENT_HPP
#define ACCUMULATORMANAGEMENT_HPP
#include <vector>
#include <numeric>
#include "data/database.hpp"
#include "Timer.hpp"

#define POWER_AVERAGE_TIME 500 /**< Time in ms for the power average calculation */
#define POWER_BUFFER_SIZE POWER_AVERAGE_TIME/ECU_CYCLE_TIME /**< Size of the power buffer */

class AccumulatorManagement {
public:
    AccumulatorManagement(const sensor_data_t* sensorData, system_data_t* systemData, const settings_t* settings);

    void update(int updateRate = 0);
private:

    int32_t powerAverage();

    const accu_t* accuData;
    accu_management_box_t* accuManagementData;
    const settings_t* settings;

    Timer updateTimer;

    std::array<int32_t, POWER_BUFFER_SIZE> powerBuffer;
    int8_t bufferIndex = 0;

};


#endif //ACCUMULATORMANAGEMENT_HPP
