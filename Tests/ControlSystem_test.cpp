#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <thread>
#include <chrono>
#include <iostream> 

#include "System.hpp"
#include "Timer.hpp"
#include "LogManager.hpp"
#include "system/arrayCompareLogic.hpp"






class System_Integration_Test : public ::testing::Test {
protected:
    void SetUp() override {
        data   =  new Data();
        LogManager::init();
        system = new System(data);
        
        sensorData = data->getSensorData();
        canDataOut = &system->getSystemData()->CANOutput;
        sensorData->SDC_Pressed = false;
    }

    void TearDown() override {
        LogManager::shutdown();
        delete system;
        delete data;
    }

    void waitFor(int ms) {
        Timer timer;
        std::cout << "Start to wait for " << ms << "ms" << std::endl;
        timer.start();
        for (int i = 0; i < ms; ++i) {
            auto wakeUpTime = std::chrono::system_clock::now()+std::chrono::milliseconds (1);
            //std::cout << "past time: " << timer.pastTime() << std::endl;
            system->update();
            std::this_thread::sleep_until(wakeUpTime);
        }
        std::cout << "End of waiting. " << "Past Time: " << timer.pastTime() <<  std::endl;
        timer.stop();
    }

    void startupSystem(){
        // Motor Disable
        // Inverter answer over the CAN Bus
        for (int i = 0; i < 4; ++i) {
            asArray(&sensorData->inverter.inverterSystemReady_FL)[i] = true; // Answer for the InverterOn = true command on the CAN out.
        }
        sensorData->cockpit.TSButton = true;
        waitFor(70);
        // Motor Precharge

        // Accu and inverter answer over the CAN Bus
        sensorData->accu.prechargeRelay = 370; // Answer for the prechargeRelay = true command on the CAN out.
        sensorData->accu.AIRNegative = 370; // Answer for the AIRNegative = true command on the CAN out.
        for (int i = 0; i < 4; ++i) {
            asArray(&sensorData->inverter.inverterDCon_FL)[i] = true; // Answer for the InverterOn = true command on the CAN out.
            asArray(&sensorData->inverter.invertersVoltage_FL)[i] = 600;
        }
        

        // Releas TS Button
        sensorData->cockpit.TSButton = false;

        waitFor(20);
        // Motor HVCoupling

        // Accu answer over the CAN Bus
        sensorData->accu.AIRPositive = 370; //370=true Answer for the AIRPositive = true command on the CAN out.
        sensorData->accu.AIRNegative = 370; //370=true Answer for the AIRNegative = true command on the CAN out.
        sensorData->accu.prechargeRelay = 0; //0=false Answer for the prechargeRelay = false command on the CAN out.
        sensorData->accu.HVvoltage = 600;

        waitFor(20);

        // Pressing the Break
        sensorData->pedal.breakPressureFront = 200;
        sensorData->pedal.breakPressureRear = 200;

        waitFor(80);
        sensorData->cockpit.RTDButton = true;

        waitFor(70);
        // Motor Ready to drive

        // Inverter answer over the CAN Bus
        for (int i = 0; i < 4; ++i) {
            asArray(&sensorData->inverter.inverterOn_FL)[i] = true; // Answer for the InverterEnable = true command on the CAN out.
        }
        waitFor(100);
        sensorData->cockpit.RTDButton = false;
        waitFor(20);
        // Motor Drive
        // Release break
        sensorData->pedal.breakPressureFront = 150;
        sensorData->pedal.breakPressureRear = 150;

        if (system->getSystemData()->motorControllerBox.currentState != DriveState) {
            std::cout << "Motor Controller: Startup was not successful." << std::endl;
            FAIL();
        }else{
            std::cout << "Motor Controller: Startup was successful." << std::endl;
            SUCCEED();
        }

    }

   Data* data;
    System* system;
    sensor_data_t* sensorData;
    CAN_data_out_t* canDataOut;
};

