#include "gtest/gtest.h"
#include <thread>
#include "system/PedalBoxStates.hpp"
#include "system/PedalBox.hpp"
#include "data/database.hpp"
#include "Timer.hpp"
#include "LogManager.hpp"

class PedalBoxTest : public ::testing::Test {
protected:
    void SetUp() override {
        LogManager::init();
        pedalBox = new PedalBox(&sensorData, &systemData, &settings);
    }

    void TearDown() override {
        delete pedalBox;
        LogManager::shutdown();
    }
    settings_t settings;
    sensor_data_t sensorData;
    system_data_t systemData;
    PedalBox* pedalBox;
};

// PedalOff PedalOn state test ############################################################################################################
// Test if PedalOff state changes to PedalOn state when SDCok is true
TEST_F(PedalBoxTest, PedalOff_PedalOn_State_Test) {
    // check if PedalOff state is set as init state
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOff::getInstance()) << "PedalOff state is not set as init state";

    // Check if PedalOff state stays in PedalOff state when SDCok is false
    sensorData.SDC_Pressed = true;
    pedalBox->update();
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOff::getInstance()) << "PedalOff state changes to PedalOn state when SDCok is false";

    // Check if PedalOff state changes to PedalOn state when SDCok is true
    sensorData.SDC_Pressed = false;
    std::cout << "SDC_Pressed = false" << std::endl;
    pedalBox->update();
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOn::getInstance()) << "PedalOff state does not change to PedalOn state when SDCok is true";

    // Check if PedalOn state changes to PedalOff state when SDCok is false
    sensorData.SDC_Pressed = true;
    std::cout << "SDC_Pressed = true" << std::endl;
    pedalBox->update();
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOff::getInstance()) << "PedalOn state does not change to PedalOff state when SDCok is false";
}

// PedalOn PedalFailure state test ############################################################################################################
// Check if PedalOn state changes to PedalFailure state when throttle value is wrong
TEST_F(PedalBoxTest, PedalOn_PedalFailure_State_Test) {
    Timer failureStateTimer;
    // Check if PedalOn state changes to PedalFailure state when throttleLeftValue and throttleRightValue are the same (shortage between Lines).
    sensorData.SDC_Pressed = false;
    std::cout << "SDC_Pressed = false" << std::endl;
    pedalBox->update();

    sensorData.pedal.throttleLeftValue = 200;
    sensorData.pedal.throttleRightValue = 200;
    std::cout << "throttleLeftValue = 200,\nthrottleRightValue = 200" << std::endl;
    failureStateTimer.start();
    pedalBox->update();
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalFailure::getInstance()) << "PedalOn state does not change to PedalFailure state when throttle value is wrong";
    std::cout<< "wait for 90ms" << std::endl;
    while (failureStateTimer.pastTime() < 89) {
        pedalBox->update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    // Change throttleLeftValue and throttleRightValue to initial values
    sensorData.pedal.throttleLeftValue = 410;
    sensorData.pedal.throttleRightValue = 110;
    std::cout << "Change throttle values back to initial values:" << std::endl;
    std::cout << "throttleLeftValue = 100,\nthrottleRightValue = 400" << std::endl;
    pedalBox->update();
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOn::getInstance()) << "The PedalFailure state does not transition to the PedalOn state\n if the throttle value becomes correct again before staying in the PedalFailure state for more than 100 ms. \n" << "Time in PedalFailure State:" << failureStateTimer.pastTime() << std::endl;
}

// PedalFailure PedalOff state test ############################################################################################################
// Check if PedalFailure state changes to PedalOff state when SDCok is false
TEST_F(PedalBoxTest, PedalFailure_PedalOff_State_Test) {
    // Check if PedalFailure state changes to PedalOff state when SDCok is false
    sensorData.SDC_Pressed = false;
    pedalBox->update();
    sensorData.pedal.throttleLeftValue = 200;
    sensorData.pedal.throttleRightValue = 200;
    pedalBox->update();
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalFailure::getInstance()) << "PedalOn state does not change to PedalFailure state when throttle value is wrong";

    sensorData.SDC_Pressed = true;
    std::cout << "SDC_Pressed = true" << std::endl;
    pedalBox->update();
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOff::getInstance()) << "PedalFailure state does not change to PedalOff state when SDCok is false";
}

