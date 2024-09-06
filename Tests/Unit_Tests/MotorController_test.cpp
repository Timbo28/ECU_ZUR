#include "gtest/gtest.h"
#include "system/MotorController.hpp"
#include "system/MotorControllerStates.hpp"
#include "LogManager.hpp"
#include "data/database.hpp"
#include "system/arrayCompareLogic.hpp"


class MotorControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        LogManager::init();
        sensorData = new sensor_data_t();
        systemData = new system_data_t();
        settings = new settings_t();
        motorController = new MotorController(sensorData, systemData, settings);
        sensorData->SDC_Pressed = false;
    }

    void TearDown() override {
        delete sensorData;
        delete systemData;
        delete settings;
        delete motorController;
        LogManager::shutdown();
    }

    sensor_data_t* sensorData;
    system_data_t* systemData;
    settings_t* settings;
    MotorController* motorController;
};


// Overheat Test ######################################################################################################
//overheat in the FL Inverter and IGPT
TEST_F(MotorControllerTest, Overheat_State_Test_1) {
    asArray(&sensorData->inverter.motorTemp_FL)[0] = 790;
    asArray(&sensorData->inverter.inverterTemp_FL)[0] = 810;
    asArray(&sensorData->inverter.IGBTTemp_FL)[0] = 810;
    EXPECT_NE(motorController->getCurrentState(), &Overheat::getInstance());
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Overheat::getInstance());
}
// not overheat in any position
TEST_F(MotorControllerTest, Overheat_State_Test_2) {
    asArray(&sensorData->inverter.motorTemp_FL)[0] = 200;
    asArray(&sensorData->inverter.inverterTemp_FL)[1] = 200;
    asArray(&sensorData->inverter.IGBTTemp_FL)[2] = 200;
    EXPECT_NE(motorController->getCurrentState(), &Overheat::getInstance());
    motorController->update();
    EXPECT_NE(motorController->getCurrentState(), &Overheat::getInstance());
}

// overheat and then cool down
TEST_F(MotorControllerTest, Overheat_State_Test_3) {
    asArray(&sensorData->inverter.motorTemp_FL)[3] = 810;
    asArray(&sensorData->inverter.inverterTemp_FL)[1] = 200;
    asArray(&sensorData->inverter.IGBTTemp_FL)[2] = 200;

    EXPECT_NE(motorController->getCurrentState(), &Overheat::getInstance()); // not in overheat State
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Overheat::getInstance());// in overheat State
    // cool down
    asArray(&sensorData->inverter.motorTemp_FL)[3] = 700;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Overheat::getInstance()); // reset overheat not pressed still in overheat State
    systemData->ledButtonBox.RTDButtonPressed = true; // press RTD button to reset overheat
    motorController->update();
    systemData->ledButtonBox.RTDButtonPressed = false;
    EXPECT_NE(motorController->getCurrentState(), &Overheat::getInstance());// not in overheat State after reset
}

// overheat and reset overheat by pressing RTD button
TEST_F(MotorControllerTest, Overheat_State_Test_4) {
    asArray(&sensorData->inverter.motorTemp_FL)[3] = 800;
    asArray(&sensorData->inverter.inverterTemp_FL)[3] = 900;
    asArray(&sensorData->inverter.IGBTTemp_FL)[2] = 200;

    EXPECT_NE(motorController->getCurrentState(), &Overheat::getInstance()); // not in overheat State
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Overheat::getInstance());// in overheat State
    //wait for 1 seconds
    for (int i = 0; i <100 ; ++i) {
        auto time = std::chrono::system_clock::now();
        motorController->update();
        std::this_thread::sleep_until(time+std::chrono::milliseconds(10));
    }
    // reset overheat
    systemData->ledButtonBox.RTDButtonPressed = true; // press RTD button to reset overheat
    motorController->update();
    systemData->ledButtonBox.RTDButtonPressed = false;
    EXPECT_NE(motorController->getCurrentState(), &Overheat::getInstance());// not in overheat State after reset
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Overheat::getInstance());// in overheat State again
}