// Test if the system is correctly started up
TEST_F(System_Integration_Test, System_startup_Test) {
    // Motor Disable
    ASSERT_EQ(system->getSystemData()->motorControllerBox.currentState, DisableState);
    // Inverter answer over the CAN Bus
    for (int i = 0; i < 4; ++i) {
        asArray(&sensorData->inverter.inverterSystemReady_FL)[i] = true; // Answer for the InverterOn = true command on the CAN out.
    }
    waitFor(100);
    EXPECT_TRUE(canDataOut->out_TSLed);
    std::cout << "TS Led ON" << std::endl;
    sensorData->cockpit.TSButton = true;
    std::cout << "TS Button Pressed" << std::endl;
    waitFor(60);
    // Motor Precharge
    ASSERT_EQ(system->getSystemData()->motorControllerBox.currentState, PrechargeState);

    // Accu and inverter answer over the CAN Bus
    sensorData->accu.prechargeRelay = 370; //370=true Answer for the prechargeRelay = true command on the CAN out.
    sensorData->accu.AIRNegative = 370; //370=true Answer for the AIRNegative = true command on the CAN out.
    for (int i = 0; i < 4; ++i) {
        asArray(&sensorData->inverter.inverterDCon_FL)[i] = true; // Answer for the InverterOn = true command on the CAN out.
        asArray(&sensorData->inverter.invertersVoltage_FL)[i] = 600;
    }

    // Releas TS Button
    sensorData->cockpit.TSButton = false;

    waitFor(100);
    // Motor HVCoupling
    ASSERT_EQ(system->getSystemData()->motorControllerBox.currentState, HVCouplingState);

    // Accu answer over the CAN Bus
    sensorData->accu.AIRPositive = 370; //370=true Answer for the AIRPositive = true command on the CAN out.
    sensorData->accu.AIRNegative = 370; //370=true Answer for the AIRNegative = true command on the CAN out.
    sensorData->accu.prechargeRelay = 0; //0=false Answer for the prechargeRelay = false command on the CAN out.
    sensorData->accu.HVvoltage = 600;

    waitFor(1000);

    // Pressing the Break
    sensorData->pedal.breakPressureFront = 200;
    sensorData->pedal.breakPressureRear = 200;
    system->getSystemData()->pedalBox.throttle=0;

    waitFor(80);
    EXPECT_TRUE(canDataOut->out_RTDLed);
    std::cout << "RTD Led ON" << std::endl;
    sensorData->cockpit.RTDButton = true;
    std::cout << "RTD Button Pressed" << std::endl;
    waitFor(60);
    // Motor Ready to drive
    ASSERT_EQ(system->getSystemData()->motorControllerBox.currentState, ReadyToDriveState);

    // Inverter answer over the CAN Bus
    for (int i = 0; i < 4; ++i) {
        asArray(&sensorData->inverter.inverterOn_FL)[i] = true; // Answer for the InverterEnable = true command on the CAN out.
    }
    waitFor(100);
    sensorData->cockpit.RTDButton = false;
    waitFor(10);
    // Motor Drive
    ASSERT_EQ(system->getSystemData()->motorControllerBox.currentState, DriveState);
    waitFor(1000);
    EXPECT_TRUE(canDataOut->out_RTDsoundOn);
    waitFor(1100);
    EXPECT_FALSE(canDataOut->out_RTDsoundOn);

}

