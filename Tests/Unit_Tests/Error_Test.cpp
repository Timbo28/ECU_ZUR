#include <gtest/gtest.h>

#include "Error.hpp"
#include "Timer.hpp"

TEST(Error, setErrorCode) {
    CLEAR_ERRORS();
    ecu::Error::setErrorCode(CAN0_SOCKET_BIND_ERROR);
    EXPECT_EQ(ecu::Error::NoErrors(), false);
    ecu::Error::flushErrorCodeBuffer();
    EXPECT_EQ(ecu::Error::NoErrors(), true);
    ecu::Error::setErrorCode(CAN0_SOCKET_BIND_ERROR);
    EXPECT_EQ(ecu::Error::NoErrors(), false);
}

TEST(Error, setErrorCode2) {
    CLEAR_ERRORS();
    ecu::Error::setErrorCode(CAN0_CLOSE_SOCKET_ERROR);
    ecu::Error::setErrorCode(CAN0_SOCKET_BIND_ERROR);
    std::vector<error_code_t> errors =  ecu::Error::getErrorBuffer();

    std::cout << "Error:" << errors[1] << std::endl;
    EXPECT_EQ(errors[1], CAN0_SOCKET_BIND_ERROR);

    std::cout << "ErrorCode:" << errors[0] << std::endl;
    EXPECT_EQ(errors[0], CAN0_CLOSE_SOCKET_ERROR);
        
}

TEST(Error, hasCriticalError){
    CLEAR_ERRORS();
    ecu::Error::setErrorCode(CAN0_CLOSE_SOCKET_ERROR);
    ecu::Error::setErrorCode(CAN0_SOCKET_BIND_ERROR);
    EXPECT_EQ(ecu::Error::hasCriticalError(), true);
    ecu::Error::flushErrorCodeBuffer();
    ecu::Error::setErrorCode(CAN0_CLOSE_SOCKET_ERROR);
    ecu::Error::setErrorCode(CAN0_SOCKET_BIND_ERROR);
    ecu::Error::setErrorCode(CAN0_INIT_SOCKET_ERROR);
    EXPECT_EQ(ecu::Error::hasCriticalError(), true);
}

TEST(Error, printError){
    CLEAR_ERRORS();
    ecu::Error::setErrorCode(CAN0_CLOSE_SOCKET_ERROR);
    ecu::Error::setErrorCode(CAN0_SOCKET_BIND_ERROR);
    ecu::Error::printErrors();
    SUCCEED();
}

TEST(Error, getErrorJson){
    CLEAR_ERRORS();
    nlohmann::json errorJson = ecu::Error::getErrorMapJson();
    std::cout << errorJson.dump(4) << std::endl;
    SUCCEED();
}

TEST(Error, numberOfErrors){
    CLEAR_ERRORS();
    ecu::Error::setErrorCode(CAN0_SOCKET_BIND_ERROR);
    ecu::Error::setErrorCode(CAN0_CLOSE_SOCKET_ERROR);
    EXPECT_EQ(ecu::Error::numberOfErrors(), 2);
    ecu::Error::setErrorCode(CAN0_CLOSE_SOCKET_ERROR);
    ecu::Error::setErrorCode(CAN0_SOCKET_BIND_ERROR);
    EXPECT_EQ(ecu::Error::numberOfErrors(), 2);

    ecu::Error::flushErrorCodeBuffer();
    EXPECT_EQ(ecu::Error::numberOfErrors(), 0);
}



TEST(ErrorTiming, jsonTiming){
    CLEAR_ERRORS();
    Timer timer;
    timer.setTimerResolution(ns);
    ecu::Error::setErrorCode(CAN0_SOCKET_BIND_ERROR);
    ecu::Error::setErrorCode(CAN0_CLOSE_SOCKET_ERROR);
    ecu::Error::setErrorCode(CAN0_INIT_SOCKET_ERROR);
    
    // Start Timer
    timer.start();
    nlohmann::json errorJson = ecu::Error::getErrorMapJson();
    // Stop Timer
    auto time = timer.pastTime();
    timer.stop();
    std::cout << "Time for Error Map to Json = " << time << " ns" << std::endl;
   
    // Start Timer
    timer.start();
    auto errorCodeJson = ecu::Error::getErrorCodeJson();
    if(errorCodeJson.has_value()){
        time = timer.pastTime();
        timer.stop();// Stop Timer
        std::cout << "Time for Error Code to Json = " << time << " ns" << std::endl;
    }else{
        std::cout << "Error Code Json is empty" << std::endl;   
        FAIL();
    }
    SUCCEED();
}

TEST(ErrorTiming, setErrorTiming){
    CLEAR_ERRORS();

    Timer timer;
    timer.setTimerResolution(ns);
    // Start Timer
    timer.start();
    ecu::Error::setErrorCode(PRECHARGE_CIRCUIT_TIMEOUT_ERROR);
    // Stop Timer
    auto time = timer.pastTime();
    timer.stop();
    std::cout << "Time for first Error  = " << time << " ns" << std::endl;

    // Start Timer
    timer.start();
    ecu::Error::setErrorCode(35);
    // Stop Timer
    time = timer.pastTime();
    timer.stop();
    std::cout << "Time for second Error Code = " << time << " ns" << std::endl;
    SUCCEED();
}







// Define the main function to run all the tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
