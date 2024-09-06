#   Add Motor Controller Unit Tests
add_executable(MotorControllerTests     MotorController_test.cpp
                                        ../../source/system/MotorControllerStates.cpp
                                        ../../source/system/MotorController.cpp
                                        ../../source/Timer.cpp
                                        ../../source/LogManager.cpp
                                        ../../source/Error.cpp
                                        ../../source/system/arrayCompareLogic.cpp )
target_link_libraries(MotorControllerTests gtest gtest_main spdlog::spdlog)

#   Add LED Button Box Unit Test
add_executable(LedButtonTest            LedButton_Test.cpp
                                        ../../source/system/LedButtonBox.cpp
                                        ../../source/Timer.cpp)
target_link_libraries(LedButtonTest gtest gtest_main )

#   Add Accumulator Management Unit Test
add_executable(AccumulatorManagementTest AccumulatorManagement_Test.cpp
                                        ../../source/system/AccumulatorManagement.cpp
                                        ../../source/Timer.cpp
                                        ../../source/Error.cpp
                                        ../../source/LogManager.cpp)
target_link_libraries(AccumulatorManagementTest gtest gtest_main spdlog::spdlog)

#   Add Pedal Box Unit Test
add_executable(PedalBoxTest             PedalBox_Test.cpp
                                        ../../source/system/PedalBox.cpp
                                        ../../source/system/PedalBoxStates.cpp
                                        ../../source/LogManager.cpp
                                        ../../source/Timer.cpp
                                        ../../source/Error.cpp
                                        ../../source/system/throttleLogic.cpp)
target_link_libraries(PedalBoxTest gtest gtest_main spdlog::spdlog)

#   Add Timer Unit Test
add_executable(TimerTest                Timer_Test.cpp
                                        ../../source/Timer.cpp)
target_link_libraries(TimerTest gtest gtest_main )



#   Add Can Frame Builder Unit Test 
add_executable(CanFrameBuilderTest      CanFrameBuilder_Test.cpp
                                        ../../source/data/CanFrameBuilder.cpp
                                        ../../source/data/ecuJson.cpp
                                        ../../source/Timer.cpp
                                        ../../source/Error.cpp
                                        ../../source/LogManager.cpp
                                        )
target_link_libraries(CanFrameBuilderTest gtest gtest_main nlohmann_json::nlohmann_json spdlog::spdlog)

add_executable(ErrorTest               Error_Test.cpp
                                       ../../source/Error.cpp
                                       ../../source/Timer.cpp
                                       )
target_link_libraries(ErrorTest gtest gtest_main nlohmann_json::nlohmann_json spdlog::spdlog)

add_executable(CanStructMapper_Test    CanStructMapper_Test.cpp
                                       ../../source/data/ecuJson.cpp
                                       ../../source/data/CanStructMapper.cpp
                                       ../../source/data/CanFrameBuilder.cpp
                                       ../../source/Timer.cpp
                                       ../../source/Error.cpp
                                       ../../source/LogManager.cpp
                                       )
target_link_libraries(CanStructMapper_Test gtest gtest_main nlohmann_json::nlohmann_json spdlog::spdlog boost_pfr)

add_executable(DataMapper_Test    DataMapper_Test.cpp
                                       ../../source/data/ecuJson.cpp
                                       ../../source/data/DataMapper.cpp
                                       ../../source/Timer.cpp
                                       ../../source/Error.cpp
                                       ../../source/LogManager.cpp
                                       )
target_link_libraries(DataMapper_Test gtest gtest_main nlohmann_json::nlohmann_json spdlog::spdlog boost_pfr)

add_executable(Steering_Test    Steering_Test.cpp
                                       ../../source/system/Steering.cpp
                                       ../../source/Timer.cpp
                                       ../../source/Error.cpp
                                       ../../source/LogManager.cpp
                                       )
target_link_libraries(Steering_Test gtest gtest_main nlohmann_json::nlohmann_json spdlog::spdlog boost_pfr)


add_test(NAME Motor_Controller_UnitTest COMMAND MotorControllerTests)
add_test(NAME Pedal_Box_UnitTest COMMAND PedalBoxTest)
add_test(NAME Led_Button_UnitTest COMMAND LedButtonTest)
add_test(NAME Accumulator_Management_UnitTest COMMAND AccumulatorManagementTest)
add_test(NAME Timer_UnitTest COMMAND TimerTest)
add_test(NAME Can_Frame_Builder_UnitTest COMMAND CanFrameBuilderTest)
add_test(NAME Error_UnitTest COMMAND ErrorTest)
add_test(NAME Can_Struct_Mapper_UnitTest COMMAND CanStructMapper_Test)
add_test(NAME Data_Mapper_UnitTest COMMAND DataMapper_Test)
add_test(NAME Steering_UnitTest COMMAND Steering_Test)