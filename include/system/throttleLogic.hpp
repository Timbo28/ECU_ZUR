/**
 * @file	throttleLogic.hpp
 * @authors	Marco Rau (raumar02@students.zhaw.ch)
 * 		    Tim Roos  (roostim1@students.zhaw.ch)
 * @brief 	Throttle calculation and error values checking functions.
 * @version	v1.0.0
 * @date	2023-12-22
 *
 * @copyright Copyright (c) 2023 Zurich UAS Racing
 *
 */
#ifndef ECU_UASRACING_THROTTLELOGIC_HPP
#define ECU_UASRACING_THROTTLELOGIC_HPP

#include "system/PedalBox.hpp"
#include "EcuException.hpp"


/**
 * @brief Calculate the average throttle position.
 *
 * This function calculates the average throttle position based on the left and right
 * throttle values, accounting for the sensor range and system constraints. It also checks
 * for out-of-range conditions and system power limits.
 *
 * IMPORTANT!: Function needs to be in a try catch block.
 *
 * @param pedalBox A pointer to the pedalBox object.
 *
 * @return The calculated throttle value.
 *
 * @throw std::out_of_range Thrown if the throttle percentages are out of range.
 * @throw std::runtime_error Thrown for if the deviation is outside the allowed range.
 */
uint16_t calculateThrottle(PedalBox *pedalBox);

/**
 * @brief Check if there is any shortage between the lines.
 *
 * This function check if there is any shortage between the lines, whit the pedal position data.
 * It calls the individual check functions and throws runtime errors when any issue is detected.
 *
 * IMPORTANT!: Function needs to be in a try catch block.
 *
 * @param pedalBox A pointer to the pedalBox object.
 *
 * @throw std::runtime_error Thrown if any shortage  is detected.

 */
void checkForShortage(PedalBox *pedalBox);


#endif //ECU_UASRACING_THROTTLELOGIC_HPP
