#include "gtest/gtest.h"
#include "LogManager.hpp"
#include "system/Steering.hpp"
#include "data/database.hpp"

class SteeringTest : public ::testing::Test {
protected:
    void SetUp() override {
        LogManager::init();
        steering = new Steering(&sensorData, &dvCommands, &systemData); 
    }

    void TearDown() override {
        delete steering;
        LogManager::shutdown();
    }
    settings_t settings;
    sensor_data_t sensorData;
    system_data_t systemData;
    driverless_commands_t dvCommands;
    Steering* steering;
};

TEST_F(SteeringTest, testSteering) {
    sensorData.DVon = true;
    dvCommands.steering = 10;
    steering->update();
    EXPECT_EQ(systemData.CANOutput.out_steeringPosition, 10);
    steering->update();
    EXPECT_EQ(systemData.CANOutput.out_steeringPosition, 10);
    steering->update();
    EXPECT_EQ(systemData.CANOutput.out_steeringPosition, 10);
}
   
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