// InverterErrorReset Test ######################################################################################################
// inverter error in the FL Inverter
TEST_F(MotorControllerTest, InverterErrorReset_State_Test_1) {
    asArray(&sensorData->inverter.inverterError_FL)[0] = true;
    EXPECT_NE(motorController->getCurrentState(), &InverterErrorReset::getInstance());
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &InverterErrorReset::getInstance());
    //reset signal check
    EXPECT_EQ(motorController->getCANOutput()->out_inverterErrorReset_FL, true);
}
// Error in the RR Inverter
TEST_F(MotorControllerTest, InverterErrorReset_State_Test_2) {
    sensorData->inverter.inverterError_RR = true;
    EXPECT_NE(motorController->getCurrentState(), &InverterErrorReset::getInstance());
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &InverterErrorReset::getInstance());
    //wait for 1 seconds
    for (int i = 0; i <100 ; ++i) {
        auto time = std::chrono::system_clock::now();
        motorController->update();
        std::this_thread::sleep_until(time+std::chrono::milliseconds(10));
    }
    systemData->ledButtonBox.RTDButtonPressed = true; // press RTD button to reset inverter error
    motorController->update();

    EXPECT_EQ(motorController->getCurrentState(), &Error::getInstance());
    EXPECT_EQ(motorController->getCANOutput()->out_inverterErrorReset_RR, true);

}

// Error in the RL Inverter with successful reset
TEST_F(MotorControllerTest, InverterErrorReset_State_Test_3) {
    sensorData->inverter.inverterError_RL= true;
    motorController->setState(Drive::getInstance());
    EXPECT_NE(motorController->getCurrentState(), &InverterErrorReset::getInstance());
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &InverterErrorReset::getInstance());
    //wait for 100ms
    for (int i = 0; i <10 ; ++i) {
        auto time = std::chrono::system_clock::now();
        motorController->update();
        std::this_thread::sleep_until(time+std::chrono::milliseconds(10));
    }
    sensorData->inverter.inverterError_RL = false;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Drive::getInstance());
}

// DischargeRelay Test ######################################################################################################

// Switch on the discharge relay
TEST_F(MotorControllerTest, DischargeRelay_Test_1) {
    sensorData->accu.prechargeRelay = false;
    sensorData->accu.AIRPositive = false;
    sensorData->accu.AIRNegative = false;
    motorController->update();
    EXPECT_EQ(motorController->getCANOutput()->out_dischargeRelay, false);
}

// Switch off the discharge relay
TEST_F(MotorControllerTest, DischargeRelay_Test_2) {
    sensorData->accu.prechargeRelay = false;
    sensorData->accu.AIRPositive = false;
    sensorData->accu.AIRNegative = false;
    motorController->update();
    sensorData->accu.AIRNegative = true;
    motorController->update();
    EXPECT_EQ(motorController->getCANOutput()->out_dischargeRelay, true);
}

