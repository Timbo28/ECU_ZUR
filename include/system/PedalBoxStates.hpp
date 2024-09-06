/**
 * @file	PedalBoxStates.hpp
 * @authors	Marco Rau (raumar02@students.zhaw.ch)
 * 		    Tim Roos  (roostim1@students.zhaw.ch)
 * @brief 	State classes for PedalBox Finite State Machine.
 * @version	v1.0.0
 * @date	2023-12-22
 *
 * @copyright Copyright (c) 2023 Zurich UAS Racing
 *
 */
#ifndef SYSTEM_PEDALBOXSTATES_H
#define SYSTEM_PEDALBOXSTATES_H

#include <iostream>
#include "system/State.hpp"
#include "system/PedalBox.hpp"
#include "Timer.hpp"

// Macros----------------------------------------------------
#define STATUS_ON true /**< @ref vehicleStatus macro*/
#define STATUS_OFF false/**< @ref vehicleStatus macro*/
#define APPS_ERROR_TIME 100 /**< 100 ms in Failure Mode until Error Mode gets activated.*/


//PedalOff ########################################################
/**
 * @class PedalOff
 * @brief Represents the PedalOff state in the finite state machine.
 *
 * <b style="color: green;"> State-Behavior: </b> In this state the throttle will be set to zero (throttle = 0). \n
 * <b style="color: orange;"> Next-State: </b> This state can only be exited if the vehicleStatus is set to on, in which case the system switches to the PedalOn state.
 * 
 * This class defines the behavior and properties of the PedalOff state within the finite state machine.
 */
class PedalOff : public State<PedalBox>{
public:
    void enterState(PedalBox *pedalBox) override;

    /**
     * @brief Resets all the error messages.
     *
     * @param pedalBox A pointer to the PedalBox object.
     */
    void stateAction(PedalBox *pedalBox) override;

    /**
     * @brief Change state to PedalOn.
     *
     * Changes to PedalOn state if the vehicle-status is on.
     *
     * @param pedalBox A pointer to the PedalBox object.
     */
    void updateState(PedalBox *pedalBox) override;


    void exitState(PedalBox *pedalBox) override;

    /**
     * @brief Convert the PedalOff state to a string representation.
     *
     * @return "PedalOff".
     */
    std::string toString() override;

    /**
     * @brief Get a reference to the PedalOff state instance
     * and holds the singleton PedalOff object.
     *
     * @return A reference to the PedalOff state instance.
     */
    static State& getInstance();

private:
    //Prevents the usage and instantiation of this class
    // by making the default constructor, copy constructor, and copy assignment operator private.
    /**
     * @brief Deleted copy constructor to prevent copying instances of PedalOff.
     */
    PedalOff(const PedalOff& other)=delete;

    /**
     * @brief Deleted copy assignment operator to prevent assignment of PedalOff instances.
     */
    PedalOff& operator=(const PedalOff& other)=delete;

    /**
     * @brief Deleted default constructor to prevent instantiation of PedalOff.
     */
    PedalOff()=default;

};

//class PedalOn ##########################################################
/**
 * @class PedalOn
 * @brief Represents the PedalOn state in the finite state machine.
 * 
 * <b style="color: green;"> State-Behavior: </b> In this state the throttle will be calculatet by the calculateThrottle() funktion (throttle = calculateThrottle()). \n
 * <b style="color: orange;"> Next-State: </b> As soon as a wrong value is detected, the system switches to the PedalFailure state.
 * Alternatively, if the vehicle status is off, the system switches to the PedalOff state.
 *
 * This class defines the behavior and properties of the PedalOn state within the finite state machine.
 */
class PedalOn : public State<PedalBox>{
public:
    void enterState(PedalBox *pedalBox) override;

    /**
     * @brief Calculates the throttle with the calculateThrottle() function and loads it in to the systemData in the System.hpp file.
     * 
     * Also checks with the checkForShortage() funktion if the pedal sensores have any shortage. 
     *
     * @param pedalBox A pointer to the PedalBox object.
     */
    void stateAction(PedalBox *pedalBox) override;

    /**
     * @brief Change state to PedalFailure or PedalOff.
     *
     * Changes to PedalFailure state if there is any not matching value from the pedal sensors,
     * or if the vehicle-status is off, it changes state to PedalOff state.
     *
     * @param pedalBox A pointer to the PedalBox object.
     */
    void updateState(PedalBox *pedalBox) override;


    void exitState(PedalBox *pedalBox) override;

    /**
     * @brief Convert the PedalOn state to a string representation.
     *
     * @return "PedalOn".
     */
    std::string toString() override;

    /**
     * @brief Get a reference to the PedalOn state instance
     * and holds the singleton PedalOn object.
     *
     * @return A reference to the PedalOn state instance.
     */
    static State& getInstance();

private:

    double throttle = 0; /**<Current throttle value. */
    double oldThrottle = 0;/**<Previous throttle value.*/
    bool failureActive = false;/**<Flag indicating if failure is active.*/

