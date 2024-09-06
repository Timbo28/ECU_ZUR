/**
 * @file	MotorController.hpp
 * @authors	Marco Rau (raumar02@students.zhaw.ch)
 * 		    Tim Roos  (roostim1@students.zhaw.ch)
 * @brief 	Motor controller unit (MotorController).
 * @version	v1.0.0
 * @date	2023-12-22
 *
 * @copyright Copyright (c) 2023 Zurich UAS Racing
 * 
 */
#ifndef MOTORCONTROLLER_HPP
#define MOTORCONTROLLER_HPP

#include <iostream>

#include "Data.hpp"
#include "data/database.hpp"
#include "system/State.hpp"
#include "Timer.hpp"

/**
 * @brief The MotorController class represents the motor controller component of the system.
 * 
 * This class is responsible for managing the motorcontroller state machine.
 */
class MotorController {
public:
    /**
     * @brief Constructs a new MotorController object.
     * 
     * @param sensorData Pointer to the sensor data.
     * @param systemData Pointer to the system data.
     * @param terminalCommands Pointer to the terminal commands.
     */
    MotorController(const sensor_data_t* sensorData, system_data_t* systemData, settings_t* settings);

    /**
     * @brief Updates the motor controller.
     * 
     * @param updateRate The update rate in milliseconds.
     */
    void update(int updateRate = 0);

    /**
     * @brief Sets the current state of the motor controller.
     * 
     * @param newState The new state to set.
     */
    void setState(State<MotorController>& newState);

    /**
     * @brief Gets the current state of the motor controller.
     * 
     * @return The current state of the motor controller.
     */
    State<MotorController>* getCurrentState() const;

    /**
     * @brief Gets the old state of the motor controller.
     *  Old state means the state before the current state.
     * 
     * @return The old state of the motor controller.
     */
    State<MotorController>* getOldState() const;

    /**
     * @brief Gets the CAN output data.
     * 
     * @return The CAN output data.
     */
    CAN_data_out_t* getCANOutput();

    /**
     * @brief Gets the motor controller data.
     * 
     * @return The motor controller data.
     */
    motor_controller_box_t* getMotorControllerData();

    /**
     * @brief Gets the LED button data.
     * 
     * @return The LED button data.
     */
    const led_button_box_t* getLedButtonData();

    /**
     * @brief Gets the pedal data.
     * 
     * @return The pedal data.
     */
    const pedal_box_t* getPedalData();

    /**
     * @brief Gets the sensor data.
     * 
     * @return The sensor data.
     */
    const sensor_data_t* getSensorData() const;

    /**
     * @brief Gets the inverter data.
     * 
     * @return The inverter data.
     */
    const inverter_t* getInverterData() const;

    /**
     * @brief Gets the accumulator data.
     * 
     * @return The accumulator data.
     */
    const accu_t* getAccuData() const;

    /**
     * @brief Gets the accumulator management data.
     * 
     * @return The accumulator management data.
     */
    const accu_management_box_t* getAccuManagementData() const;

    /**
     * @brief Gets the terminal commands.
     * 
     * @return The terminal commands.
     */
    const settings_t* getSettings() const;

private:
    State<MotorController>* currentState; /**< The current state of the motor controller. */

    Timer updateTimer; /**< The timer used for updating the motor controller. */

    CAN_data_out_t* CANOutput; /**< The CAN output data. */
    motor_controller_box_t* motorControllerData; /**< The motor controller data. */
    const led_button_box_t* ledButtonData; /**< The LED button data. */
    const pedal_box_t* pedalData; /**< The pedal data. */

    const sensor_data_t* sensorData; /**< The sensor data. */
    const inverter_t* inverterData; /**< The inverter data. */
    const accu_t* accuData; /**< The accumulator data. */
    const accu_management_box_t* accuManagementBox; /**< The accumulator management data. */

    const settings_t* settings; /**< The terminal commands. */

    const std::string Positions[4] = {"front left", "front right", "rear left", "rear right"}; /**< The positions of the motors on the car. */
    std::string oldStateName; /**< The name of the old state. */

    
    /**
     * @brief Checks for overheat condition.
     */
    void overheatCheck();

    /**
     * @brief Checks for inverter error.
     */
    void inverterErrorCheck();

    /**
     * @brief Saves the old state of the motor controller.
     * 
     * @param oldState Pointer to the old state.
     */
    void saveOldState(State<MotorController> *oldState);

    /**
     * @brief Checks for discharge condition.
     */
    void dischargeCheck();
};

#endif //MOTORCONTROLLER_HPP
