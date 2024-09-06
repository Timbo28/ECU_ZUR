#include <thread>
#include "gtest/gtest.h"
#include "system/LedButtonBox.hpp"
#include "data/database.hpp"
#include "Timer.hpp"

#define CAR_IS_STARTING_UP true

class LedButtonBoxTest : public ::testing::Test {
protected:
       void SetUp() override {
            sensorData.cockpit.TSButton = false;
            sensorData.cockpit.RTDButton = false;

            ledButtonBox = new LedButtonBox(&sensorData, &systemData, &settings);
        }

        void TearDown() override {
            delete ledButtonBox;
        }
    settings_t settings;
    sensor_data_t sensorData;
    system_data_t systemData;
    LedButtonBox* ledButtonBox;

};

// Test ButtonControl function ############################################################################################################
// Test if CAR is starting up turns off all buttons
TEST_F(LedButtonBoxTest, Button_Control_Start_up_Test) {
    sensorData.cockpit.TSButton = true;
    sensorData.cockpit.RTDButton = true;

    ledButtonBox->update(CAR_IS_STARTING_UP);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ledButtonBox->update(CAR_IS_STARTING_UP);
    EXPECT_FALSE(systemData.ledButtonBox.TSButtonPressed);
    EXPECT_FALSE(systemData.ledButtonBox.RTDButtonPressed);
}

// Test TS button delay of 50ms
TEST_F(LedButtonBoxTest, TS_Button_Delay_Test) {
    sensorData.cockpit.TSButton = true;
    ledButtonBox->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ledButtonBox->update();
    EXPECT_FALSE(systemData.ledButtonBox.TSButtonPressed);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    ledButtonBox->update();


    EXPECT_TRUE(systemData.ledButtonBox.TSButtonPressed);
}

// Test RTD button delay of 50ms
TEST_F(LedButtonBoxTest, RTD_Button_Delay_Test) {
    sensorData.cockpit.RTDButton = true;
    ledButtonBox->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ledButtonBox->update();
    EXPECT_FALSE(systemData.ledButtonBox.RTDButtonPressed);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    ledButtonBox->update();

    EXPECT_TRUE(systemData.ledButtonBox.RTDButtonPressed);
}

// Test multiple button presses
TEST_F(LedButtonBoxTest, Multiple_Button_Presses_Test) {
    sensorData.cockpit.TSButton = true;
    ledButtonBox->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ledButtonBox->update();
    EXPECT_TRUE(systemData.ledButtonBox.TSButtonPressed);
    sensorData.cockpit.TSButton = false;
    ledButtonBox->update();
    EXPECT_FALSE(systemData.ledButtonBox.TSButtonPressed);

    sensorData.cockpit.RTDButton = true;
    ledButtonBox->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ledButtonBox->update();
    EXPECT_FALSE(systemData.ledButtonBox.RTDButtonPressed);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    ledButtonBox->update();
    EXPECT_TRUE(systemData.ledButtonBox.RTDButtonPressed);
}

// Short Button press ( 20ms )
TEST_F(LedButtonBoxTest, Short_Button_Press_Test) {
    sensorData.cockpit.TSButton = true;
    ledButtonBox->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    sensorData.cockpit.TSButton = false;
    ledButtonBox->update();
    EXPECT_FALSE(systemData.ledButtonBox.TSButtonPressed);
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}