// PedalFailure PedalError state test ############################################################################################################
// Check if PedalFailure state changes to PedalError state when throttle value is wrong for more than 100ms
TEST_F(PedalBoxTest, PedalFailure_PedalError_State_Test) {
    Timer failureStateTimer;
    // Check if PedalFailure state changes to PedalError state when throttleLeftValue and throttleRightValue are the same (shortage between Lines).
    sensorData.SDC_Pressed = false;
    pedalBox->update();
    sensorData.pedal.throttleLeftValue = 200;
    sensorData.pedal.throttleRightValue = 200;
    pedalBox->update();
    failureStateTimer.start();
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalFailure::getInstance()) << "PedalOn state does not change to PedalFailure state when throttle value is wrong";

    std::cout<< "wait for 120ms" << std::endl;
    while (failureStateTimer.pastTime() < 120) {
        pedalBox->update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalError::getInstance()) << "The PedalFailure state does not transition to the PedalError state\nif the throttle value stays incorrect for more than 100ms. \n" << "Time in PedalFailure State:" << failureStateTimer.pastTime() << std::endl;
}

// PedalError PedalOff state test ############################################################################################################
// Check if PedalError state changes to PedalOff state when SDCok is false
TEST_F(PedalBoxTest, PedalError_PedalOff_State_Test) {
    Timer failureStateTimer;
    // Check if PedalError state changes to PedalOff state when SDCok is false
    sensorData.SDC_Pressed = false;
    pedalBox->update();
    sensorData.pedal.throttleLeftValue = 200;
    sensorData.pedal.throttleRightValue = 200;
    pedalBox->update();
    failureStateTimer.start();
    while (failureStateTimer.pastTime() < 110) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        pedalBox->update();
    }
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalError::getInstance()) << "PedalFailure state does not change to PedalError state when throttle value is wrong for more than 100ms";

    sensorData.SDC_Pressed = true;
    std::cout << "SDC_Pressed = true" << std::endl;
    pedalBox->update();
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOff::getInstance()) << "PedalError state does not change to PedalOff state when SDCok is false";
}

// Throttle calculation test ############################################################################################################
// Check if throttle value is calculated correctly
TEST_F(PedalBoxTest, Throttle_Calculation_Test) {
    // Change to PedalOn state
    sensorData.SDC_Pressed = false;
    pedalBox->update();

    //Set Throttle values to 0%
    std::cout << "Set Throttle values to 0% (Throttle pedal not Pressed)" << std::endl;
    sensorData.pedal.throttleLeftValue = 484;
    sensorData.pedal.throttleRightValue = 184;
    pedalBox->update();
    std::cout << "Throttle value: " << pedalBox->getPedalBoxDate()->throttle <<"%"<< std::endl;
    ASSERT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value is not calculated correctly";

    //Set Throttle values to 50%
    std::cout << "Set Throttle values to 50% (Throttle pedal half Pressed)" << std::endl;
    sensorData.pedal.throttleLeftValue = 447;
    sensorData.pedal.throttleRightValue = 147;
    pedalBox->update();
    std::cout << "Throttle value: " <<(int) pedalBox->getPedalBoxDate()->throttle <<"%"<< std::endl;
    ASSERT_EQ(pedalBox->getPedalBoxDate()->throttle, 50) << "Throttle value is not calculated correctly";

    //Set Throttle values to 100%
    std::cout << "Set Throttle values to 100% (Throttle pedal fully Pressed)" << std::endl;
    sensorData.pedal.throttleLeftValue = 410;
    sensorData.pedal.throttleRightValue = 110;
    pedalBox->update();
    std::cout << "Throttle value: " <<(int) pedalBox->getPedalBoxDate()->throttle <<"%"<< std::endl;
    ASSERT_EQ(pedalBox->getPedalBoxDate()->throttle, 100) << "Throttle value is not calculated correctly";
}

