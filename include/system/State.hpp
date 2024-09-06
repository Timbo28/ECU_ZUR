/**
 * @file	State.hpp
 * @authors	Marco Rau (raumar02@students.zhaw.ch)
 * 		Tim Roos  (roostim1@students.zhaw.ch)
 * @brief 	Interface and base class for the states in the finite state machine.
 * @version	v1.0.0
 * @date	2023-12-22
 *
 * @copyright Copyright (c) 2023 Zurich UAS Racing
 *
 */

#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H


/**
 * @class State
 * @brief Interface and base class for states in the finite state machine.
 */
template <class T>
class State {
public:
    /**
     * @brief Enter the state.
     *
     * This pure virtual function is to be implemented by derived states to define their behavior when entering the state.
     *
     * @param systemControlClass A pointer to the systemControlClass object.
     */
    virtual void enterState(T *systemControlClass) = 0;

    /**
     * @brief Exit the state.
     *
     * This pure virtual function is to be implemented by derived states to define their behavior when exiting the state.
     *
     * @param systemControlClass A pointer to the systemControlClass object.
     */
    virtual void exitState(T *systemControlClass) = 0;

    /**
    * @brief Perform the state-specific action.
    *
    * This pure virtual function is to be implemented by derived states to define their behavior.
    *
    * @param systemControlClass A pointer to the systemControlClass object.
    */
    virtual void stateAction(T *systemControlClass) = 0;

    /**
     * @brief Update the state to other states.
     *
     * This pure virtual function is to be implemented to change state according to different criteria which should bi defined in this methode.
     *
     * @param systemControlClass A pointer to the systemControlClass object.
     */
    virtual void updateState(T *systemControlClass) = 0;

    /**
     * @brief Convert the state to a string representation.
     *
     * This pure virtual function converts the state to a string representation for debugging and logging purposes.
     *
     * @return A string representation of the state.
     */
    virtual std::string toString() = 0;

};



#endif //SYSTEM_STATE_H
