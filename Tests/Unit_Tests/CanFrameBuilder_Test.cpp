#include <gtest/gtest.h>

// Include the header file for the code you want to test
#include "data/CanFrameBuilder.hpp"
#include "LogManager.hpp"




TEST(CanFrameBuilder, buildInputCanFrame_Test) {
    can_frame frame;
    can_frame_t canFrame;
    frame.can_id = 0x320;
    frame.can_dlc = 8;
    frame.data[0] = 0b0100'1010;
    frame.data[1] = 0b0101'0101;
    frame.data[2] = 0b1000'0001;
    frame.data[3] = 0b0010'0010;
    frame.data[4] = 0b0000'0110;
    frame.data[5] = 0x00;
    frame.data[6] = 0x00;
    frame.data[7] = 0x00;


    LogManager::init();
    CanFrameBuilder canFrameBuilder(0,"../../../Tests/Unit_Tests/CanInitTest.json");

    canFrame = canFrameBuilder.buildReceiveCanFrame(frame);
    #ifdef DEBUG
        std::cout << "\nBuilt can frame:";
        canFrame.print();
    #endif

    EXPECT_EQ(canFrame.name, std::hash<string>{}("InputTest1"));
    EXPECT_EQ(canFrame.CanBusID, 0x320);
    EXPECT_EQ(canFrame.syncFrame, 0);
    EXPECT_EQ(canFrame.AnalogData[std::hash<string>{}("AnalogInput1")].value, 341);
    EXPECT_EQ(canFrame.AnalogData[std::hash<string>{}("AnalogInput2")].value, 8833);
    EXPECT_EQ(canFrame.AnalogData[std::hash<string>{}("AnalogInput3")].value, 6);
    EXPECT_EQ(canFrame.DigitalData[std::hash<string>{}("DigitalInput1")].value, true);
    EXPECT_EQ(canFrame.DigitalData[std::hash<string>{}("DigitalInput2")].value, false);
    EXPECT_EQ(canFrame.DigitalData[std::hash<string>{}("DigitalInput3")].value, true);

    LogManager::shutdown();
}

TEST(CanFrameBuilder, buildInputCanFrame_Test2) {
    can_frame frame;
    can_frame_t canFrame;
    frame.can_id = 0x340;
    frame.can_dlc = 5;
    frame.data[0] = 0b0000'0000;
    frame.data[1] = 0b1000'0000;
    frame.data[2] = 0b1111'0100;
    frame.data[3] = 0b0011'1111;
    frame.data[4] = 0b0000'0101;
    frame.data[5] = 0x00;
    frame.data[6] = 0x00;
    frame.data[7] = 0x00;

    LogManager::init();
    CanFrameBuilder canFrameBuilder(0,"../../../Tests/Unit_Tests/CanInitTest.json");

    canFrame = canFrameBuilder.buildReceiveCanFrame(frame);
    #ifdef DEBUG
        std::cout << "\nBuilt can frame:";
        canFrame.print();
    #endif

    EXPECT_EQ(canFrame.name, std::hash<string>{}("InputTest2"));
    EXPECT_EQ(canFrame.CanBusID, 0x340);
    EXPECT_EQ(canFrame.syncFrame, 0);
    EXPECT_EQ(canFrame.AnalogData[std::hash<string>{}("AnalogInput4")].value, 1023);
    EXPECT_EQ(canFrame.AnalogData[std::hash<string>{}("AnalogInput5")].value, 9);
    EXPECT_EQ(canFrame.DigitalData[std::hash<string>{}("DigitalInput4")].value, true);
    EXPECT_EQ(canFrame.DigitalData[std::hash<string>{}("DigitalInput5")].value, true);

    LogManager::shutdown();
}