    /**
     * @brief Deleted default constructor to prevent instantiation of PedalOn.
     */
    PedalOn()=default;

    /**
     * @brief Deleted copy constructor to prevent copying instances of PedalOn.
     */
    PedalOn(const PedalOff& other)=delete;

    /**
     * @brief Deleted copy assignment operator to prevent assignment of PedalOn instances.
     */
    PedalOn& operator=(const PedalOff& other)=delete;
};

//class PedalFailure ######################################################
/**
 * @class PedalFailure
 * @brief Represents the PedalFailure state in the finite state machine.
 * 
 * <b style="color: green;"> State-Behavior: </b> In this state the throttle will not be changed (throttle = oldThrottle). \n
 * <b style="color: orange;"> Next-State: </b> If the error lasts longer than 100ms, the system switches to the PedalError state. However, if the error lasts less than 100ms, the system switches back to the PedalOn state.
 * Alternatively, if the vehicle status is off, the system switches to the PedalOff state.
 * 
 * This class defines the behavior and properties of the PedalFailure state within the finite state machine.
 */
class PedalFailure : public State<PedalBox>{
public:

    void enterState(PedalBox *pedalBox) override;
    /**
     * @brief Starts a timer to measure how long the PedalFailure state lasts.
     *
     * @param pedalBox A pointer to the PedalBox object.
     */
    void stateAction(PedalBox *pedalBox) override;

    /**
     * @brief Change state to PedalError, PedalFailure or PedalOff.
     * 
     * This methode is continiusly checking if there is still a failure value in the sensorData. This is done by calling the finctions calculateThrottle() and checkForShortage(). 
     * 
     * 
     * Changes to PedalError state if it stays more than 100 ms in the PedalFailure state.
     * If the sensor values are correct again before the 100 ms has expired, then the system switches back to the PedalOn state.
     * If the vehicle-status is not ok it changes state to PedalOff state.
     *
     * During this state there is no change of the throttle.
     *
     * @param pedalBox A pointer to the PedalBox object.
     */
    void updateState(PedalBox *pedalBox) override;

    void exitState(PedalBox *pedalBox) override;

    /**
     * @brief Convert the PedalFailure state to a string representation.
     *
     * @return "PedalFailure".
     */
    std::string toString() override;

    /**
     * @brief Get a reference to the PedalFailure state instance
     * and holds the singleton PedalFailure object.
     *
     * @return A reference to the PedalFailure state instance.
     */
    static State& getInstance();

private:
    Timer failureStateTimer;/**<Timer object to measure time in the PedalFailure state.*/

    /**
     * @brief Deleted default constructor to prevent instantiation of PedalFailure.
     */
    PedalFailure() = default;

    /**
     * @brief Deleted copy constructor to prevent copying instances of PedalFailure.
     */
    PedalFailure(const PedalOff& other)=delete;

    /**
     * @brief Deleted copy assignment operator to prevent assignment of PedalFailure instances.
     */
    PedalFailure& operator=(const PedalOff& other)=delete;
};

//class PedalError ###########################################################
/**
 * @class PedalError
 * @brief Represents the PedalError state in the finite state machine.
 * 
 * <b style="color: green;"> State-Behavior: </b> In this state the throttle will be set to zero (throttle = 0). \n
 * <b style="color: orange;"> Next-State: </b> This state can only be exited if the vehicleStatus is switched off, in which case it switches to the PedalOff state.
 *
 * This class defines the behavior and properties of the PedalError state within the finite state machine.
 */
class PedalError : public State<PedalBox>{
public:

    void enterState(PedalBox *pedalBox) override;

    /**
     * @brief Sets the throttle to 0 and writes a accelerometer pedal position sensor error (APPS error) to the system data in the System.hpp file.
     *
     * @param pedalBox A pointer to the PedalBox object.
     */
    void stateAction(PedalBox *pedalBox) override;

    /**
     * @brief Change state to PedalOff.
     *
     * If the vehicle-status is off it changes state to PedalOff state.
     *
     * @param pedalBox A pointer to the PedalBox object.
     */
    void updateState(PedalBox *pedalBox) override;

    void exitState(PedalBox *pedalBox) override;

    /**
     * @brief Convert the PedalError state to a string representation.
     *
     * @return "PedalError".
     */
    std::string toString() override;

    /**
     * @brief Get a reference to the PedalError state instance
     * and holds the singleton PedalError object.
     *
     * @return A reference to the PedalError state instance.
     */
    static State& getInstance();

private:
    /**
     * @brief Deleted default constructor to prevent instantiation of PedalError.
     */
    PedalError()=default;

    /**
     * @brief Deleted copy constructor to prevent copying instances of PedalError.
     */
    PedalError(const PedalOff& other)=delete;

    /**
     * @brief Deleted copy assignment operator to prevent assignment of PedalError instances.
     */
    PedalError& operator=(const PedalOff& other)=delete;
};


#endif //SYSTEM_PEDALBOXSTATES_H
