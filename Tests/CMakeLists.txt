


enable_testing()

add_subdirectory(Test_Librarys/googletest)  # Pfad zum GoogleTest-Verzeichnis

add_executable(ControlSystemTest ControlSystem_test.cpp
                                    ${SOURCE_FILES})

target_link_libraries(ControlSystemTest gtest gtest_main spdlog::spdlog gmock gmock_main)


add_subdirectory(Unit_Tests)


add_test(NAME ControlSystemTest COMMAND ControlSystemTest)