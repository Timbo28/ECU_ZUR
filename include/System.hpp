/**
 * @file	System.hpp
 * @authors	Marco Rau (raumar02@students.zhaw.ch)
 * 		    Tim Roos  (roostim1@students.zhaw.ch)
 * @brief 	electronic control unit logic system
 * @version	v1.0.0
 * @date	2023-12-22
 *
 * @copyright Copyright (c) 2023 Zurich UAS Racing
 *
 */

#ifndef SYSTEM_SYSTEM_H
#define SYSTEM_SYSTEM_H

#include <memory>
#include "data/database.hpp"
#include "system/PedalBox.hpp"
#include "system/DV_PedalBox.hpp"
#include "system/MotorController.hpp"
#include "system/AccumulatorManagement.hpp"
#include "system/LedButtonBox.hpp"
#include "system/Steering.hpp"
#include "Data.hpp"
#include "main.hpp"





class PedalBox;
/**
 * @class System
 * @brief This class acts as a facade for all system control objects.
 *
 * It creates and saves the pointers to these objects.
 * Additionally, it updates all system control objects simultaneously.
 *
 * For encapsulation reasons, only the System class can access the Data class and not the other way round.
 * The System class also provides the system objects with pointers to the corresponding memory in the Data class, which they are allowed to access.
 * This ensures that no class can access memory that has nothing to do with its function.\n
 *\n
 *  \image html ../images/ECU_UML_Top.png "Top-UML from System side"
 *\n
 *  System control classes:\n
 *                       -PedalBox\n
 *                       -MotorController\n
 *                       \n
 */

class System {
public:
    /**
    * @brief Creates all the system controllers(objects)
    ** @param date pointer to the data object
    */
    System(Data* data);

    /**
     * @brief Updates all the system controllers(objects)
     */
    void update();

    // Necessary for testing
    system_data_t* getSystemData() { return &systemData; } /**< Getter for the system data.*/

private:
    Data* data; /**< Pointer to the data object.*/

    /* data structs */
    system_data_t systemData; /**< data struct which contains all the system data. this data is shared with all the system control objects. */

    /* pointer to all system control objects */
    std::unique_ptr<PedalBox> pedalBox; /**< Pointer to the pedalBox object which is created in the constructor PedalBox() uf this class. */
    std::unique_ptr<DV_PedalBox> dvPedalBox; /**< Pointer to the dvPedalBox object which is created in the constructor DV_PedalBox() uf this class. */
    std::unique_ptr<MotorController> motorController; /**< Pointer to the motorController object which is created in the constructor MotorController() uf this class. */
    std::unique_ptr<AccumulatorManagement> accumulatorManagement; /**< Pointer to the accumulatorManagement object which is created in the constructor AccumulatorManagement() uf this class. */
    std::unique_ptr<LedButtonBox> ledButtonBox; /**< Pointer to the ledButtonBox object which is created in the constructor LedButtonBox() uf this class. */
    std::unique_ptr<Steering> steering; /**< Pointer to the steering object which is created in the constructor Steering() uf this class. */
    const bool& dvOn; /**< Flag to check if the DV is active.*/

};



#endif //SYSTEM_SYSTEM_H
