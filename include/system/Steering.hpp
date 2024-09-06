/**
 * @file	Steering.hpp
 * @authors	Marco Rau (raumar02@students.zhaw.ch)
 *          Tim Roos  (roostim1@students.zhaw.ch)
 * @brief 	Used to control the steering wheel for DV
 * @version	v1.0.0
 * @date	2024-06-26
 *
 * @copyright Copyright (c) 2024 Zurich UAS Racing
 *
 */
#ifndef ECU_UASRACING_STEERING_HPP
#define ECU_UASRACING_STEERING_HPP

#include "data/database.hpp"
#include "Timer.hpp"
#include "system/State.hpp"
#include "system/SteeringStates.hpp"



class Steering {
public:
    Steering(const sensor_data_t* sensorData, const driverless_commands_t* dvCommands, system_data_t* systemData);

    void update(int updateRate = 0);

    
    /**
    * @brief Sets a new State.
     *
     * @param newState new state.
    */
    void setState(State<Steering>& newState);

     /**
     * @brief Get the current state of the fsm.
     *
     * @return A pointer to the current state.
     */
    State<Steering>* getCurrentState() const;

    CAN_data_out_t& getCanOut(void);

    const driverless_commands_t& getDriverlessCommands(void);

    const bool& DVIsOn(void);

    const steering_t& getSteeringData(void);

    const bool& scdIsPressed(void);

private:

    State<Steering>* currentState;      /**< Pointer to the current state. can only be changed with the setState() methode.*/

    const bool& scdPressed;
    const bool& dvOn;

    const steering_t& steeringData;

    const driverless_commands_t& dvCommands;
    
    CAN_data_out_t& canOut;

    Timer updateTimer;
};

#endif //ECU_UASRACING_STEERING_HPP