// State Disable Switch Mode selection Test ######################################################################################################
// Set slow mode and reset to normal mode
TEST_F(MotorControllerTest, Disable_State_Test_1) {
    sensorData->SDC_Pressed = true;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
    EXPECT_EQ(motorController->getMotorControllerData()->currentDriveMode, normalMode);

    systemData->ledButtonBox.TSButtonPressed = false;
    systemData->ledButtonBox.RTDButtonPressed = true;
    motorController->update();
    EXPECT_EQ(motorController->getMotorControllerData()->currentDriveMode, normalMode);

    std::this_thread::sleep_for(std::chrono::milliseconds (4100));
    motorController->update();
    EXPECT_EQ(motorController->getMotorControllerData()->currentDriveMode, slowMode);

    systemData->ledButtonBox.TSButtonPressed = false;
    systemData->ledButtonBox.RTDButtonPressed = false;
    motorController->update();
    EXPECT_EQ(motorController->getMotorControllerData()->currentDriveMode, slowMode);

    systemData->ledButtonBox.TSButtonPressed = true;
    systemData->ledButtonBox.RTDButtonPressed = true;
    motorController->update();
    EXPECT_EQ(motorController->getMotorControllerData()->currentDriveMode, normalMode);

    systemData->ledButtonBox.TSButtonPressed = false;
    systemData->ledButtonBox.RTDButtonPressed = false;
    motorController->update();
    EXPECT_EQ(motorController->getMotorControllerData()->currentDriveMode, normalMode);
}
// Set endurance mode and reset to normal mode
TEST_F(MotorControllerTest, Disable_State_Test_2) {
    sensorData->SDC_Pressed = true;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
    EXPECT_EQ(motorController->getMotorControllerData()->currentDriveMode, normalMode);

    systemData->ledButtonBox.TSButtonPressed = true;
    systemData->ledButtonBox.RTDButtonPressed = false;
    motorController->update();
    EXPECT_EQ(motorController->getMotorControllerData()->currentDriveMode, normalMode);
    std::this_thread::sleep_for(std::chrono::seconds (4));
    motorController->update();
    EXPECT_EQ(motorController->getMotorControllerData()->currentDriveMode, enduranceMode);

    systemData->ledButtonBox.TSButtonPressed = false;
    systemData->ledButtonBox.RTDButtonPressed = false;
    motorController->update();

    EXPECT_EQ(motorController->getMotorControllerData()->currentDriveMode, enduranceMode);

    systemData->ledButtonBox.TSButtonPressed = true;
    systemData->ledButtonBox.RTDButtonPressed = true;
    motorController->update();
    EXPECT_EQ(motorController->getMotorControllerData()->currentDriveMode, normalMode);

    systemData->ledButtonBox.TSButtonPressed = false;
    systemData->ledButtonBox.RTDButtonPressed = false;
    motorController->update();
    EXPECT_EQ(motorController->getMotorControllerData()->currentDriveMode, normalMode);
}

// Disable Precharge state Test ######################################################################################################
// Change to Precharge state and back to Disable state
TEST_F(MotorControllerTest, Precharge_Disable_State_Test_1) {
    sensorData->SDC_Pressed = false;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
    for (int i = 0; i < 4; ++i) {
        asArray(&sensorData->inverter.inverterSystemReady_FL)[i]= true;
    }
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
    // change to Precharge state
    systemData->ledButtonBox.TSButtonPressed = true;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Precharge::getInstance());
    // Check if the state stays in Precharge even if the TS button is not released (ButtonReleaseFlag test)
    motorController->update();
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Precharge::getInstance());
    // release the TS button
    systemData->ledButtonBox.TSButtonPressed = false;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Precharge::getInstance());
    // Change back to Disable state by pressing the TS button
    systemData->ledButtonBox.TSButtonPressed = true;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
    // Check if the state stays in Disable even if the TS button is not released (ButtonReleaseFlag test)
    motorController->update();
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
    // release the TS button
    systemData->ledButtonBox.TSButtonPressed = false;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
    // Do the same test but use the RTD button to change back to Disable state
    // Change to Precharge state
    systemData->ledButtonBox.TSButtonPressed = true;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Precharge::getInstance());
    // release the TS button
    systemData->ledButtonBox.TSButtonPressed = false;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Precharge::getInstance());
    // Change back to Disable state by pressing the RTD button
    systemData->ledButtonBox.RTDButtonPressed = true;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
    systemData->ledButtonBox.RTDButtonPressed = false;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
}
// SDCok is false and changes to Disable state from Precharge state
TEST_F(MotorControllerTest, Precharge_Disable_State_Test_2) {
    sensorData->SDC_Pressed = false;
    motorController->setState(Precharge::getInstance());
    motorController->update();
    sensorData->SDC_Pressed = true;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
}