// Throttle calculation failure test ############################################################################################################
// Check if the throttle calculation detects potentiometer out of range failures
TEST_F(PedalBoxTest, Throttle_Calculation_Failure_Test) {
    // Change to PedalOn state
    sensorData.SDC_Pressed = false;
    pedalBox->update();

    //Set Throttle values to 0%
    std::cout << "Set Throttle values to 0% (Throttle pedal not Pressed)" << std::endl;
  sensorData.pedal.throttleLeftValue = 484;
    sensorData.pedal.throttleRightValue = 184;
    pedalBox->update();
    std::cout << "Throttle value: " << pedalBox->getPedalBoxDate()->throttle <<"%"<< std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value is not calculated correctly";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOn::getInstance()) << "calculateThrottle function detected failures when there are none";

    //Set Throttle right value to 101%
    std::cout << "Set Throttle right value to 101% (Throttle pedal fully Pressed)" << std::endl;
    sensorData.pedal.throttleLeftValue = 409;
    sensorData.pedal.throttleRightValue = 109;
    pedalBox->update();
    std::cout << "Throttle value: " << (int)pedalBox->getPedalBoxDate()->throttle <<"%"<< std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value is not calculated correctly";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalFailure::getInstance()) << "calculateThrottle function does not detect potentiometer out of range failures";

    // Set Throttle right value to 100% (PedalOn state again)
    std::cout << "Set Throttle right value to 100% (Throttle pedal fully Pressed)" << std::endl;
    sensorData.pedal.throttleLeftValue = 410;
    sensorData.pedal.throttleRightValue = 110;
    pedalBox->update();
    pedalBox->update();
    std::cout << "Throttle value: " << (int)pedalBox->getPedalBoxDate()->throttle <<"%"<< std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 100) << "Throttle value is not calculated correctly";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOn::getInstance()) << "calculateThrottle function detected failures when there are none";


    //Set Throttle left value to -1%
    std::cout << "Set Throttle left value to -1% (Throttle pedal not Pressed)" << std::endl;
    sensorData.pedal.throttleLeftValue = 485;
    sensorData.pedal.throttleRightValue = 185;
    pedalBox->update();
    std::cout << "Throttle value: " << (int)pedalBox->getPedalBoxDate()->throttle <<"%"<< std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 100) << "Throttle value is not calculated correctly";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalFailure::getInstance()) << "calculateThrottle function does not detect potentiometer out of range failures";
}

// Throttle shortage test ############################################################################################################
// Check if the throttle calculation detects shortage between the value lines
TEST_F(PedalBoxTest, Throttle_Value_Line_Shortage_Test) {
    // Change to PedalOn state
    sensorData.SDC_Pressed = false;
    pedalBox->update();

    //Set Throttle values to 0%
    std::cout << "Set Throttle values to 0% (Throttle pedal not Pressed)" << std::endl;
  sensorData.pedal.throttleLeftValue = 484;
    sensorData.pedal.throttleRightValue = 184;
    pedalBox->update();
    std::cout << "Throttle value: " << pedalBox->getPedalBoxDate()->throttle << "%" << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value is not calculated correctly";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOn::getInstance())
                                << "calculateThrottle function detected failures when there are none";

    // Make shortage between the value lines
    std::cout << "Make shortage between the value lines." << std::endl;
    sensorData.pedal.throttleLeftValue = 200;
    sensorData.pedal.throttleRightValue = 200;
    pedalBox->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    pedalBox->update();
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value should stay the same when there is a shortage between the value lines";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalError::getInstance()) << "checkForShortage function does not detect shortage between the value lines";
}

// Check if the throttle calculation detects shortage between the min lines
TEST_F(PedalBoxTest, Throttle_Min_Line_Shortage_Test) {
    // Change to PedalOn state
    sensorData.SDC_Pressed = false;
    pedalBox->update();

    //Set Throttle values to 0%
    std::cout << "Set Throttle values to 0% (Throttle pedal not Pressed)" << std::endl;
  sensorData.pedal.throttleLeftValue = 484;
    sensorData.pedal.throttleRightValue = 184;
    pedalBox->update();
    std::cout << "Throttle value: " << pedalBox->getPedalBoxDate()->throttle << "%" << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value is not calculated correctly";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOn::getInstance())
                                << "calculateThrottle function detected failures when there are none";

    // Make shortage between the min lines
    std::cout << "Make shortage between the min lines." << std::endl;
    sensorData.pedal.throttleLeftMin = 400;
    sensorData.pedal.throttleRightMin = 400;
    pedalBox->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    pedalBox->update();
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value should stay the same when there is a shortage between the min lines";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalError::getInstance()) << "checkForShortage function does not detect shortage between the min lines";
}
// Check if the throttle calculation detects shortage between the max lines
TEST_F(PedalBoxTest, Throttle_Max_Line_Shortage_Test) {
    // Change to PedalOn state
    sensorData.SDC_Pressed = false;
    pedalBox->update();

    //Set Throttle values to 0%
    std::cout << "Set Throttle values to 0% (Throttle pedal not Pressed)" << std::endl;
  sensorData.pedal.throttleLeftValue = 484;
    sensorData.pedal.throttleRightValue = 184;
    pedalBox->update();
    std::cout << "Throttle value: " << pedalBox->getPedalBoxDate()->throttle << "%" << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value is not calculated correctly";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOn::getInstance())
                                << "calculateThrottle function detected failures when there are none";

    // Make shortage between the max lines
    std::cout << "Make shortage between the max lines." << std::endl;
    sensorData.pedal.throttleLeftMax = 500;
    sensorData.pedal.throttleRightMax = 500;
    pedalBox->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    pedalBox->update();
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value should stay the same when there is a shortage between the max lines";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalError::getInstance()) << "checkForShortage function does not detect shortage between the max lines";
}

