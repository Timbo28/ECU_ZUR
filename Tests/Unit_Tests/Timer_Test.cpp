#include "gtest/gtest.h"
#include "Timer.hpp"
#include <thread>

// Timer Test ######################################################################################################
// Timer isActive Test
TEST(TimerTest, Timer_isActive_Test) {
    Timer timer;
    EXPECT_EQ(timer.isActive(), false);
    timer.start();
    EXPECT_EQ(timer.isActive(), true);
    timer.stop();
    EXPECT_EQ(timer.isActive(), false);
}
// Timer isStopped Test
TEST(TimerTest, Timer_isStopped_Test) {
    Timer timer;
    EXPECT_EQ(timer.isStopped(), true);
    timer.start();
    EXPECT_EQ(timer.isStopped(), false);
    timer.stop();
    EXPECT_EQ(timer.isStopped(), true);
}
// Timer pastTime Test
TEST(TimerTest, Timer_pastTime_Test) {
    Timer timer;
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto pastTime = timer.pastTime();
    std::cout << "past time: " << pastTime << std::endl;
    EXPECT_GE(pastTime, 100);
    timer.stop();
}
// Timer autoStopCheck Test
TEST(TimerTest, Timer_autoStopCheck_Test) {
    Timer timer(100);
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(timer.isStopped(), true);
   
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(timer.isActive(), false);

    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto pastTime = timer.pastTime();
    std::cout << "past time: " << pastTime << std::endl;
    EXPECT_EQ(pastTime, 0);

    timer.start();
    EXPECT_EQ(timer.isActive(), true);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(timer.isActive(), false);
    timer.start();
    EXPECT_EQ(timer.isActive(), true);

}

TEST(TimerTest, Timer_setTimerResolution_Test) {
    Timer timer;
    timer.setTimerResolution(ns);
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    auto pastTime = timer.pastTime();
    std::cout << "past time in ns: " << pastTime << std::endl;
    EXPECT_GE(pastTime, 1'000'000);
    timer.stop();
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