// Precharge Error state Test ######################################################################################################
// Check the state changes to Error state from Precharge state if the Precharge lasts more than 7 seconds
TEST_F(MotorControllerTest, Precharge_Error_State_Test_1) {
    sensorData->SDC_Pressed = false;
    motorController->setState(Precharge::getInstance());
    for (int i = 0; i < 700; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds (10));
        motorController->update();
    }
    EXPECT_EQ(motorController->getCurrentState(), &Error::getInstance());
}
// Check if the Precharge circuit is closed for more than 5 seconds and changes to Error state
TEST_F(MotorControllerTest, Precharge_Error_State_Test_2) {
    sensorData->SDC_Pressed = false;
    // Check with closed Precharge Relay
    sensorData->accu.prechargeRelay = 680;
    motorController->setState(Precharge::getInstance());
    for (int i = 0; i < 550; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds (10));
        motorController->update();
    }
    EXPECT_EQ(motorController->getCurrentState(), &Error::getInstance());

    // Check with open AIRNegative Relay
    sensorData->accu.prechargeRelay = 600;// false
    sensorData->accu.AIRNegative = 700;//true
    motorController->setState(Precharge::getInstance());
    for (int i = 0; i < 550; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds (10));
        motorController->update();
    }
    EXPECT_EQ(motorController->getCurrentState(), &Error::getInstance());
}

// Precharge HVCoupling state Test ######################################################################################################
// Check the state changes to HVCoupling state from Precharge state
TEST_F(MotorControllerTest, Precharge_HVCoupling_State_Test_1) {
    sensorData->SDC_Pressed = false;
    motorController->setState(Precharge::getInstance());
    for (int i = 0; i < 4; ++i) {
        asArray(&sensorData->inverter.inverterDCon_FL)[i] = true;
        motorController->update();
    }
    EXPECT_EQ(motorController->getCurrentState(), &HVCoupling::getInstance());
}

// HVCoupling Disable state Test ######################################################################################################
// Check the state changes to Disable state from HVCoupling state
TEST_F(MotorControllerTest, HVCoupling_Disable_State_Test_1) {
    sensorData->SDC_Pressed = false;
    sensorData->accu.AIRPositive = 680;//true
    motorController->setState(HVCoupling::getInstance());
    systemData->motorControllerBox.RTDLed = true;
    motorController->update();
    EXPECT_EQ(systemData->motorControllerBox.RTDLed, false);
    systemData->ledButtonBox.RTDButtonPressed = true;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
}
// SDCok is false and changes to Disable state from HVCoupling state
TEST_F(MotorControllerTest, HVCoupling_Disable_State_Test_2) {
    sensorData->SDC_Pressed = false;
    motorController->setState(HVCoupling::getInstance());
    motorController->update();
    sensorData->SDC_Pressed = true;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
}
// HVCoupling Error state Test ######################################################################################################
// Check the state changes to Error state from HVCoupling state
TEST_F(MotorControllerTest, HVCoupling_Error_State_Test_1) {
    sensorData->SDC_Pressed = false;
    sensorData->accu.prechargeRelay = true;
    sensorData->accu.AIRPositive = true;
    motorController->setState(HVCoupling::getInstance());
    for (int i = 0; i < 300; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds (10));
        motorController->update();
    }
    EXPECT_EQ(motorController->getCurrentState(), &Error::getInstance());

    sensorData->accu.prechargeRelay = false;
    sensorData->accu.AIRPositive = false;
    motorController->setState(HVCoupling::getInstance());
    for (int i = 0; i < 300; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds (10));
        motorController->update();
    }
    EXPECT_EQ(motorController->getCurrentState(), &Error::getInstance());
}

// HVCoupling ReadyToDrive state Test ######################################################################################################
// Check the state changes to ReadyToDrive state from HVCoupling state
TEST_F(MotorControllerTest, HVCoupling_ReadyToDrive_State_Test_1) {
    sensorData->SDC_Pressed = false;
    sensorData->accu.AIRPositive = 680;//true
    motorController->setState(HVCoupling::getInstance());
    motorController->update();
    systemData->pedalBox.breakPressed = true;
    systemData->pedalBox.throttle = 0;
    motorController->update();
    EXPECT_EQ(systemData->motorControllerBox.RTDLed, true);
    motorController->update();
    systemData->ledButtonBox.RTDButtonPressed = true;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &ReadyToDrive::getInstance());
}