// Test if the throttle is working correctly
TEST_F(System_Integration_Test, Throttle_Test) {
    startupSystem();

    // Set throttle to 20 %
    std::cout << "Set throttle to 20 %" << std::endl;
    sensorData->pedal.throttleLeftMax = 500;
    sensorData->pedal.throttleLeftMin = 400;
    sensorData->pedal.throttleLeftValue = 420;

    sensorData->pedal.throttleRightMax = 200;
    sensorData->pedal.throttleRightMin = 100;
    sensorData->pedal.throttleRightValue = 120;

    waitFor(20);
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(asArray(&canDataOut->out_targetTorque_FL)[i], 200);
    }
    std::cout << "targetTorque = " << canDataOut->out_targetTorque_FL << std::endl;

    // Set throttle to 75 %
    std::cout << "Set throttle to 75 %" << std::endl;
    sensorData->pedal.throttleLeftMax = 500;
    sensorData->pedal.throttleLeftMin = 400;
    sensorData->pedal.throttleLeftValue = 475;

    sensorData->pedal.throttleRightMax = 200;
    sensorData->pedal.throttleRightMin = 100;
    sensorData->pedal.throttleRightValue = 175;

    waitFor(20);
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(asArray(&canDataOut->out_targetTorque_FL)[i], 750);
    }
    std::cout << "targetTorque = " << canDataOut->out_targetTorque_FL << std::endl;

    // Set throttle to 100 %
    std::cout << "Set throttle to 100 %" << std::endl;
    sensorData->pedal.throttleLeftMax = 500;
    sensorData->pedal.throttleLeftMin = 400;
    sensorData->pedal.throttleLeftValue = 500;

    sensorData->pedal.throttleRightMax = 200;
    sensorData->pedal.throttleRightMin = 100;
    sensorData->pedal.throttleRightValue = 200;

    waitFor(20);
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(asArray(&canDataOut->out_targetTorque_FL)[i], 1000);
    }
    std::cout << "targetTorque = " << canDataOut->out_targetTorque_FL << std::endl;

    // Set throttle to 50 %
    std::cout << "Set throttle to 50 %" << std::endl;
    sensorData->pedal.throttleLeftMax = 500;
    sensorData->pedal.throttleLeftMin = 400;
    sensorData->pedal.throttleLeftValue = 450;

    sensorData->pedal.throttleRightMax = 200;
    sensorData->pedal.throttleRightMin = 100;
    sensorData->pedal.throttleRightValue = 150;

    waitFor(20);
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(asArray(&canDataOut->out_targetTorque_FL)[i], 500);
    }
    std::cout << "targetTorque = " << canDataOut->out_targetTorque_FL << std::endl;

    // Set throttle to 0 %
    std::cout << "Set throttle to 0 %" << std::endl;
    sensorData->pedal.throttleLeftMax = 500;
    sensorData->pedal.throttleLeftMin = 400;
    sensorData->pedal.throttleLeftValue = 400;

    sensorData->pedal.throttleRightMax = 200;
    sensorData->pedal.throttleRightMin = 100;
    sensorData->pedal.throttleRightValue = 100;

    waitFor(20);
for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(asArray(&canDataOut->out_targetTorque_FL)[i], 0);
    }
    std::cout << "targetTorque = " << canDataOut->out_targetTorque_FL << std::endl;

}

// Test APPSError
TEST_F(System_Integration_Test, APPSError_Test) {
    startupSystem();

    // Set throttle to 20 %
    std::cout << "Set throttle to 20 %" << std::endl;
    sensorData->pedal.throttleLeftMax = 500;
    sensorData->pedal.throttleLeftMin = 400;
    sensorData->pedal.throttleLeftValue = 420;

    sensorData->pedal.throttleRightMax = 200;
    sensorData->pedal.throttleRightMin = 100;
    sensorData->pedal.throttleRightValue = 120;

    waitFor(20);

    // Make shortage between value Lines
    std::cout << "Make shortage between value Lines" << std::endl;
    sensorData->pedal.throttleLeftMax = 500;
    sensorData->pedal.throttleLeftMin = 400;
    sensorData->pedal.throttleLeftValue = 420;

    sensorData->pedal.throttleRightMax = 200;
    sensorData->pedal.throttleRightMin = 100;
    sensorData->pedal.throttleRightValue = 420;

    waitFor(120);
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(asArray(&canDataOut->out_targetTorque_FL)[i], 0);
    }
    EXPECT_EQ(system->getSystemData()->motorControllerBox.currentState, ErrorState);

}

