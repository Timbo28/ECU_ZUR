#include <gtest/gtest.h>
#include <boost/pfr.hpp>


#include "data/CanStructMapper.hpp"
#include "data/ecuJson.hpp"
#include "LogManager.hpp"

 struct test_struct_t{
        int16_t mapData1;
        uint16_t mapData2;
        float floatData1;
        double doubleData;
        uint32_t uint32Data1;
        int64_t int64Data1;
        uint64_t uint64Data1;
        struct boolStruct_t{
            bool mapData3;
            bool mapData4;
        }boolStruct;

        struct out_struct_t{
            int16_t mapData5;
            uint16_t mapData6;
            bool mapData7;
            bool mapData8;
        }outStruct;
    };

struct sync_test_struct_t{
    int16_t syncData1 =0;
    uint16_t syncData2 = 0;
    int16_t syncData3 = 0;
    struct child_struct_t{
        bool syncData4 = false;
        int16_t syncData6 = 0;
    }childStruct;
    bool syncData5 = false;
    uint16_t syncData7 = 0;
    uint16_t syncData8 = 0;
    bool syncData9 = false;
    bool syncData10 = false;
};

class CanStructMapper_Test : public ::testing::Test {
protected:
    void SetUp() override {
        LogManager::init();
        canStructMapper = new CanStructMapper("../../../Tests/Unit_Tests/CanInitTest.json");
        canStructMapper->registerStruct(test_struct);
    }

    void TearDown() override {
        delete canStructMapper;
        LogManager::shutdown();
    }
    test_struct_t test_struct;
    CanStructMapper* canStructMapper;
};



TEST_F(CanStructMapper_Test, mapInputCanFrame_Test) {
    can_frame_t canFrame;
    canFrame.name = std::hash<string>{}("MappingTest1");
    canFrame.CanBusID = 0x680;
    canFrame.syncFrame = 0;
    canFrame.AnalogData[std::hash<string>{}("mapData1")].value = 888;
    canFrame.AnalogData[std::hash<string>{}("mapData1")].size = 10;
    canFrame.AnalogData[std::hash<string>{}("mapData2")].value = 8833;
    canFrame.AnalogData[std::hash<string>{}("mapData2")].size = 16;
    canFrame.DigitalData[std::hash<string>{}("mapData3")].value = true;
    canFrame.DigitalData[std::hash<string>{}("mapData4")].value = false;

    canStructMapper->mapInputCanFrame(canFrame);

    EXPECT_EQ(test_struct.mapData1, 888);
    EXPECT_EQ(test_struct.mapData2, 8833);
    EXPECT_EQ(test_struct.boolStruct.mapData3, true);
    EXPECT_EQ(test_struct.boolStruct.mapData4, false);

    can_frame_t canFrame1;
    canFrame1.name = std::hash<string>{}("datatypeTest1");
    canFrame1.CanBusID = 0x222;
    canFrame1.syncFrame = 0;
    canFrame1.AnalogData[std::hash<string>{}("floatData1")].value = 0b01000001110011000000000000000000; // = 25.5
    canFrame1.AnalogData[std::hash<string>{}("floatData1")].size = 32;
    canFrame1.AnalogData[std::hash<string>{}("uint32Data1")].value = 536870912;
    canFrame1.AnalogData[std::hash<string>{}("uint32Data1")].size = 32;

    test_struct.floatData1 = 0;

    canStructMapper->mapInputCanFrame(canFrame1);

    //print floatData1 in binary
    int i;
    std::memcpy(&i, &test_struct.floatData1, sizeof(i));
    
    std::cout << "floatData1: " << std::bitset<32>(i) << std::endl;
    
    EXPECT_EQ(test_struct.uint32Data1, 536870912);
    EXPECT_EQ(test_struct.floatData1, 25.5);

    can_frame_t canFrame2;
    canFrame2.name = std::hash<string>{}("datatypeTest4");
    canFrame2.CanBusID = 0x200;
    canFrame2.syncFrame = 0;
    canFrame2.AnalogData[std::hash<string>{}("uint64Data1")].value = 1844674407370955161; 
    canFrame2.AnalogData[std::hash<string>{}("uint64Data1")].size = 64;

    test_struct.uint64Data1 = 0;

    canStructMapper->mapInputCanFrame(canFrame2);

    EXPECT_EQ(test_struct.uint64Data1, 1844674407370955161);
}