// ReadyToDrive Disable state Test ######################################################################################################
// SDCok is false and changes to Disable state from ReadyToDrive state
TEST_F(MotorControllerTest, ReadyToDrive_Disable_State_Test_1) {
    sensorData->SDC_Pressed = false;
    systemData->ledButtonBox.RTDButtonPressed = true;
    motorController->setState(ReadyToDrive::getInstance());
    motorController->update();
    sensorData->SDC_Pressed = true;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
}

// ReadyToDrive Drive state Test ######################################################################################################
// Check the state changes to Drive state from ReadyToDrive state
TEST_F(MotorControllerTest, ReadyToDrive_Drive_State_Test_1) {
    sensorData->SDC_Pressed = false;
    sensorData->accu.AIRPositive = 680;//true
    motorController->setState(HVCoupling::getInstance());
    motorController->update();
    systemData->pedalBox.breakPressed = true;
    systemData->pedalBox.throttle = 0;
    motorController->update();
    systemData->ledButtonBox.RTDButtonPressed = true;
    motorController->update();
    systemData->ledButtonBox.RTDButtonPressed = false;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Drive::getInstance());
}

// Drive Disable state Test ######################################################################################################
// SDCok is false and changes to Disable state from Drive state
TEST_F(MotorControllerTest, Drive_Disable_State_Test_1) {
    sensorData->SDC_Pressed = false;
    for (int i = 0; i < 4; ++i) {
        asArray(&sensorData->inverter.inverterDCon_FL)[i] = true;
        asArray(&sensorData->inverter.inverterOn_FL)[i] = true;
        asArray(&sensorData->inverter.inverterSystemReady_FL)[i] = true;
    }
    motorController->setState(Drive::getInstance());
    motorController->update();
    sensorData->SDC_Pressed = true;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
}
// Press button to change to Disable state from Drive state
TEST_F(MotorControllerTest, Drive_Disable_State_Test_2) {
    sensorData->SDC_Pressed = false;
    for (int i = 0; i < 4; ++i) {
        asArray(&sensorData->inverter.inverterDCon_FL)[i] = true;
        asArray(&sensorData->inverter.inverterOn_FL)[i] = true;
        asArray(&sensorData->inverter.inverterSystemReady_FL)[i] = true;
    }
    // press RTD button to change to Disable state
    motorController->setState(Drive::getInstance());
    motorController->update();
    systemData->ledButtonBox.RTDButtonPressed = true;
    motorController->update();
    systemData->ledButtonBox.RTDButtonPressed = false;
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());

    // press TS button to change to Disable state
    motorController->setState(Drive::getInstance());
    motorController->update();
    systemData->ledButtonBox.TSButtonPressed = true;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());

    // Check if it stays in Disable state even if the button is not released (ButtonReleaseFlag test)
    motorController->update();
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
}

