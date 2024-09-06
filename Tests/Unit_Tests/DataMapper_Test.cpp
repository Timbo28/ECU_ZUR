#include <gtest/gtest.h>

#include "Timer.hpp"
#include "data/DataMapper.hpp"
#include "Error.hpp"
#include "data/ecuJson.hpp"
#include "LogManager.hpp"

class DataMapperTest : public ::testing::Test {
protected:
    void SetUp() override {
        LogManager::init();
        dataMapper = new DataMapper("../../../Tests/Unit_Tests/senMLConfigTest.json",2);
        dataMapper->registerStruct(sensorData);
        dataMapper->registerStruct(systemData, true);
    }

    void TearDown() override {
        delete dataMapper;
        LogManager::shutdown();
    }
    DataMapper* dataMapper;
    sensor_data_t sensorData;
    system_data_t systemData;
};

TEST_F(DataMapperTest, getSenMLTest) {
    systemData.pedalBox.throttle = 50;
    systemData.pedalBox.breakPercentage = 30;
    systemData.pedalBox.breakPressed = false;
    systemData.motorControllerBox.currentState = DriveState;
    nlohmann::json senML = dataMapper->handelDataRequest("ECU","pedal","sensor");
    std::cout << senML.dump(4) << std::endl;
    EXPECT_EQ(senML[0]["bn"],"ECU");
    EXPECT_EQ(senML[0]["n"],"pedal");
    EXPECT_EQ(senML[0]["vs"],"sensor");
    EXPECT_EQ(senML[1]["v"],systemData.pedalBox.throttle);
    EXPECT_EQ(senML[2]["v"],systemData.pedalBox.breakPercentage);
    EXPECT_EQ(senML[3]["vb"],systemData.pedalBox.breakPressed);
    EXPECT_EQ(senML[4]["v"],DriveState);
}

TEST_F(DataMapperTest, multi_getSenMLTest){
    systemData.pedalBox.throttle = 50;
    systemData.pedalBox.breakPercentage = 30;
    systemData.pedalBox.breakPressed = false;
    systemData.motorControllerBox.currentState = DriveState;

    sensorData.inverter.motorTemp_FL = 45;
    sensorData.inverter.inverterTemp_FL = 22;
    sensorData.inverter.IGBTTemp_FR = 88;
    sensorData.SDC_Pressed = false;

    systemData.CANOutput.out_ASSIblue = false;
    systemData.CANOutput.out_ASSIgreen = true;
    systemData.CANOutput.out_ASSIred = true;



    nlohmann::json request = nlohmann::json::array();
    request.push_back({{{"bn","ECU"},{"n","pedal"},{"vs","sensor"}}});
    request.push_back({{{"bn","ECU"},{"n","inverter"},{"vs","sensor"}}});
    request.push_back({{{"bn","ECU"},{"n","assi"},{"vs","actuator"}}});

    nlohmann::json senML = dataMapper->getSenML(request);
    std::cout << senML.dump(4) << std::endl;

    senML = dataMapper->getSenML(request);
    std::cout << senML.dump(4) << std::endl;

     senML = dataMapper->getSenML(request);
    std::cout << senML.dump(4) << std::endl;

    senML = dataMapper->getSenML(request);
    std::cout << senML.dump(4) << std::endl;


    EXPECT_EQ(senML[0][0]["bn"],"ECU");
    EXPECT_EQ(senML[0][0]["n"],"pedal");
    EXPECT_EQ(senML[0][0]["vs"],"sensor");
    EXPECT_EQ(senML[0][1]["v"],systemData.pedalBox.throttle);
    EXPECT_EQ(senML[0][2]["v"],systemData.pedalBox.breakPercentage);
    EXPECT_EQ(senML[0][3]["vb"],systemData.pedalBox.breakPressed);
    EXPECT_EQ(senML[0][4]["v"],DriveState);

    EXPECT_EQ(senML[1][0]["bn"],"ECU");
    EXPECT_EQ(senML[1][0]["n"],"inverter");
    EXPECT_EQ(senML[1][0]["vs"],"sensor");
    EXPECT_EQ(senML[1][1]["v"],sensorData.inverter.motorTemp_FL);
    EXPECT_EQ(senML[1][2]["v"],sensorData.inverter.inverterTemp_FL);
    EXPECT_EQ(senML[1][3]["v"],sensorData.inverter.IGBTTemp_FR);
    EXPECT_EQ(senML[1][4]["vb"],sensorData.SDC_Pressed);

    EXPECT_EQ(senML[2][0]["bn"],"ECU");
    EXPECT_EQ(senML[2][0]["n"],"assi");
    EXPECT_EQ(senML[2][0]["vs"],"actuator");
    EXPECT_EQ(senML[2][1]["vb"],systemData.CANOutput.out_ASSIred);
    EXPECT_EQ(senML[2][2]["vb"],systemData.CANOutput.out_ASSIgreen);
    EXPECT_EQ(senML[2][3]["vb"],systemData.CANOutput.out_ASSIblue);
    EXPECT_EQ(senML[2][4]["vb"],systemData.CANOutput.out_ASSIred);
    EXPECT_EQ(senML[2][5]["vb"],systemData.CANOutput.out_ASSIgreen);
    EXPECT_EQ(senML[2][6]["vb"],systemData.CANOutput.out_ASSIblue);
    EXPECT_EQ(senML[2][7]["vb"],systemData.CANOutput.out_ASSIred);
    EXPECT_EQ(senML[2][8]["vb"],systemData.CANOutput.out_ASSIgreen);
    EXPECT_EQ(senML[2][9]["vb"],systemData.CANOutput.out_ASSIblue);
}