// sync frame test
TEST_F(CanStructMapper_Test, mapInputCanFrame_sync_Test) {
    sync_test_struct_t syncTestStruct;
    canStructMapper->registerStruct(syncTestStruct);

    can_frame_t canFrame;
    canFrame.name = std::hash<string>{}("MappingTest1");
    canFrame.CanBusID = 0x680;
    canFrame.syncFrame = 0;
    canFrame.AnalogData[std::hash<string>{}("mapData1")].value = 888;
    canFrame.AnalogData[std::hash<string>{}("mapData1")].size = 10;
    canFrame.AnalogData[std::hash<string>{}("mapData2")].value = 8833;
    canFrame.AnalogData[std::hash<string>{}("mapData2")].size = 16;
    canFrame.DigitalData[std::hash<string>{}("mapData3")].value = true;
    canFrame.DigitalData[std::hash<string>{}("mapData4")].value = false;

    can_frame_t canFrame1;
    canFrame1.name = std::hash<string>{}("SyncFrame1");
    canFrame1.CanBusID = 0x870;
    canFrame1.syncFrame = std::hash<string>{}("SyncFrame2");
    canFrame1.AnalogData[std::hash<string>{}("syncData1")].value = 888;
    canFrame1.AnalogData[std::hash<string>{}("syncData1")].size = 10;
    canFrame1.AnalogData[std::hash<string>{}("syncData2")].value = 8833;
    canFrame1.AnalogData[std::hash<string>{}("syncData2")].size = 16;

    canFrame1.AnalogData[std::hash<string>{}("syncData3")].value = 6;
    canFrame1.AnalogData[std::hash<string>{}("syncData3")].size = 4;

    canFrame1.DigitalData[std::hash<string>{}("syncData4")].value = true;
    canFrame1.DigitalData[std::hash<string>{}("syncData5")].value = false;

    can_frame_t canFrame2;
    canFrame2.name = std::hash<string>{}("SyncFrame2");
    canFrame2.CanBusID = 0x860;
    canFrame2.syncFrame = std::hash<string>{}("SyncFrame1");
    canFrame2.AnalogData[std::hash<string>{}("syncData6")].value = 888;
    canFrame2.AnalogData[std::hash<string>{}("syncData6")].size = 10;
    canFrame2.AnalogData[std::hash<string>{}("syncData7")].value = 8833;
    canFrame2.AnalogData[std::hash<string>{}("syncData7")].size = 16;
    canFrame2.AnalogData[std::hash<string>{}("syncData8")].value = 6;
    canFrame2.AnalogData[std::hash<string>{}("syncData8")].size = 4;
    canFrame2.DigitalData[std::hash<string>{}("syncData9")].value = true;
    canFrame2.DigitalData[std::hash<string>{}("syncData10")].value = false;

    canStructMapper->mapInputCanFrame(canFrame1);
    canStructMapper->mapInputCanFrame(canFrame);
    EXPECT_EQ(syncTestStruct.syncData1, 0);
    EXPECT_EQ(syncTestStruct.syncData2, 0);
    EXPECT_EQ(syncTestStruct.syncData3, 0);
    EXPECT_EQ(syncTestStruct.childStruct.syncData4, 0);
    EXPECT_EQ(syncTestStruct.childStruct.syncData6, 0);
    EXPECT_EQ(syncTestStruct.syncData5, 0);
    EXPECT_EQ(syncTestStruct.syncData7, 0);
    EXPECT_EQ(syncTestStruct.syncData8, 0);
    EXPECT_EQ(syncTestStruct.syncData9, 0);
    EXPECT_EQ(syncTestStruct.syncData10, 0);
    canStructMapper->mapInputCanFrame(canFrame2);
    EXPECT_EQ(syncTestStruct.syncData1, 888);
    EXPECT_EQ(syncTestStruct.syncData2, 8833);
    EXPECT_EQ(syncTestStruct.syncData3, 6);
    EXPECT_EQ(syncTestStruct.childStruct.syncData4, true);
    EXPECT_EQ(syncTestStruct.childStruct.syncData6, 888);
    EXPECT_EQ(syncTestStruct.syncData5, false);
    EXPECT_EQ(syncTestStruct.syncData7, 8833);
    EXPECT_EQ(syncTestStruct.syncData8, 6);
    EXPECT_EQ(syncTestStruct.syncData9, true);
    EXPECT_EQ(syncTestStruct.syncData10, false);

}

