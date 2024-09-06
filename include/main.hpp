#ifndef MAIN_HPP
#define MAIN_HPP
#include "Profiling.hpp"
#include "Error.hpp"
#include "log.hpp"

// Turns on additional debug information printouts on the console
//#define DEBUG 1

// Turns on the profiling of the program
//#define PROFILING_ON 1

// Turns off the logging
//#define LOGGER_OFF 1

// TIMINGS ---------------------------------------------------
#define ECU_CYCLE_TIME 15 /**< The cycle time of the ECU in ms. */
#define CAN_CYCLE_TIME 10 /**< The cycle time of the CAN communication in ms. */

#endif // MAIN_HPP