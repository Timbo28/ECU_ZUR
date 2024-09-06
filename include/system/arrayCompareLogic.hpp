//
// Created by timro on 17/02/2024.
//

#ifndef MOTORCONTROLLER_ARRAYCOMPARELOGIC_HPP
#define MOTORCONTROLLER_ARRAYCOMPARELOGIC_HPP
#include <vector>
#include <iostream>


void checkStructArrayOrder();

template <typename T>
T* asArray(T* firstPosition){
    return (T*)firstPosition;
}
/**
 * @brief Checks if all elements in the array are true and are masked
 *
 * @param array Array of boolean values
 * @param mask Array of boolean values to mask the array
 * @param size Size of the array
 * @return true if all elements are true
 * @return false if at least one element is false
 */
bool allElementsTrueM(const bool *array,  const bool* mask,int size=4);
/**
 * @brief Checks if all elements in the array are true
 *
 * @param array Array of boolean values
 * @return true if all elements are true
 * @return false if at least one element is false
 */
bool allElementsTrue(const bool *array, int size=4 );

/**
 * @brief Checks if all elements in the array are false and are masked
 *
 * @param array Array of boolean values
 * @param mask Array of boolean values to mask the array
 * @param size Size of the array
 * @return true if all elements are false
 * @return false if at least one element is true
 */
bool allElementsFalseM(const bool *array,  const bool* mask,int size=4);
/**
 * @brief Checks if all elements in the array are false
 *
 * @param array Array of boolean values
 * @return true if all elements are false
 * @return false if at least one element is true
 */
bool allElementsFalse(const bool* array, int size=4);

/**
 * @brief Checks if all elements in the array are greater than or equal to the value and are masked
 *
 * @param firstElement Pointer to the first element of the array
 * @param value Value to compare the elements to
 * @param mask Array of boolean values to mask the array
 * @param size Size of the array
 * @return true if all elements are greater than or equal to the value
 * @return false if at least one element is less than the value
 */
bool allElementsGEM(const int16_t* firstElement, int value, const bool* mask, int size=4);

/**
 * @brief Checks if all elements in the array are greater than or equal to the value
 *
 * @param firstElement Pointer to the first element of the array
 * @param value Value to compare the elements to
 * @param size Size of the array
 * @return true if all elements are greater than or equal to the value
 * @return false if at least one element is less than the value
 */
bool allElementsGE(const int16_t* firstElement, int value, int size=4);



/**
 * @brief Returns the positions of the true elements in the array that are masked
 *
 * @param array Array of boolean values
 * @param mask Array of boolean values to mask the array
 * @param size Size of the array
 * @return std::vector<int> Vector of the positions of the true elements
 */
std::vector<int> getTrueElementPositionsM(const bool* array, const bool* mask, int size=4);
/**
 * @brief Returns the positions of the true elements in the array
 *
 * @param array Array of boolean values
 * @param size Size of the array
 * @return std::vector<int> Vector of the positions of the true elements
 */
std::vector<int> getTrueElementPositions(const bool* array, int size=4);


/**
 * @brief Returns the positions of the false elements in the array that are masked
 *
 * @param array Array of boolean values
 * @param mask Array of boolean values to mask the array
 * @param size Size of the array
 * @return std::vector<int> Vector of the positions of the false elements
 */
std::vector<int> getFalseElementPositionsM(const bool* array,const bool* mask, int size=4);
/**
 * @brief Returns the positions of the false elements in the array
 *
 * @param array Array of boolean values
 * @return std::vector<int> Vector of the positions of the false elements
 */
std::vector<int> getFalseElementPositions(const bool* array, int size=4);

#endif //MOTORCONTROLLER_ARRAYCOMPARELOGIC_HPP