// Check if the throttle calculation detects shortage between right value and Vdc
TEST_F(PedalBoxTest, Throttle_RightValue_Vdc_Shortage_Test) {
    // Change to PedalOn state
    sensorData.SDC_Pressed = false;
    pedalBox->update();

    //Set Throttle values to 0%
    std::cout << "Set Throttle values to 0% (Throttle pedal not Pressed)" << std::endl;
  sensorData.pedal.throttleLeftValue = 484;
    sensorData.pedal.throttleRightValue = 184;
    pedalBox->update();
    std::cout << "Throttle value: " << pedalBox->getPedalBoxDate()->throttle << "%" << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value is not calculated correctly";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOn::getInstance())
                                << "calculateThrottle function detected failures when there are none";

    // Make shortage between the value and Vdc
    std::cout << "Make shortage between the right value and Vdc." << std::endl;
    sensorData.pedal.throttleLeftValue = 400;
    sensorData.pedal.throttleRightValue = 1024;
    pedalBox->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    pedalBox->update();
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value should stay the same when there is a shortage between the value and Vdc";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalError::getInstance()) << "checkForShortage function does not detect shortage between the value and Vdc";
}

// Check if the throttle calculation detects shortage between left value and Vdc
TEST_F(PedalBoxTest, Throttle_LeftValue_Vdc_Shortage_Test) {
    // Change to PedalOn state
    sensorData.SDC_Pressed = false;
    pedalBox->update();

    //Set Throttle values to 0%
    std::cout << "Set Throttle values to 0% (Throttle pedal not Pressed)" << std::endl;
  sensorData.pedal.throttleLeftValue = 484;
    sensorData.pedal.throttleRightValue = 184;
    pedalBox->update();
    std::cout << "Throttle value: " << pedalBox->getPedalBoxDate()->throttle << "%" << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value is not calculated correctly";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOn::getInstance())
                                << "calculateThrottle function detected failures when there are none";

    // Make shortage between the value and Vdc
    std::cout << "Make shortage between the left value and Vdc." << std::endl;
    sensorData.pedal.throttleLeftValue = 1024;
    sensorData.pedal.throttleRightValue = 100;
    pedalBox->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    pedalBox->update();
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value should stay the same when there is a shortage between the value and Vdc";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalError::getInstance()) << "checkForShortage function does not detect shortage between the value and Vdc";
}

// Check if the throttle calculation detects shortage between left min and Vdc
TEST_F(PedalBoxTest, Throttle_LeftMin_Vdc_Shortage_Test) {
    // Change to PedalOn state
    sensorData.SDC_Pressed = false;
    pedalBox->update();

    //Set Throttle values to 0%
    std::cout << "Set Throttle values to 0% (Throttle pedal not Pressed)" << std::endl;
  sensorData.pedal.throttleLeftValue = 484;
    sensorData.pedal.throttleRightValue = 184;
    pedalBox->update();
    std::cout << "Throttle value: " << pedalBox->getPedalBoxDate()->throttle << "%" << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value is not calculated correctly";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOn::getInstance())
                                << "calculateThrottle function detected failures when there are none";

    // Make shortage between the min and Vdc
    std::cout << "Make shortage between the left min and Vdc." << std::endl;
    sensorData.pedal.throttleLeftMin = 1024;
    sensorData.pedal.throttleRightMin = 100;
    pedalBox->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    pedalBox->update();
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value should stay the same when there is a shortage between the min and Vdc";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalError::getInstance()) << "checkForShortage function does not detect shortage between the min and Vdc";
}

