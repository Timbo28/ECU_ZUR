#include "gtest/gtest.h"
#include "system/AccumulatorManagement.hpp"
#include "data/database.hpp"
#include "LogManager.hpp"

class AccumulatorManagementTest : public ::testing::Test {
protected:
    void SetUp() override {
         LogManager::init();
        sensorData.accu.HVvoltage = 815;
        sensorData.accu.HVcurrent = 100;
        sensorData.accu.accuStateOfCharge = 100;
        sensorData.accu.accuTemperature = 20;
        accuManagement = new AccumulatorManagement(&sensorData, &systemData, &settings);
        for (int i = 0; i < 62; i++) {
            accuManagement->update();
        }
    }

    void TearDown() override {
        delete accuManagement;
        LogManager::shutdown();
    }
    settings_t settings;
    sensor_data_t sensorData;
    system_data_t systemData;
    AccumulatorManagement* accuManagement;

};

// Test if the HVvoltage average is correctly set in the systemData############################################################################################################
// Test Averaging of voltage 63 values one is deleted
TEST_F(AccumulatorManagementTest, HVvoltage_Average_Test) {
    sensorData.accu.HVvoltage = 50;
    accuManagement->update();
    EXPECT_EQ(systemData.accuManagementBox.accuVoltage, 803);
}

// Test if average current is correctly set in the systemData############################################################################################################
// Test Averaging of 46 current values one is deleted
TEST_F(AccumulatorManagementTest, HVcurrent_Average_Test) {
    sensorData.accu.HVcurrent = 54;
    accuManagement->update(); // currentAverage = 99 > MaxDischargeCurrent = 98
    EXPECT_EQ(systemData.accuManagementBox.accuPowerLimit, true);

    accuManagement->update(); //is false (currentAverage = 98 > MaxDischargeCurrent = 98)
    EXPECT_EQ(systemData.accuManagementBox.accuPowerLimit, false);
}
// Test if the charge limits the maxDischargeCurrent############################################################################################################
// test critical charge limit
TEST_F(AccumulatorManagementTest, Critical_Charge_Limit_Test) {
    sensorData.accu.HVcurrent = 0;
    for (int i = 0; i < 45; ++i) {
       accuManagement->update();
    }
    sensorData.accu.accuStateOfCharge = 19;
    accuManagement->update(); //is false (currentAverage = 0 > MaxDischargeCurrent = 0)
    EXPECT_EQ(systemData.accuManagementBox.accuPowerLimit, false);
}

// test low charge limit
TEST_F(AccumulatorManagementTest, Low_Charge_Limit_Test) {
    sensorData.accu.HVcurrent = 49;
    for (int i = 0; i < 45; ++i) {
       accuManagement->update();
    }
    sensorData.accu.accuStateOfCharge = 29;
    accuManagement->update(); //is false (currentAverage = 49 > MaxDischargeCurrent = 98/2 = 49)
    EXPECT_EQ(systemData.accuManagementBox.accuPowerLimit, false);

    sensorData.accu.HVcurrent = 100;
    accuManagement->update(); //is true (currentAverage = 50 > MaxDischargeCurrent = 98/2 = 49)
    EXPECT_EQ(systemData.accuManagementBox.accuPowerLimit, true);
}

// Test if the temperature limits the maxDischargeCurrent############################################################################################################
// test critical temperature limit
TEST_F(AccumulatorManagementTest, Critical_Temperature_Limit_Test) {
    sensorData.accu.HVcurrent = 0;
    for (int i = 0; i < 45; ++i) {
       accuManagement->update();
    }
    sensorData.accu.accuTemperature = 61;
    accuManagement->update(); //is false (currentAverage = 0 > MaxDischargeCurrent = 0)
    EXPECT_EQ(systemData.accuManagementBox.accuPowerLimit, false);
}

// test high temperature limit
TEST_F(AccumulatorManagementTest, High_Temperature_Limit_Test) {
    sensorData.accu.HVcurrent = 49;
    for (int i = 0; i < 45; ++i) {
       accuManagement->update();
    }
    sensorData.accu.accuTemperature = 51;
    accuManagement->update(); //is false (currentAverage = 49 > MaxDischargeCurrent = 98/2 = 49)
    EXPECT_EQ(systemData.accuManagementBox.accuPowerLimit, false);

    sensorData.accu.HVcurrent = 100;
    accuManagement->update(); //is true (currentAverage = 50 > MaxDischargeCurrent = 98/2 = 49)
    EXPECT_EQ(systemData.accuManagementBox.accuPowerLimit, true);
}





int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}