// Test breakError
TEST_F(System_Integration_Test, BreakError_Test) {
    startupSystem();

    // Set throttle to 20 %
    std::cout << "Set throttle to 20 %" << std::endl;
    sensorData->pedal.throttleLeftMax = 500;
    sensorData->pedal.throttleLeftMin = 400;
    sensorData->pedal.throttleLeftValue = 420;

    sensorData->pedal.throttleRightMax = 200;
    sensorData->pedal.throttleRightMin = 100;
    sensorData->pedal.throttleRightValue = 120;

    waitFor(20);

    // press break
    std::cout << "Press break" << std::endl;
    sensorData->pedal.breakPressureFront = 200;
    sensorData->pedal.breakPressureRear = 200;
    waitFor(20);
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(asArray(&canDataOut->out_targetTorque_FL)[i], 0);
    }
    EXPECT_EQ(system->getSystemData()->motorControllerBox.currentState, DriveState);

    // Break Error
    std::cout << "Break Error" << std::endl;
    sensorData->pedal.breakPressureFront = 100;
    sensorData->pedal.breakPressureRear = 50;

    waitFor(20);

    EXPECT_EQ(system->getSystemData()->motorControllerBox.currentState, ErrorState);

}

// Test Error reset
TEST_F(System_Integration_Test, ErrorReset_Test) {
    startupSystem();

    // Set throttle to 20 %
    std::cout << "Set throttle to 20 %" << std::endl;
    sensorData->pedal.throttleLeftMax = 500;
    sensorData->pedal.throttleLeftMin = 400;
    sensorData->pedal.throttleLeftValue = 420;

    sensorData->pedal.throttleRightMax = 200;
    sensorData->pedal.throttleRightMin = 100;
    sensorData->pedal.throttleRightValue = 120;

    waitFor(20);

    // Make shortage between value Lines
    std::cout << "Make shortage between value Lines" << std::endl;
    sensorData->pedal.throttleLeftMax = 500;
    sensorData->pedal.throttleLeftMin = 400;
    sensorData->pedal.throttleLeftValue = 420;

    sensorData->pedal.throttleRightMax = 200;
    sensorData->pedal.throttleRightMin = 100;
    sensorData->pedal.throttleRightValue = 420;

    waitFor(120);
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(asArray(&canDataOut->out_targetTorque_FL)[i], 0);
    }
    EXPECT_EQ(system->getSystemData()->motorControllerBox.currentState, ErrorState);

    // Error reset with SDCok
    std::cout << "Error reset with SDC" << std::endl;
    sensorData->SDC_Pressed = true;
    waitFor(20);
    sensorData->SDC_Pressed = false;
    EXPECT_EQ(system->getSystemData()->motorControllerBox.currentState, DisableState);

    //set throttle to 0 % and restart the system
    sensorData->pedal.throttleRightValue = 100;
    sensorData->pedal.throttleLeftValue = 400;

    for (int i = 0; i < 4; ++i) {
        asArray(&sensorData->inverter.inverterOn_FL)[i] = false;
        asArray(&sensorData->inverter.inverterDCon_FL)[i] = false;
        asArray(&sensorData->inverter.inverterSystemReady_FL)[i] = false;
    }
    sensorData->accu.AIRNegative = 0;//false
    sensorData->accu.AIRPositive = 0;//false
    sensorData->accu.prechargeRelay = 0;//false
    waitFor(20);
    startupSystem();

    // Make shortage between value Lines
    std::cout << "Make shortage between value Lines" << std::endl;
    sensorData->pedal.throttleRightValue = 400;
    sensorData->pedal.throttleLeftValue = 400;

    waitFor(120);
    EXPECT_EQ(system->getSystemData()->motorControllerBox.currentState, ErrorState);
    std::cout << "Error reset with TS Button" << std::endl;
    sensorData->cockpit.TSButton = true;
    waitFor(4100);
    sensorData->cockpit.TSButton = false;
    EXPECT_EQ(system->getSystemData()->motorControllerBox.currentState, DisableState);

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}