// Check if the throttle calculation detects shortage between right max and Vdc
TEST_F(PedalBoxTest, Throttle_RightMax_Vdc_Shortage_Test) {
    // Change to PedalOn state
    sensorData.SDC_Pressed = false;
    pedalBox->update();

    //Set Throttle values to 0%
    std::cout << "Set Throttle values to 0% (Throttle pedal not Pressed)" << std::endl;
  sensorData.pedal.throttleLeftValue = 484;
    sensorData.pedal.throttleRightValue = 184;
    pedalBox->update();
    std::cout << "Throttle value: " << pedalBox->getPedalBoxDate()->throttle << "%" << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value is not calculated correctly";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOn::getInstance())
                                << "calculateThrottle function detected failures when there are none";

    // Make shortage between the max and Vdc
    std::cout << "Make shortage between the left max and Vdc." << std::endl;
    sensorData.pedal.throttleLeftMax = 1024;
    sensorData.pedal.throttleRightMax = 200;
    pedalBox->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    pedalBox->update();
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value should stay the same when there is a shortage between the max and Vdc";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalError::getInstance()) << "checkForShortage function does not detect shortage between the max and Vdc";
}

// Check if the throttle calculation detects shortage between right min and Ground
TEST_F(PedalBoxTest, Throttle_RightMin_Ground_Shortage_Test) {
    // Change to PedalOn state
    sensorData.SDC_Pressed = false;
    pedalBox->update();

    //Set Throttle values to 0%
    std::cout << "Set Throttle values to 0% (Throttle pedal not Pressed)" << std::endl;
  sensorData.pedal.throttleLeftValue = 484;
    sensorData.pedal.throttleRightValue = 184;
    pedalBox->update();
    std::cout << "Throttle value: " << pedalBox->getPedalBoxDate()->throttle << "%" << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value is not calculated correctly";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOn::getInstance())
                                << "calculateThrottle function detected failures when there are none";

    // Make shortage between the min and Ground
    std::cout << "Make shortage between the right min and Ground." << std::endl;
    sensorData.pedal.throttleLeftMin = 400;
    sensorData.pedal.throttleRightMin = 0;
    pedalBox->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    pedalBox->update();
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value should stay the same when there is a shortage between the min and Ground";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalError::getInstance()) << "checkForShortage function does not detect shortage between the min and Ground";
}

// Check if the throttle calculation detects shortage between right max and Ground
TEST_F(PedalBoxTest, Throttle_RightMax_Ground_Shortage_Test) {
    // Change to PedalOn state
    sensorData.SDC_Pressed = false;
    pedalBox->update();

    //Set Throttle values to 0%
    std::cout << "Set Throttle values to 0% (Throttle pedal not Pressed)" << std::endl;
  sensorData.pedal.throttleLeftValue = 484;
    sensorData.pedal.throttleRightValue = 184;
    pedalBox->update();
    std::cout << "Throttle value: " << pedalBox->getPedalBoxDate()->throttle << "%" << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value is not calculated correctly";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOn::getInstance())
                                << "calculateThrottle function detected failures when there are none";

    // Make shortage between the max and Ground
    std::cout << "Make shortage between the right max and Ground." << std::endl;
    sensorData.pedal.throttleLeftMax = 400;
    sensorData.pedal.throttleRightMax = 0;
    pedalBox->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    pedalBox->update();
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value should stay the same when there is a shortage between the max and Ground";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalError::getInstance()) << "checkForShortage function does not detect shortage between the max and Ground";
}

// Check if the throttle calculation detects shortage between left value and Ground
TEST_F(PedalBoxTest, Throttle_LeftValue_Ground_Shortage_Test) {
    // Change to PedalOn state
    sensorData.SDC_Pressed = false;
    pedalBox->update();

    //Set Throttle values to 0%
    std::cout << "Set Throttle values to 0% (Throttle pedal not Pressed)" << std::endl;
      sensorData.pedal.throttleLeftValue = 484;
    sensorData.pedal.throttleRightValue = 184;
    pedalBox->update();
    std::cout << "Throttle value: " << pedalBox->getPedalBoxDate()->throttle << "%" << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value is not calculated correctly";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOn::getInstance())
                                << "calculateThrottle function detected failures when there are none";

    // Make shortage between the value and Ground
    std::cout << "Make shortage between the left value and Ground." << std::endl;
    sensorData.pedal.throttleLeftValue = 0;
    sensorData.pedal.throttleRightValue = 100;
    pedalBox->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    pedalBox->update();
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value should stay the same when there is a shortage between the value and Ground";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalError::getInstance()) << "checkForShortage function does not detect shortage between the value and Ground";
}

