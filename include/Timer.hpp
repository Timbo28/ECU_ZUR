/**
 * @file	Timer.c
 * @authors	Marco Rau (raumar02@students.zhaw.ch)
 * 		    Tim Roos  (roostim1@students.zhaw.ch)
 * @brief 	simple stopwatch
 * @version	v1.0.0
 * @date	2023-12-22
 *
 * @copyright Copyright (c) 2023 Zurich UAS Racing
 *
 */

#ifndef ECU_UASRACING_TIMER_HPP
#define ECU_UASRACING_TIMER_HPP

#include <chrono>
#include "main.hpp"

enum TimeResolution{
    s = 1'000'000'000,
    ms = 1'000'000,
    us = 1000,
    ns = 1
};

 using namespace std::chrono;

/**
 * @class Timer
 * @brief This class can start and stop a timer. During the timer is running
 * it is possible to check how much time is past since start.
 */
class Timer {
public:
    /**
     * @brief Default constructor with out autoStopTime.
     */
    Timer() = default;
    /**
     * @brief Constructor which sets an auto stop timer.
     *
     * @param autoStopTime The time in ms after which the timer should stop automatically.
     */
    Timer(int autoStopTime);

    /**
     * @brief Starts the timer if it is not already running.
     *
     * If it is already running it throws an exception.
     */
    void start();

    void restart();

    /**
     * @brief Stops the timer.
     */
    void stop();

     /**
     * @brief Shows the past time since start.
     *
     *@return past time in ms.
     */
        double pastTime();

        bool isActive() ;

        bool isStopped() ;

        void setTimerResolution(TimeResolution Resolution);


    private:
        TimeResolution timeResolution = ms;
        double unprotectedPastTime();
        bool autoStopCheck();
        int autoStopTime = 0; /**<The time in ms after which the timer should stop automatically default is 100s.*/
    time_point<high_resolution_clock> startTime; /**<Represents the time point when the timer started.*/
    bool timerRunning = false; /**< Indicates whether the timer is currently running or not.*/
};

#endif //ECU_UASRACING_TIMER_HPP