TEST_F(CanStructMapper_Test, mapOutputCanFrame_Test) {
    can_frame_t canFrame;
    canFrame.name = std::hash<string>{}("MappingTest2");
    canFrame.CanBusID = 0x680;
    canFrame.syncFrame = 0;
    canFrame.AnalogData[std::hash<string>{}("mapData5")].value = 0;
    canFrame.AnalogData[std::hash<string>{}("mapData5")].size = 10;
    canFrame.AnalogData[std::hash<string>{}("mapData6")].value = 0;
    canFrame.AnalogData[std::hash<string>{}("mapData6")].size = 16;
    canFrame.DigitalData[std::hash<string>{}("mapData7")].value = true;
    canFrame.DigitalData[std::hash<string>{}("mapData8")].value = false;

    test_struct.outStruct.mapData5 = 888;
    test_struct.outStruct.mapData6 = 8833;
    test_struct.outStruct.mapData7 = false;
    test_struct.outStruct.mapData8 = true;

    canStructMapper->mapOutputCanFrame(canFrame);

    EXPECT_EQ(canFrame.AnalogData[std::hash<string>{}("mapData5")].value, 888);
    EXPECT_EQ(canFrame.AnalogData[std::hash<string>{}("mapData6")].value, 8833);
    EXPECT_EQ(canFrame.DigitalData[std::hash<string>{}("mapData7")].value, false);
    EXPECT_EQ(canFrame.DigitalData[std::hash<string>{}("mapData8")].value, true);


    can_frame_t canFrame1;
    canFrame1.name = std::hash<string>{}("datatypeTest2");
    canFrame1.CanBusID = 0x202;
    canFrame1.syncFrame = 0;
    canFrame1.AnalogData[std::hash<string>{}("doubleData")].value = 0; 
    canFrame1.AnalogData[std::hash<string>{}("doubleData")].size = 64;

    test_struct.doubleData = -1.25;

    canStructMapper->mapOutputCanFrame(canFrame1);

     //print floatData1 in binary
    int64_t i;
    std::memcpy(&i, &canFrame1.AnalogData[std::hash<string>{}("doubleData")].value, sizeof(i));
    
    std::cout << "floatData1: " << std::bitset<64>(i) << std::endl;

    EXPECT_EQ(canFrame1.AnalogData[std::hash<string>{}("doubleData")].value, 0b1011111111110100000000000000000000000000000000000000000000000000);


    can_frame_t canFrame2;
    canFrame2.name = std::hash<string>{}("datatypeTest3");
    canFrame2.CanBusID = 0x201;
    canFrame2.syncFrame = 0;
    canFrame2.AnalogData[std::hash<string>{}("int64Data1")].value = 0;
    canFrame2.AnalogData[std::hash<string>{}("int64Data1")].size = 64;

    test_struct.int64Data1 = -7307071977898606207;

    canStructMapper->mapOutputCanFrame(canFrame2);

    EXPECT_EQ(canFrame2.AnalogData[std::hash<string>{}("int64Data1")].value, -7307071977898606207);
}

TEST_F(CanStructMapper_Test, getFrameCanMap_Test) {
    std::unordered_map<std::size_t, uint8_t> frameCanMap = canStructMapper->getFrameCanMap();
    //print the frameCanMap
    for(auto& [key,value]: frameCanMap){
        std::cout << "Key: " << key << " Value: " << static_cast<int>(value) << std::endl;
    }
    
    EXPECT_EQ(frameCanMap[std::hash<string>{}("InitFrameTest1")], 0);
    EXPECT_EQ(frameCanMap[std::hash<string>{}("OutputTest1")], 1);
    EXPECT_EQ(frameCanMap[std::hash<string>{}("OutputTest2")], 1);
    EXPECT_EQ(frameCanMap[std::hash<string>{}("MappingTest2")], 2);
}



int main(int argc, char const *argv[])
{
    testing::InitGoogleTest(&argc, (char**)argv);
    return RUN_ALL_TESTS();
}