// Check if the throttle calculation detects shortage between right value and Ground
TEST_F(PedalBoxTest, Throttle_RightValue_Ground_Shortage_Test) {
    // Change to PedalOn state
    sensorData.SDC_Pressed = false;
    pedalBox->update();

    //Set Throttle values to 0%
    std::cout << "Set Throttle values to 0% (Throttle pedal not Pressed)" << std::endl;
  sensorData.pedal.throttleLeftValue = 484;
    sensorData.pedal.throttleRightValue = 184;
    pedalBox->update();
    std::cout << "Throttle value: " << pedalBox->getPedalBoxDate()->throttle << "%" << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value is not calculated correctly";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOn::getInstance())
                                << "calculateThrottle function detected failures when there are none";

    // Make shortage between the value and Ground
    std::cout << "Make shortage between the right value and Ground." << std::endl;
    sensorData.pedal.throttleLeftValue = 400;
    sensorData.pedal.throttleRightValue = 0;
    pedalBox->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    pedalBox->update();
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value should stay the same when there is a shortage between the value and Ground";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalError::getInstance()) << "checkForShortage function does not detect shortage between the value and Ground";
}

// Throttle off test ############################################################################################################
// Check if throttle value is 0 when PedalOff state
TEST_F(PedalBoxTest, Throttle_Off_Test) {
    // Change to PedalOn state
    sensorData.SDC_Pressed = false;
    pedalBox->update();

    //Set Throttle values to 50%
    std::cout << "Set Throttle values to 50% (Throttle pedal half Pressed)" << std::endl;
    sensorData.pedal.throttleLeftValue = 447;
    sensorData.pedal.throttleRightValue = 147;
    pedalBox->update();
    std::cout << "Throttle value: " << pedalBox->getPedalBoxDate()->throttle << "%" << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 50) << "Throttle value is not calculated correctly";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOn::getInstance())
                                << "calculateThrottle function detected failures when there are none";

    // Change to PedalOff state
    sensorData.SDC_Pressed = true;
    std::cout << "SDC_Pressed = true" << std::endl;
    pedalBox->update();
    std::cout << "Throttle value: " << pedalBox->getPedalBoxDate()->throttle << "%" << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value is not 0 when PedalOff state";
}

// Check if throttle value is 0 when PedalError state
TEST_F(PedalBoxTest, Throttle_Error_Test) {
    // Change to PedalOn state
    sensorData.SDC_Pressed = false;
    pedalBox->update();

    //Set Throttle values to 50%
    std::cout << "Set Throttle values to 50% (Throttle pedal half Pressed)" << std::endl;
    sensorData.pedal.throttleLeftValue = 447;
    sensorData.pedal.throttleRightValue = 147;
    pedalBox->update();
    std::cout << "Throttle value: " << pedalBox->getPedalBoxDate()->throttle << "%" << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 50) << "Throttle value is not calculated correctly";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOn::getInstance())
                                << "calculateThrottle function detected failures when there are none";

    // Change to PedalError state
    sensorData.pedal.throttleLeftValue = 200;
    sensorData.pedal.throttleRightValue = 200;
    pedalBox->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(110));
    pedalBox->update();
    std::cout << "Throttle value: " << pedalBox->getPedalBoxDate()->throttle << "%" << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 0) << "Throttle value is not 0 when PedalError state";
}

// APPSError test ############################################################################################################
// Check if the APPSError is set when PedalError state
TEST_F(PedalBoxTest, APPSError_Test) {
    // Change to PedalOn state
    sensorData.SDC_Pressed = false;
    pedalBox->update();

    //Set Throttle values to 50%
    std::cout << "Set Throttle values to 50% (Throttle pedal half Pressed)" << std::endl;
    sensorData.pedal.throttleLeftValue = 447;
    sensorData.pedal.throttleRightValue = 147;
    pedalBox->update();
    std::cout << "Throttle value: " << pedalBox->getPedalBoxDate()->throttle << "%" << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 50) << "Throttle value is not calculated correctly";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOn::getInstance())
                                << "calculateThrottle function detected failures when there are none";

    // Change to PedalError state
    sensorData.pedal.throttleLeftValue = 200;
    sensorData.pedal.throttleRightValue = 200;
    pedalBox->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(110));
    pedalBox->update();
    std::cout << "APPSError: " << pedalBox->getPedalBoxDate()->APPSError << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->APPSError, true) << "APPSError is not set when PedalError state";
}

