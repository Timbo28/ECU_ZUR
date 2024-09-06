//
// Created by timro on 19/02/2024.
//

#ifndef LEDBUTTONCONTROLLER_HPP
#define LEDBUTTONCONTROLLER_HPP
#include  "data/database.hpp"
#include "Timer.hpp"

enum led_blink_combination_t{
    TS_LED_only,
    RTD_LED_only,
    RTD_and_TS_LED_sync,
    RTD_and_TS_LED_async
};

class LedButtonBox {
public:
    LedButtonBox(const sensor_data_t* sensorData, system_data_t* systemData, const settings_t* settings);

    void update(int updateRate =0 , bool carIsStartingUp = false);

private:
    void dashboardLedControl(bool carIsStartingUp = false);
    void ASSILedControl(bool carIsStartingUp = false);
    void buttonControl();

    void makeLedBlink(int onTime, int offTime,led_blink_combination_t combination);
    void blinkCombination(bool value,led_blink_combination_t combination);

    const cockpit_t* cockpitData;
    led_button_box_t* ledButtonData;
    CAN_data_out_t* CANoutputData;
    const motor_controller_box_t* motorControllerData;
    const sensor_data_t* sensorData;

    Timer updateTimer;


};


#endif //LEDBUTTONCONTROLLER_HPP
