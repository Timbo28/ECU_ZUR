/**
 * @file	PedalBoxStates.hpp
 * @authors	Marco Rau (raumar02@students.zhaw.ch)
 * 		    Tim Roos  (roostim1@students.zhaw.ch)
 * @brief 	State classes for Steering Finite State Machine.
 * @version	v1.0.0
 * @date	2023-12-22
 *
 * @copyright Copyright (c) 2023 Zurich UAS Racing
 *
 */

#ifndef ECU_UASRACING_STEERINGSTATES_HPP
#define ECU_UASRACING_STEERINGSTATES_HPP

#include "system/Steering.hpp"

class Steering;
//SteeringOff ##################################################################
/**
 * @class SteeringOff
 * @brief State when the steering is off.
 * 
 * This class defines the behavior and properties of the steering when it is off.
 */
class SteeringOff : public State<Steering> {
public:
    /**
     * @brief Action to be performed in this state.
     * 
     * @param steering The steering object. 
     */
    void stateAction(Steering* steering) override;

    /** 
     * @brief Update the state.
     * 
     * @param steering The steering object. 
     */
    void updateState(Steering* steering) override;

    /** 
     * @brief Enter the state.
     * 
     * @param steering The steering object. 
     */
    void enterState(Steering* steering) override;

    /** 
     * @brief Exit the state.
     * 
     * @param steering The steering object. 
     */
    void exitState(Steering* steering) override;

    /** 
     * @brief Get the state as a string.
     * 
     * @return The state as a string. 
     */
    std::string toString() override;

    /**
     * @brief Get the instance of the state.
     * 
     * @return The instance of the state. 
     */
    static State& getInstance();

private:
    //Prevents the usage and instantiation of this class
    // by making the default constructor, copy constructor, and copy assignment operator private.
    /**
     * @brief Deleted default constructor to prevent instantiation of SteeringOff.
     */
    SteeringOff() = default;

    /**
     * @brief Deleted copy constructor to prevent copying instances of SteeringOff.
     */
    SteeringOff(const SteeringOff&) = delete;
    
    /**
     * @brief Deleted copy assignment operator to prevent assignment of SteeringOff instances.
     */
    SteeringOff& operator=(const SteeringOff&) = delete;

};

//SteeringOn ##################################################################
/**
 * @class SteeringOn
 * @brief State when the steering is on.
 * 
 * This class defines the behavior and properties of the steering when it is on.
 */
class SteeringOn : public State<Steering> {
public:
    /**
     * @brief Action to be performed in this state.
     * 
     * @param steering The steering object. 
     */
    void stateAction(Steering* steering) override;

    /** 
     * @brief Update the state.
     * 
     * @param steering The steering object. 
     */
    void updateState(Steering* steering) override;

    /** 
     * @brief Enter the state.
     * 
     * @param steering The steering object. 
     */
    void enterState(Steering* steering) override;

    /** 
     * @brief Exit the state.
     * 
     * @param steering The steering object. 
     */
    void exitState(Steering* steering) override;

    /** 
     * @brief Get the state as a string.
     * 
     * @return The state as a string. 
     */
    std::string toString() override;

    /**
     * @brief Get the instance of the state.
     * 
     * @return The instance of the state. 
     */
    static State& getInstance();

private:
    Timer errorTimer;

    float lastSteeringPosition = 0;

private:
    //Prevents the usage and instantiation of this class
    // by making the default constructor, copy constructor, and copy assignment operator private.
    /**
     * @brief Deleted default constructor to prevent instantiation of SteeringOn.
     */
    SteeringOn(): errorTimer(500){};// errorTimer is reset after 500ms

    /**
     * @brief Deleted copy constructor to prevent copying instances of SteeringOn.
     */
    SteeringOn(const SteeringOn&) = delete;
    
    /**
     * @brief Deleted copy assignment operator to prevent assignment of SteeringOn instances.
     */
    SteeringOn& operator=(const SteeringOn&) = delete;

};

//SteeringError ##################################################################
/**
 * @class SteeringError
 * @brief State when the steering has an error.
 * 
 * This class defines the behavior and properties of the steering when it has an error.
 */
class SteeringError : public State<Steering> {
public:
    /**
     * @brief Action to be performed in this state.
     * 
     * @param steering The steering object. 
     */
    void stateAction(Steering* steering) override;

    /** 
     * @brief Update the state.
     * 
     * @param steering The steering object. 
     */
    void updateState(Steering* steering) override;

    /** 
     * @brief Enter the state.
     * 
     * @param steering The steering object. 
     */
    void enterState(Steering* steering) override;

    /** 
     * @brief Exit the state.
     * 
     * @param steering The steering object. 
     */
    void exitState(Steering* steering) override;

    /** 
     * @brief Get the state as a string.
     * 
     * @return The state as a string. 
     */
    std::string toString() override;

    /**
     * @brief Get the instance of the state.
     * 
     * @return The instance of the state. 
     */
    static State& getInstance();

private:
    //Prevents the usage and instantiation of this class
    // by making the default constructor, copy constructor, and copy assignment operator private.
    /**
     * @brief Deleted default constructor to prevent instantiation of SteeringError.
     */
    SteeringError() = default;

    /**
     * @brief Deleted copy constructor to prevent copying instances of SteeringError.
     */
    SteeringError(const SteeringError&) = delete;
    
    /**
     * @brief Deleted copy assignment operator to prevent assignment of SteeringError instances.
     */
    SteeringError& operator=(const SteeringError&) = delete;

};

#endif //ECU_UASRACING_STEERINGSTATES_HPP