// Check if the APPSError is reset when PedalOff state
TEST_F(PedalBoxTest, APPSError_Reset_Test) {
    // Change to PedalOn state
    sensorData.SDC_Pressed = false;
    pedalBox->update();

    //Set Throttle values to 50%
    std::cout << "Set Throttle values to 50% (Throttle pedal half Pressed)" << std::endl;
    sensorData.pedal.throttleLeftValue = 447;
    sensorData.pedal.throttleRightValue = 147;
    pedalBox->update();
    std::cout << "Throttle value: " << pedalBox->getPedalBoxDate()->throttle << "%" << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->throttle, 50) << "Throttle value is not calculated correctly";
    ASSERT_EQ(pedalBox->getCurrentState(), &PedalOn::getInstance())
                                << "calculateThrottle function detected failures when there are none";

    // Change to PedalError state
    sensorData.pedal.throttleLeftValue = 200;
    sensorData.pedal.throttleRightValue = 200;
    pedalBox->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(110));
    pedalBox->update();

    // Change to PedalOff state
    sensorData.SDC_Pressed = true;
    std::cout << "SDC_Pressed = true" << std::endl;
    pedalBox->update();
    std::cout << "APPSError: " << pedalBox->getPedalBoxDate()->APPSError << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->APPSError, false) << "APPSError is not reset when PedalOff state";
}



// Break test ############################################################################################################

class PedalBoxBreakTest : public ::testing::Test {
protected:
    void SetUp() override {
        LogManager::init();
        pedalBox = new PedalBox(&sensorData, &systemData, &settings);
    }

    void TearDown() override {
        delete pedalBox;
        LogManager::shutdown();
    }
    settings_t settings;
    sensor_data_t sensorData;
    system_data_t systemData;
    PedalBox* pedalBox;

};

// Check the breakPressed Flag ############################################################################################################
// Check if the breakPressed flag is set when the break is pressed more than 10%
TEST_F(PedalBoxBreakTest, Break_Pressed_Test) {
    //Set Break values to 5%
    std::cout << "Set Break values to 5% (Break pedal slightly Pressed)" << std::endl;
    sensorData.pedal.breakPressureFront = 162; // 5% = (400 - 150) *0.05 + 150 = 162.5
    sensorData.pedal.breakPressureRear = 163; // 5% = (400 - 150) *0.05 +150= 162.5
    pedalBox->update();
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakPercentage, 5) << "Break percentage is not calculated correctly";
    std::cout << "Break Pressed: " << pedalBox->getPedalBoxDate()->breakPressed << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakPressed, false) << "Break Pressed flag is set when the break is pressed less than 10%";

    //Set Break values to 11%
    std::cout << "Set Break values to 11% (Break pedal Pressed)" << std::endl;
    sensorData.pedal.breakPressureFront = 178; // 11% = (400 - 150) *0.11 +150= 177.5
    sensorData.pedal.breakPressureRear = 177; // 11% = (400 - 150) *0.11 +150= 177.5
    pedalBox->update();
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakPercentage, 11) << "Break percentage is not calculated correctly";
    std::cout << "Break Pressed: " << pedalBox->getPedalBoxDate()->breakPressed << std::endl;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakPressed, true) << "Break Pressed flag is not set when the break is pressed more than 10%";
}

// Check the break Error Flag ############################################################################################################
// Check if the breakError flag is set when the break is pressed more than 100%
TEST_F(PedalBoxBreakTest, Break_outOfRange_Max_Error_Test) {
    //Set Break values to 100%
    std::cout << "Set Break values to 100% (Break pedal fully Pressed)" << std::endl;
    sensorData.pedal.breakPressureFront = 400; // 100% = 400
    sensorData.pedal.breakPressureRear = 400;
    pedalBox->update();
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakPercentage, 100) << "Break percentage is not calculated correctly";
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakError, false) << "Break Error flag is set when the break is pressure is in range";

    //Set front Break values to 101%
    std::cout << "Set Right Break values to 101% (Break pedal fully Pressed)" << std::endl;
    sensorData.pedal.breakPressureFront = 403; // 101% = (400-150)*1.01+150 = 403
    sensorData.pedal.breakPressureRear = 400;
    pedalBox->update();
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakPercentage, 0) << "Break dos not detect the error ";
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakError, true) << "Break Error flag is not set when the break is pressed more than 100%";

    //reset Error flag
    sensorData.SDC_Pressed = true;
    pedalBox->update();
    sensorData.SDC_Pressed = false;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakError, false) << "Break Error flag is not reset when the SDCok is set to true";

    //Set rear Break values to 101%
    std::cout << "Set Left Break values to 101% (Break pedal fully Pressed)" << std::endl;
    sensorData.pedal.breakPressureFront = 400;
    sensorData.pedal.breakPressureRear = 403; // 101% = (400-150)*1.01+150 = 403
    pedalBox->update();
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakPercentage, 0) << "Break dos not detect the error ";
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakError, true) << "Break Error flag is not set when the break is pressed more than 100%";
}

