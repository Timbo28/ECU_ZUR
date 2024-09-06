/**
 * @file    Data.hpp
 * @authors Marco Rau (raumar02@students.zhaw.ch)
 *          Tim Roos  (roostim1@students.zhaw.ch)
 * @brief   Interface for CAN, Terminal and Server
 * @version v1.0.0
 * @date    2023-12-22
 * 
 * @copyright Copyright (c) 2023 Zurich UAS Racing
 * 
 */

#ifndef DATA_HPP
#define DATA_HPP

#include "data/database.hpp"
#include "data/CAN.hpp"
#include "data/Terminal.hpp"
#include "data/CanStructMapper.hpp"
#include "data/DataMapper.hpp"

#include "data/server/Datalogger.hpp"
#include "data/server/Driverless.hpp"
#include "data/server/Diagnostic.hpp"
#include "data/server/Display.hpp"

#include "main.hpp"




class Data {

// VARIABLES ####################################################

private:
    // CLASS ---------------------------------------------------
    std::shared_ptr<CanStructMapper> canStructMapper;
    std::shared_ptr<DataMapper> dataMapper;
    // THREADS -------------------------------------------------
    int numberOfCan;
    std::array<std::unique_ptr<CAN>, 4> can;
    std::unique_ptr<Terminal> terminal;

    // server threads (optional)
    std::unique_ptr<Datalogger> datalogger;
    std::unique_ptr<Driverless> driverless;
    std::unique_ptr<Diagnostic> diagnostic;
    std::unique_ptr<Display> display;


    // FLAGS ---------------------------------------------------
    bool flagServerActive = false;

    // SYSTEM DATA ---------------------------------------------
    sensor_data_t sensorData;   // Data for System
    

    // INPUT DATA ----------------------------------------------
    settings_t   settings;
    driverless_commands_t driverlessCommands;



// CONSTRUCTORS AND METHODS #####################################

public:

    // CONSTRUCTOR ---------------------------------------------

    /**
     * @brief Constructs a Data object.
     * @param port The port to open the CAN connection.
     */
    Data();


    // DATA ----------------------------------------------------

    /**
     * @brief Updates the sensor data by reading from the CAN bus.
     */
    void update(void);
    
    // CAN -----------------------------------------------------
    void CANTransmit(void);
    void CANTransmit(std::string frameName );
    // GETTER --------------------------------------------------

    /**
     * @brief Retrieves the sensor data.
     * @return A pointer to the sensor data structure.
     */
    sensor_data_t* getSensorData(void);


    /**
     * @brief Retrieves the terminal data.
     * 
     * @return A pointer to the terminal data structure.
     */
    settings_t* getSettings(void);

    /**
     * @brief Retrieves the driverless commands.
     * 
     * @return A pointer to the driverless commands structure.
     */ 
    driverless_commands_t* getDriverlessCommands(void);

    // Mapping -------------------------------------------------
    template <typename T>
    void registerCanMappingStruct(T& structToRegister){
        canStructMapper->registerStruct(structToRegister);
    }

    template <typename T>
    void registerDataMappingStruct(T& structToRegister,bool reaydOnly = false){
        dataMapper->registerStruct(structToRegister,reaydOnly);
    }



private:

    void initSettings(void);

    // CAN -----------------------------------------------------



    // Server --------------------------------------------------

    template <typename Application>
    /**
     * @brief Sets the initial value of the server
     * 
     * @param server    server application
     * @param terminal  where to safe initState
     * @param initState should application be on or off?
     */
    void serverInit(const Application& server, bool& terminal, int port);

    template <typename Application>
    /**
     * @brief Activates or Deactivates a server application
     * 
     * @tparam Application 
     * @param server    server application
     * @param actual    state of the application
     * @param terminal  should application be on or off?
     */
    void serverConfig(const Application& server, bool& actual, bool& terminal, int port);

    
};

#endif // DATA_HPP