/**
 * @file	MProfiling.hpp
 * @authors	Marco Rau (raumar02@students.zhaw.ch)
 * 		    Tim Roos  (roostim1@students.zhaw.ch)
 * @brief 	Macro functions for recordes the profiling data
 * @version	v1.0.0
 * @date	2023-12-22
 *
 * @copyright Copyright (c) 2023 Zurich UAS Racing
 *
 */
#ifndef PROFILING_H
#define PROFILING_H
//#define PROFILING_ON 1
#include "main.hpp"

#ifdef PROFILING_ON
    #include <iostream>
    #include "ProfilingManager.hpp"
    #include "ProfilingTimer.hpp"


    /**
     * @brief Begins a profiling session.
     * @param sessionName Name of the profiling session.
     */
    #define PROFILING_BEGIN_SESSION(sessionName) Profiling::Get().BeginSession(sessionName)
    /**
     * @brief Ends the current profiling session.
     */
    #define PROFILING_END_SESSION() Profiling::Get().EndSession()
    /**
     * @brief Measures the duration of a code block.
     * @param name Name of the code block.
     */
    #define PROFILING_SCOPE(name) ProfilingTimer timer##__LINE__(name)
    /**
     * @brief Measures the duration of a function.
     */
    #define PROFILING_FUNCTION() PROFILING_SCOPE(__PRETTY_FUNCTION__)

#else
    /**
     * @brief Dummy macro if profiling is turned off.
     */
    #define PROFILING_BEGIN_SESSION(sessionName) (void)0

    /**
     * @brief Dummy macro if profiling is turned off.
     */
    #define PROFILING_END_SESSION() (void)0

    /**
     * @brief Dummy macro if profiling is turned off.
     */
    #define PROFILING_SCOPE(name) (void)0

    /**
     * @brief Dummy macro if profiling is turned off.
     */
    #define PROFILING_FUNCTION() (void)0

#endif //PROFILING

#endif //PROFILING_H