// Check if the breakError flag is set when the break is pressed less than 0%
TEST_F(PedalBoxBreakTest, Break_outOfRange_Min_Error_Test) {
    //Set Break values to 0%
    std::cout << "Set Break values to 0% (Break pedal not Pressed)" << std::endl;
    sensorData.pedal.breakPressureFront = 150; // 0% = 150
    sensorData.pedal.breakPressureRear = 150;
    pedalBox->update();
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakPercentage, 0) << "Break percentage is not calculated correctly";
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakError, false) << "Break Error flag is set when the break is pressure is in range";

    //Set front Break values to -1%
    std::cout << "Set Right Break values to -1% (Break pedal not Pressed)" << std::endl;
    sensorData.pedal.breakPressureFront = 148; // -1% = (400-150)*-0.01+150 = 148
    sensorData.pedal.breakPressureRear = 150;
    pedalBox->update();
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakPercentage, 0) << "Break dos not detect the error ";
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakError, true) << "Break Error flag is not set when the break is pressed less than 0%";

    //reset Error flag
    sensorData.SDC_Pressed = true;
    pedalBox->update();
    sensorData.SDC_Pressed = false;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakError, false) << "Break Error flag is not reset when the SDCok is set to true";

    //Set rear Break values to -1%
    std::cout << "Set Left Break values to -1% (Break pedal not Pressed)" << std::endl;
    sensorData.pedal.breakPressureFront = 150;
    sensorData.pedal.breakPressureRear = 148; // -1% = (400-150)*-0.01+150 = 148
    pedalBox->update();
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakPercentage, 0) << "Break does not detect the error";
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakError, true) << "Break Error flag is not set when the break is pressed less than 0%";
}

// Check if the error flag is set when the frond and the rear break pressures are not equal (more than 10% difference)
TEST_F(PedalBoxBreakTest, Break_Pressure_Difference_Error_Test) {
    //Set Break values to 5%
    std::cout << "Set break with a slight in limit difference." << std::endl;
    sensorData.pedal.breakPressureFront = 162; // 4.8% = (400 - 150) *0.048 + 150 = 162
    sensorData.pedal.breakPressureRear = 161; // 4.4% = (400 - 150) *0.05 +150= 161
    pedalBox->update();                        // 4.8*0.9 = 4.32  4.4*1.1 = 4.84
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakPercentage, 4.6) << "Break percentage is not calculated correctly";
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakError, false)
                        << "Break Error flag is set when the break is pressure is in range";


    std::cout << "Set break with a slight out of limit difference." << std::endl;
    sensorData.pedal.breakPressureFront = 162; // 4.8% = (400 - 150) *0.10 +150= 170
    sensorData.pedal.breakPressureRear = 160; // 4% = (400 - 150) *0.05 +150= 160
    pedalBox->update();                        // 4.8*0.9 = 4.32  4*1.1= 4.4
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakPercentage, 0) << "Break does not detect the error";
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakError, true)
                        << "Break Error flag is not set when the frond and the rear break pressures are not equal (more than 10% difference)";

    //reset Error flag
    sensorData.SDC_Pressed = true;
    pedalBox->update();
    sensorData.SDC_Pressed = false;
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakError, false)
                        << "Break Error flag is not reset when the SDCok is set to true";

    std::cout << "Set break with a slight in limit difference." << std::endl;
    sensorData.pedal.breakPressureRear = 162; // 4.8% = (400 - 150) *0.048 + 150 = 162
    sensorData.pedal.breakPressureFront = 161; // 4.4% = (400 - 150) *0.05 +150= 161
    pedalBox->update();                        // 4.8*0.9 = 4.32  4.4*1.1 = 4.84
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakPercentage, 4.6) << "Break percentage is not calculated correctly";
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakError, false)
                        << "Break Error flag is set when the break is pressure is in range";


    std::cout << "Set break with a slight out of limit difference." << std::endl;
    sensorData.pedal.breakPressureRear = 162; // 4.8% = (400 - 150) *0.10 +150= 170
    sensorData.pedal.breakPressureFront = 160; // 4% = (400 - 150) *0.05 +150= 160
    pedalBox->update();                        // 4.8*0.9 = 4.32  4*1.1= 4.4
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakPercentage, 0) << "Break does not detect the error";
    EXPECT_EQ(pedalBox->getPedalBoxDate()->breakError, true)
                        << "Break Error flag is not set when the frond and the rear break pressures are not equal (more than 10% difference)";

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}