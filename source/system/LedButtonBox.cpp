#include <iostream>
#include "system/LedButtonBox.hpp"
#include "Profiling.hpp"
#define CAR_IS_STARTING_UP true

LedButtonBox::LedButtonBox(const sensor_data_t* sensorData, system_data_t* systemData, const settings_t* settings) { PROFILING_FUNCTION();
    this->cockpitData = &sensorData->cockpit;
    this->ledButtonData = &systemData->ledButtonBox;
    this->CANoutputData = &systemData->CANOutput;
    this->motorControllerData = &systemData->motorControllerBox;
    this->sensorData = sensorData;

    updateTimer.start();
}

void LedButtonBox::update(int updateRate, bool carIsStartingUp) { PROFILING_FUNCTION();
    if (updateTimer.pastTime() >= updateRate) {
        updateTimer.restart();

        if (carIsStartingUp) {
            dashboardLedControl(CAR_IS_STARTING_UP);
            ASSILedControl(CAR_IS_STARTING_UP);
        }else {
            dashboardLedControl();
            ASSILedControl();
            buttonControl();
        }
    }

}

void LedButtonBox::dashboardLedControl(bool carIsStartingUp) { PROFILING_FUNCTION();
    static Timer TSBlinkTimer(3000);
    if (carIsStartingUp) {
        // RTD and TS LEDs blink 0.5Hz (1.5s on, 0.5s off)
        makeLedBlink(1500, 500, RTD_and_TS_LED_sync);

    }else if (!cockpitData->RTDButton && !cockpitData->TSButton) {

        if (motorControllerData->currentState == OverheatState){
            // Overheat Led: RTD Led is blinking 4Hz (0.125s on, 0.125s off)
            makeLedBlink(125, 125, RTD_LED_only);
        }else if (motorControllerData->currentState == ErrorState){
            // Error Led: TS Led is blinking 4Hz (0.125s on, 0.125s off)
            makeLedBlink(125, 125, TS_LED_only);
        }else {
            if ((motorControllerData->currentState == DisableState && sensorData->SDC_Pressed)) {

                switch (motorControllerData->currentDriveMode) {
                    case normalMode:
                        // Normal mode: led will be controlled by the motor controller
                        CANoutputData->out_RTDLed = false;
                        CANoutputData->out_TSLed = false;
                        break;
                    case slowMode:
                        // Slow mode: TS and RTD Leds are blinking alternately 1Hz (0.5s on, 0.5s off)
                        makeLedBlink(500, 500, RTD_and_TS_LED_async);
                        break;
                    case enduranceMode:
                        // Endurance mode: RTD and TS Led are blinking at 0.5Hz (1s on, 1s off)
                        CANoutputData->out_RTDLed = true;
                        CANoutputData->out_TSLed = true;
                        break;
                }
            } else {
                // Led will be controlled by the motor controller
                //!debug 
                //ECU_DEBUG("TS Led: {}, motorcomand:{}",CANoutputData->out_TSLed, motorControllerData->TSLed);
                CANoutputData->out_RTDLed = motorControllerData->RTDLed;
                CANoutputData->out_TSLed = motorControllerData->TSLed;
            }
        }
    }else {
        // buttons control the LEDs
        CANoutputData->out_RTDLed = cockpitData->RTDButton;
        CANoutputData->out_TSLed = cockpitData->TSButton;
    }
}

void LedButtonBox::makeLedBlink(int onTime, int offTime, led_blink_combination_t combination) { PROFILING_FUNCTION();
    static Timer BlinkTimer(3000);
    auto pastTime = BlinkTimer.pastTime();
    // Make the LED blink with the given combination
        if (BlinkTimer.isStopped()) {
            BlinkTimer.start();
            blinkCombination(true, combination);
        }

        if(pastTime >= (onTime+offTime)) {
            BlinkTimer.stop();
        }else if (pastTime > onTime) {
        blinkCombination(false, combination);
        }
}

void LedButtonBox::blinkCombination(bool value, led_blink_combination_t combination) { PROFILING_FUNCTION();
    static led_blink_combination_t lastCombination = TS_LED_only;
    static bool lastState = false;
    if(value != lastState) {
        lastState = value;
        if (combination != lastCombination) {
            lastCombination = combination;
            CANoutputData->out_TSLed = false;
            CANoutputData->out_RTDLed = false;
        }

        switch (combination) {
            case TS_LED_only:
                CANoutputData->out_TSLed = value;
                break;
            case RTD_LED_only:
                CANoutputData->out_RTDLed = value;
                break;
            case RTD_and_TS_LED_sync:
                CANoutputData->out_TSLed = value;
                CANoutputData->out_RTDLed = value;
                break;
            case RTD_and_TS_LED_async:
                CANoutputData->out_TSLed = value;
                CANoutputData->out_RTDLed = !value;
                break;
        }
    }
}


void LedButtonBox::ASSILedControl(bool carIsStartingUp) { PROFILING_FUNCTION();
    // Control the ASSI LEDs
    if (carIsStartingUp) {
        // ASSI LEDs are white
        CANoutputData->out_ASSIred = true;
        CANoutputData->out_ASSIgreen = true;
        CANoutputData->out_ASSIblue = true;
    }else if(sensorData->DVon) {
        // ASSI LEDs are yellow
        CANoutputData->out_ASSIred = true;
        CANoutputData->out_ASSIgreen = true;
        CANoutputData->out_ASSIblue = false;
    }else {
        // ASSI LEDs are off
        CANoutputData->out_ASSIred = false;
        CANoutputData->out_ASSIgreen = false;
        CANoutputData->out_ASSIblue = false;
    }
}

void LedButtonBox::buttonControl() { PROFILING_FUNCTION();
    static Timer TSButtonPressTimer;
    static Timer RTDButtonPressTimer;
    // Delays the TS button press for 50ms to avoid bouncing
    if (cockpitData->TSButton){
        if (TSButtonPressTimer.isStopped()) {
            TSButtonPressTimer.start();
        }
        if (TSButtonPressTimer.pastTime() >= 50) {
            ledButtonData->TSButtonPressed = true;
            TSButtonPressTimer.stop();
        }
    }else {
        ledButtonData->TSButtonPressed = false;
        TSButtonPressTimer.stop();
    }
    // Delays the RTD button press for 50ms to avoid bouncing
    if (cockpitData->RTDButton){
        if (RTDButtonPressTimer.isStopped()) {
            RTDButtonPressTimer.start();
        }
        if (RTDButtonPressTimer.pastTime() >= 50) {
            ledButtonData->RTDButtonPressed = true;
            RTDButtonPressTimer.stop();
        }
    }else {
        ledButtonData->RTDButtonPressed = false;
        RTDButtonPressTimer.stop();
    }
}