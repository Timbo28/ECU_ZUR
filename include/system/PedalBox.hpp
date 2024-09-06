/**
 * @file	System.hpp
 * @authors	Marco Rau (raumar02@students.zhaw.ch)
 * 		    Tim Roos  (roostim1@students.zhaw.ch)
 * @brief 	Pedal control unit (PedalBox).
 * @version	v1.0.0
 * @date	2023-12-22
 *
 * @copyright Copyright (c) 2023 Zurich UAS Racing
 *
 */
#ifndef SYSTEM_PEDALBOX_H
#define SYSTEM_PEDALBOX_H

#include "System.hpp"
#include "Data.hpp"
#include "system/State.hpp"
#include "data/database.hpp"
#include "Timer.hpp"



/**
 * @class PedalBox
 * @brief This class is responsible for handling the finite state machine.
 *
 * Additionally it holds the pointers to the sensor data and system date.
 *
 * \image html ../images/PedalboxFSM.png "Pedalbox Finite State Machine"
 *
 * The current status of the FMS is saved in this class. The update() method
 * continuously executes the status action and checks whether it needs to be
 * changed to a new status.
 *
 * Every state has there own State-class, which are stored in the PedalboxStates.hpp.
 *
 * \note The state-classes are singleton-classes and can only be called withe the getInstance() method.
 */
class PedalBox{
public:
    /**
     * @brief Constructor for the PedalBox class.
     *
     * This constructor initializes an instance of the PedalBox class and
     * sets the initial state for the FSM to "PedalOff".
     *
     * @param sensorData A pointer to the Sensor data in the Data.hpp file.
     * @param systemData A pointer to the system data in the System.hpp file.
     */
    PedalBox(const sensor_data_t* sensorData, system_data_t* systemData, const settings_t *settings);


    /**
     * @brief Calls the methods of the current state.
     *
     * the state action and the update state of the @ref currentState will be called here.
     */
    void update(int updateRate = 0);

    void checkBreakForErrors();

    void checkIfBreakPressed();


    /**
    * @brief Sets a new State.
     *
     * @param newState new state.
    */
    void setState(State<PedalBox>& newState);

//Getter-----------------------------------------------------------------------------
    /**
     * @brief Get the current state of the fsm.
     *
     * @return A pointer to the current state.
     */
    State<PedalBox>* getCurrentState() const;

    /**
     * @brief Get sensor data form the Data.hpp file.
     *
     * @return A pointer to the sensor data.
     */
    const sensor_data_t* getSensorData() const;

    const pedal_t* getPedalData() const;

    /**
     * @brief Get pedal box data, which is in the sensor data struct.
     *
     * @return A pointer to the pedal box data.
     */
    pedal_box_t* getPedalBoxDate();

    const accu_management_box_t* getAccuManagementData() const;

    const settings_t* getSettings() const;

private:
    State<PedalBox>* currentState;      /**< Pointer to the current state. can only be changed with the setState() methode.*/

    Timer updateTimer; /**< Timer for the update function.*/

    const sensor_data_t* sensorData; /**< Pointer to sensor data.*/
    const pedal_t* pedalData; /**< Pointer to pedal data.*/

    pedal_box_t* pedalBoxData; /**< Pointer to pedal box data.*/
    const accu_management_box_t *accuManagementData; /**< Pointer to accu management data.*/
    CAN_data_out_t* canOutput; /**< Pointer to the can output data.*/

    const settings_t* settings; /**< Pointer to the settings data.*/

};


#endif //SYSTEM_PEDALBOX_H