// Check if change to Disable state from Drive state if inverter DC turned off
TEST_F(MotorControllerTest, Drive_Disable_State_Test_3) {
    sensorData->SDC_Pressed = false;
    for (int i = 0; i < 4; ++i) {
        asArray(&sensorData->inverter.inverterDCon_FL)[i] = true;
        asArray(&sensorData->inverter.inverterOn_FL)[i] = true;
        asArray(&sensorData->inverter.inverterSystemReady_FL)[i] = true;
    }
    motorController->setState(Drive::getInstance());
    motorController->update();
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Drive::getInstance());
    // turn off inverter DC rear right
    sensorData->inverter.inverterDCon_RR = false;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
}
// Check if change to Disable state from Drive state if any inverter turned off
TEST_F(MotorControllerTest, Drive_Disable_State_Test_4) {
    sensorData->SDC_Pressed = false;
    for (int i = 0; i < 4; ++i) {
        asArray(&sensorData->inverter.inverterDCon_FL)[i] = true;
        asArray(&sensorData->inverter.inverterOn_FL)[i] = true;
        asArray(&sensorData->inverter.inverterSystemReady_FL)[i] = true;
    }
    motorController->setState(Drive::getInstance());
    std::this_thread::sleep_for(std::chrono::seconds(5));
    motorController->update();
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Drive::getInstance());
    motorController->update();
    motorController->update();
    // turn off inverter front right
    sensorData->inverter.inverterOn_FR = false;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
}
// Check if change to Disable state from Drive state if inverter system not ready
TEST_F(MotorControllerTest, Drive_Disable_State_Test_5) {
    sensorData->SDC_Pressed = false;
    for (int i = 0; i < 4; ++i) {
        asArray(&sensorData->inverter.inverterDCon_FL)[i] = true;
        asArray(&sensorData->inverter.inverterOn_FL)[i] = true;
        asArray(&sensorData->inverter.inverterSystemReady_FL)[i] = true;
    }
    motorController->setState(Drive::getInstance());
    motorController->update();
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Drive::getInstance());
    // turn off inverter system ready front left
    asArray(&sensorData->inverter.inverterSystemReady_FL)[0] = false;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
}
// Check if any Inverter is off for more than 5 seconds during startup Drive state
TEST_F(MotorControllerTest, Drive_Disable_State_Test_6) {
    sensorData->SDC_Pressed = false;
    for (int i = 0; i < 4; ++i) {
        asArray(&sensorData->inverter.inverterDCon_FL)[i] = true;
        asArray(&sensorData->inverter.inverterSystemReady_FL)[i] = true;
    }
    motorController->setState(Drive::getInstance());
    for (int i = 0; i < 500; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds (10));
        motorController->update();
    }
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
}
// Drive Recuperate state Test ######################################################################################################
// Check the state changes to Recuperate state from Drive state
TEST_F(MotorControllerTest, Drive_Recuperate_State_Test_1) {
    sensorData->SDC_Pressed = false;
    for (int i = 0; i < 4; ++i) {
        asArray(&sensorData->inverter.inverterDCon_FL)[i] = true;
        asArray(&sensorData->inverter.inverterOn_FL)[i] = true;
        asArray(&sensorData->inverter.inverterSystemReady_FL)[i] = true;
    }
    settings->recuperationEnabled = true;

    // change to Recuperation state
    motorController->setState(Drive::getInstance());
    systemData->pedalBox.throttle = 0;
    systemData->pedalBox.breakPressed = true;
    motorController->update();
    EXPECT_NE(motorController->getCurrentState(), &Recuperate::getInstance());
    // Wait until sound is finished
    std::this_thread::sleep_for(std::chrono::seconds(2));
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Recuperate::getInstance());

    // change to Drive state
    systemData->pedalBox.throttle = 10;
    systemData->pedalBox.breakPressed = false;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Drive::getInstance());
}