TEST_F(DataMapperTest, getSenMLTestErrorCode){
    nlohmann::json senML = dataMapper->handelDataRequest("ECU","error","code");
    std::cout << senML.dump(4) << std::endl;
    EXPECT_EQ(senML[0]["bn"],"ECU");
    EXPECT_EQ(senML[0]["n"],"error");
    EXPECT_EQ(senML[0]["vs"],"code");
    EXPECT_EQ(senML[1]["v"],ecu::Error::getErrorCode());

    ecu::Error::setErrorCode(28);

    senML = dataMapper->handelDataRequest("ECU","error","code");
    std::cout << senML.dump(4) << std::endl;
    EXPECT_EQ(senML[1]["v"],28);
}

TEST_F(DataMapperTest, getSenMLTestErrorMap){
    nlohmann::json senML = dataMapper->handelDataRequest("ECU","error","map");
    std::cout << senML.dump(4) << std::endl;
 
}

TEST_F(DataMapperTest, setSenMLTest){
        
    nlohmann::json request = nlohmann::json::array();
    request.push_back({{{"bn","ECU"},{"n","DV"},{"vs","actuator"}},
                       {{"n","throttleLeftMax"},{"u","%"},{"v",28}},
                       {{"n","breakPressureFront"},{"u","%"},{"v",88}},
                       {{"n","SDC_Pressed"},{"vb",true}},
                       {{"n","out_targetTorque_FL"},{"u","c"},{"v",3}}});

    nlohmann::json senML = dataMapper->getSenML(request);
    std::cout << senML.dump(4) << std::endl;

    EXPECT_EQ(sensorData.pedal.throttleLeftMax,28);
    EXPECT_EQ(sensorData.pedal.breakPressureFront,88);
    EXPECT_EQ(sensorData.SDC_Pressed,true);
    EXPECT_NE(systemData.CANOutput.out_targetTorque_FL,3);
}

TEST_F(DataMapperTest, multi_setSenMLTest){
        
    nlohmann::json request = nlohmann::json::array();
    request.push_back({{{"bn","ECU"},{"n","DV"},{"vs","actuator"}},
                       {{"n","throttleLeftMax"},{"u","%"},{"v",128}},
                       {{"n","breakPressureFront"},{"u","%"},{"v",188}},
                       {{"n","SDC_Pressed"},{"vb",true}},
                       {{"n","out_targetTorque_FL"},{"u","c"},{"v",13}}});

    request.push_back({{{"bn","ECU"},{"n","pedal"},{"vs","sensor"}}});

    request.push_back({{{"bn","ECU"},{"n","inverter"},{"vs","sensor"}}});

    request.push_back({{{"bn","ECU"},{"n","diagnistic"},{"vs","actuator"}},
                       {{"n","accuStateOfCharge"},{"u","%"},{"v",63}},
                       {{"n","prechargeRelay"},{"u","%"},{"v",98}},
                       {{"n","TSButton"},{"vb",true}},
                       {{"n","invertersVoltage_FL"},{"u","c"},{"v",7}}});
   
    nlohmann::json senML = dataMapper->getSenML(request);
    std::cout << senML.dump(4) << std::endl;

    EXPECT_EQ(sensorData.pedal.throttleLeftMax,128);
    EXPECT_EQ(sensorData.pedal.breakPressureFront,188);
    EXPECT_EQ(sensorData.SDC_Pressed,true);
    EXPECT_NE(systemData.CANOutput.out_targetTorque_FL,13);

    EXPECT_EQ(sensorData.accu.accuStateOfCharge,63);
    EXPECT_EQ(sensorData.accu.prechargeRelay,98);
    EXPECT_EQ(sensorData.cockpit.TSButton,true);
    EXPECT_EQ(sensorData.inverter.invertersVoltage_FL,7);
}

// Define the main function to run all the tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