TEST(CanFrameBuilder, buildOutputCanFrame_Test) {
     LogManager::init();
    CanFrameBuilder canFrameBuilder(1,"../../../Tests/Unit_Tests/CanInitTest.json"); // Remove the parentheses after CanFrameBuilder
    // Test buildTransmitCanFrame function
    can_frame_t canFrame = canFrameBuilder.buildTransmitCanFrame(std::hash<std::string>{}("OutputTest1"));
    
    EXPECT_EQ(canFrame.name, std::hash<string>{}("OutputTest1"));
    EXPECT_EQ(canFrame.CanBusID, 0x520);
    EXPECT_EQ(canFrame.syncFrame, 0);
    EXPECT_TRUE(canFrame.AnalogData.find(std::hash<string>{}("AnalogOutput1")) != canFrame.AnalogData.end());  
    EXPECT_TRUE(canFrame.AnalogData.find(std::hash<string>{}("AnalogOutput2")) != canFrame.AnalogData.end());
    EXPECT_TRUE(canFrame.AnalogData.find(std::hash<string>{}("AnalogOutput3")) != canFrame.AnalogData.end());
    EXPECT_TRUE(canFrame.DigitalData.find(std::hash<string>{}("DigitalOutput1")) != canFrame.DigitalData.end());      
    EXPECT_TRUE(canFrame.DigitalData.find(std::hash<string>{}("DigitalOutput2")) != canFrame.DigitalData.end());

    canFrame.AnalogData[std::hash<string>{}("AnalogOutput1")].value = 2;
    canFrame.AnalogData[std::hash<string>{}("AnalogOutput2")].value = 1023;
    canFrame.AnalogData[std::hash<string>{}("AnalogOutput3")].value = 166;
    canFrame.DigitalData[std::hash<string>{}("DigitalOutput1")].value = false;
    canFrame.DigitalData[std::hash<string>{}("DigitalOutput2")].value = true;

    // Test convert to Raw Frame function
    can_frame frame = canFrameBuilder.convertToRawFrame(canFrame);

    EXPECT_EQ(frame.can_id, 0x520);
    EXPECT_EQ(frame.can_dlc, 7);
    EXPECT_EQ(frame.data[0], 0b0000'0000);
    EXPECT_EQ(frame.data[1], 0b0000'0010);
    EXPECT_EQ(frame.data[2], 0b1111'1100);
    EXPECT_EQ(frame.data[3], 0b0000'1111);
    EXPECT_EQ(frame.data[4], 0b1010'0110);
    EXPECT_EQ(frame.data[5], 0b0000'0000);
    EXPECT_EQ(frame.data[6], 0b1000'0000);
   


    LogManager::shutdown();
}

TEST(CanFrameBuilder, buildOutputCanFrame_Test2) {
    LogManager::init();
    CanFrameBuilder canFrameBuilder(1,"../../../Tests/Unit_Tests/CanInitTest.json"); // Remove the parentheses after CanFrameBuilder
    // Test buildTransmitCanFrame function
    can_frame_t canFrame = canFrameBuilder.buildTransmitCanFrame(std::hash<std::string>{}("OutputTest2"));
    
    EXPECT_EQ(canFrame.name, std::hash<string>{}("OutputTest2"));
    EXPECT_EQ(canFrame.CanBusID, 0x540);
    EXPECT_EQ(canFrame.syncFrame, 0);
    EXPECT_TRUE(canFrame.AnalogData.find(std::hash<string>{}("AnalogOutput4")) != canFrame.AnalogData.end());  
    EXPECT_TRUE(canFrame.AnalogData.find(std::hash<string>{}("AnalogOutput5")) != canFrame.AnalogData.end());
    EXPECT_TRUE(canFrame.AnalogData.find(std::hash<string>{}("AnalogOutput6")) != canFrame.AnalogData.end());
    EXPECT_TRUE(canFrame.DigitalData.find(std::hash<string>{}("DigitalOutput3")) != canFrame.DigitalData.end());      
    EXPECT_TRUE(canFrame.DigitalData.find(std::hash<string>{}("DigitalOutput4")) != canFrame.DigitalData.end());

    canFrame.AnalogData[std::hash<string>{}("AnalogOutput4")].value = 57;
    canFrame.AnalogData[std::hash<string>{}("AnalogOutput5")].value = 1023;
    canFrame.AnalogData[std::hash<string>{}("AnalogOutput6")].value = 153;
    canFrame.DigitalData[std::hash<string>{}("DigitalOutput3")].value = true;
    canFrame.DigitalData[std::hash<string>{}("DigitalOutput4")].value = false;
    

    // Test convert to Raw Frame function
    can_frame frame = canFrameBuilder.convertToRawFrame(canFrame);

    EXPECT_EQ(frame.can_id, 0x540);
    EXPECT_EQ(frame.can_dlc, 4);
    EXPECT_EQ(frame.data[0], 0b1110'0101);
    EXPECT_EQ(frame.data[1], 0b1111'1111);
    EXPECT_EQ(frame.data[2], 0b0110'0111);
    EXPECT_EQ(frame.data[3], 0b0000'0010);
 

    LogManager::shutdown();

}

TEST(CanFrameBuilder, ByteInvertedInput_Test) {
     can_frame frame;
    can_frame_t canFrame;
    frame.can_id = 0x7700;
    frame.can_dlc = 4;
    frame.data[0] = 0x98;
    frame.data[1] = 0x12;
    frame.data[2] = 0xAF;
    frame.data[3] = 0xDC;
    frame.data[4] = 0x00;
    frame.data[5] = 0x00;
    frame.data[6] = 0x00;
    frame.data[7] = 0x00;

    LogManager::init();
    CanFrameBuilder canFrameBuilder(2,"../../../Tests/Unit_Tests/CanInitTest.json");

    canFrame = canFrameBuilder.buildReceiveCanFrame(frame);
    #ifdef DEBUG
        std::cout << "\nBuilt can frame:";
        canFrame.print();
    #endif

    EXPECT_EQ(canFrame.name, std::hash<string>{}("ByteInvertedTest"));
    EXPECT_EQ(canFrame.CanBusID, 0x7700);
    EXPECT_EQ(canFrame.syncFrame, 0);
    EXPECT_EQ(canFrame.AnalogData[std::hash<string>{}("notInverted")].value, 0x1298);
    EXPECT_EQ(canFrame.AnalogData[std::hash<string>{}("inverted")].value, (int16_t)0xAFDC);

    LogManager::shutdown();
}

TEST(CanFrameBuilder, ByteInvertedOutput_Test) {
    LogManager::init();
    CanFrameBuilder canFrameBuilder(2,"../../../Tests/Unit_Tests/CanInitTest.json"); // Remove the parentheses after CanFrameBuilder
    // Test buildTransmitCanFrame function
    can_frame_t canFrame = canFrameBuilder.buildTransmitCanFrame(std::hash<std::string>{}("ByteInvertedOutputTest"));
    
    EXPECT_EQ(canFrame.name, std::hash<string>{}("ByteInvertedOutputTest"));
    EXPECT_EQ(canFrame.CanBusID, 0x7701);
    EXPECT_EQ(canFrame.syncFrame, 0);
    EXPECT_TRUE(canFrame.AnalogData.find(std::hash<string>{}("notInverted1")) != canFrame.AnalogData.end());  
    EXPECT_TRUE(canFrame.AnalogData.find(std::hash<string>{}("inverted1")) != canFrame.AnalogData.end());

    canFrame.AnalogData[std::hash<string>{}("notInverted1")].value = 0x1234;
    canFrame.AnalogData[std::hash<string>{}("inverted1")].value = 0xABCD;
    

    // Test convert to Raw Frame function
    can_frame frame = canFrameBuilder.convertToRawFrame(canFrame);

    EXPECT_EQ(frame.can_id, 0x7701);
    EXPECT_EQ(frame.can_dlc, 4);
    EXPECT_EQ(frame.data[0], 0x34);
    EXPECT_EQ(frame.data[1], 0x12);
    EXPECT_EQ(frame.data[2], 0xAB);
    EXPECT_EQ(frame.data[3], 0xCD);
 

    LogManager::shutdown();

}





// Define the main function to run all the tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