// Recuperate Disable state Test ######################################################################################################
// SDCok is false and changes to Disable state from Recuperate state
TEST_F(MotorControllerTest, Recuperate_Disable_State_Test_1) {
    sensorData->SDC_Pressed = false;
    for (int i = 0; i < 4; ++i) {
        asArray(&sensorData->inverter.inverterDCon_FL)[i] = true;
        asArray(&sensorData->inverter.inverterOn_FL)[i] = true;
        asArray(&sensorData->inverter.inverterSystemReady_FL)[i] = true;
    }
    motorController->setState(Recuperate::getInstance());
    motorController->update();
    sensorData->SDC_Pressed = true;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
}
// Press button to change to Disable state from Recuperate state
TEST_F(MotorControllerTest, Recuperate_Disable_State_Test_2) {
    sensorData->SDC_Pressed = false;
    for (int i = 0; i < 4; ++i) {
        asArray(&sensorData->inverter.inverterDCon_FL)[i] = true;
        asArray(&sensorData->inverter.inverterOn_FL)[i] = true;
        asArray(&sensorData->inverter.inverterSystemReady_FL)[i] = true;
    }
    systemData->ledButtonBox.RTDButtonPressed = false;
    systemData->ledButtonBox.TSButtonPressed = false;
    // press RTD button to change to Disable state
    motorController->setState(Recuperate::getInstance());
    motorController->update();
    systemData->ledButtonBox.RTDButtonPressed = true;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
    systemData->ledButtonBox.RTDButtonPressed = false;
    motorController->update();


    // press TS button to change to Disable state
    motorController->setState(Recuperate::getInstance());
    motorController->update();
    systemData->ledButtonBox.TSButtonPressed = true;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());

    // Check if it stays in Disable state even if the button is not released (ButtonReleaseFlag test)
    motorController->update();
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
}

// Check if change to Disable state from Recuperate state if inverter DC turned off
TEST_F(MotorControllerTest, Recuperate_Disable_State_Test_3) {
    sensorData->SDC_Pressed = false;
    for (int i = 0; i < 4; ++i) {
        asArray(&sensorData->inverter.inverterDCon_FL)[i] = true;
        asArray(&sensorData->inverter.inverterOn_FL)[i] = true;
        asArray(&sensorData->inverter.inverterSystemReady_FL)[i] = true;
    }
    motorController->setState(Recuperate::getInstance());
    motorController->update();
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Recuperate::getInstance());
    // turn off inverter DC rear right
    asArray(&sensorData->inverter.inverterDCon_FL)[3] = false;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
}
// Check if change to Disable state from Recuperate state if any inverter turned off
TEST_F(MotorControllerTest, Recuperate_Disable_State_Test_4) {
    sensorData->SDC_Pressed = false;
    for (int i = 0; i < 4; ++i) {
        asArray(&sensorData->inverter.inverterDCon_FL)[i] = true;
        asArray(&sensorData->inverter.inverterOn_FL)[i] = true;
        asArray(&sensorData->inverter.inverterSystemReady_FL)[i] = true;
    }
    motorController->setState(Recuperate::getInstance());
    std::this_thread::sleep_for(std::chrono::seconds(5));
    motorController->update();
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Recuperate::getInstance());
    motorController->update();
    motorController->update();
    // turn off inverter front right
    asArray(&sensorData->inverter.inverterOn_FL)[1] = false;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
}
// Check if change to Disable state from Recuperate state if inverter system not ready
TEST_F(MotorControllerTest, Recuperate_Disable_State_Test_5) {
    sensorData->SDC_Pressed = false;
    for (int i = 0; i < 4; ++i) {
        asArray(&sensorData->inverter.inverterDCon_FL)[i] = true;
        asArray(&sensorData->inverter.inverterOn_FL)[i] = true;
        asArray(&sensorData->inverter.inverterSystemReady_FL)[i] = true;
    }
    motorController->setState(Recuperate::getInstance());
    motorController->update();
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Recuperate::getInstance());
    // turn off inverter system ready front left
    asArray(&sensorData->inverter.inverterSystemReady_FL)[0] = false;
    motorController->update();
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
}
// Check if any Inverter is off for more than 5 seconds during startup Recuperate state
TEST_F(MotorControllerTest, Recuperate_Disable_State_Test_6) {
    sensorData->SDC_Pressed = false;
    for (int i = 0; i < 4; ++i) {
        asArray(&sensorData->inverter.inverterDCon_FL)[i] = true;
        asArray(&sensorData->inverter.inverterSystemReady_FL)[i] = true;
    }
    motorController->setState(Recuperate::getInstance());
    for (int i = 0; i < 500; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds (10));
        motorController->update();
    }
    EXPECT_EQ(motorController->getCurrentState(), &Disable::getInstance());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}