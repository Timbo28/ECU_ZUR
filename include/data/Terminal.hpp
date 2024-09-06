/**
 * @file	Terminal.hpp
 * @authors	Marco Rau (raumar02@students.zhaw.ch)
 * 		    Tim Roos  (roostim1@students.zhaw.ch)
 * @brief 	Provides terminal commands which can be typed in while the program is running.
 * @version	v1.0.0
 * @date	2023-12-22
 *
 * @copyright Copyright (c) 2023 Zurich UAS Racing
 *
 */

#ifndef ECU_UASRACING_TERMINAL_HPP
#define ECU_UASRACING_TERMINAL_HPP

#include <vector>
#include <thread>
#include "data/database.hpp"
#include <termios.h>
#include "main.hpp"

//Macros for console commands---------------------------------------------
#define NOTHING         ""                      /**< Represents an empty string. */
#define SHUTDOWN        "shutdown"              /**< Represents the command to shut down the ECU. */
#define SERVER          "server"                /**< Represents the command for managing server. */
#define VEHICLE_STATUS  "vehiclestatus"         /**< Represents the command for managing vehicle status. */
#define LOG_LEVEL       "loglevel"              /**< Represents the command for managing log levels. */
#define MOTOR_ENABLE    "moto"                  /**< Represents the command for managing motor enable. */
#define MOTOR_LOAD      "motoload"              /**< Represents the command for managing motor load per motor */
#define MOTOR_TEMP      "mototmp"           
#define INVERTER_TEMP   "invtmp"
#define ACCU_TEMP       "accutmp"
#define ACCU_CHARGE     "accucrg"
#define MAX_TORQUE      "maxtqr"             /**< Represents the command for managing max torque. */
#define PEDAL_RESPONSIVENESS "peres"             /**< Represents the command for managing pedel responsiveness. */
#define MAX_POWER       "maxpow"                /**< Represents the command for managing max power. */
#define POWER_LIMIT_KP  "plkp"                  /**< Represents the command for managing power limit kp. */
#define POWER_LIMIT_KI  "plki"                  /**< Represents the command for managing power limit ki. */


/**
 * @class Terminal
 * @brief Provides functionalities to interact with the terminal for input handling.
 * 
 * Commands can only be entered, when the ECU is running.
 * 
 * <b style="color: red;">!!ATTENTION!!</b> The console sink is switched off while the commands are being typed in.
 * 
 * List of possible commands: 
 * \n 
 * - <b style="color: #0380ac;"> loglevel console get </b> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;//"Returns the log level of the console sink."\n 
 * - <b style="color: #0380ac;">loglevel file get </b>     &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;//"Returns the log level of the file sink."\n 
 * - <b style="color: #0380ac;">loglevel console [off/trace/debug/info/warn/error/critical] </b> &nbsp; //"Sets the log level of the console sink to one of the listed levels."\n 
 * - <b style="color: #0380ac;">loglevel file [off/trace/debug/info/warn/error/critical] </b> &nbsp;  //"Sets the log level of the file sink to one of the listed levels."\n 
 * - <b style="color: #0380ac;">vehiclestatus off  </b> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;//"Sets the vehicleStatus in the sensorData struct to false."\n
 * - <b style="color: #0380ac;">vehiclestatus on </b>   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; //"Sets the vehicleStatus in the sensorData struct to true."\n
 * - <b style="color: #0380ac;">vehiclestatus reset </b> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; //"Sets the vehicleStatus in the sensorData struct to false and than agan to true, to reset the system by software."\n
 * - <b style="color: #0380ac;">shutdown </b>           &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; //"Switches off the ECU."\n
 * \n
 * 
 * \note if you want to add a new Terminal command, simply add a new command Macro and extende the if else statement in the runTerminal() methode.
 *   
 */
class Terminal {

// VARIABLES ####################################################

private:
    settings_t& settings; /**<Reference to terminal commands structure.*/
    sensor_data_t& sensorData;/**<Reference to sensor data structure.*/

    struct termios originalTermios, newTermios;/**<Terminal settings structures.*/

    std::thread terminalThread;/**<Terminal processing thread.*/
    bool isRunning;/**<Flag indicating if the terminal is running.*/

// CONSTRUCTORS AND METHODS #####################################

public:
    /**
     * @brief Constructor to initialize Terminal instance.
     * 
     * This constructor starts a thread with the method runTerminal(), which runs in a loop until the programme is stopped.
     * 
     * @param settingsRef Reference to terminal commands structure.
     * @param sensorDataRef       Reference to sensor data structure.
     */
    Terminal(settings_t& settingsRef, sensor_data_t& sensorDataRef);
    /**
     * @brief Destructor for Terminal.
     * 
     * Joins the thread when the programm is stoped.
     */
    ~Terminal();

private:
    /**
     * @brief Starts and manages the terminal for input handling.
     */
    void run(void);

    /**
     * @brief Reads user input from the terminal.
     * 
     * @param command Reference to store the command entered by the user.
     * @param args    Reference to store command arguments entered by the user.
     */
    void readInput(std::string& command, std::vector<std::string>& args);

    /**
     * @brief Deactivates line buffering for terminal input.
     */
    void deactivateLineBuffer(void);

    /**
     * @brief Activates line buffering for terminal input.
     */
    void activateLineBuffer(void);

    /**
     * @brief Executes the 'shutdown' command.
     */
    void commandSHUTDOWN(void);

    void commandSERVER(std::vector<std::string>& args);

    /**
     * @brief Executes the 'loglevel' command.
     * 
     * @param args Vector containing command arguments.
     */
    void commandLOG_LEVEL(std::vector<std::string>& args);

    /**
     * @brief Executes the 'vehiclestatus' command.
     * 
     * @param args Vector containing command arguments.
     */
    void commandVEHICLESTATUS(std::vector<std::string>& args);

    void commandMOTOR_ENABLE(std::vector<std::string>& args);

    void commandMAX_TORQUE(std::vector<std::string>& args);

    void commandPEDAL_RESPONSIVENESS(std::vector<std::string>& args);

    void commandMOTOR_TEMP(std::vector<std::string>& args);

    void commandINVERTER_TEMP(std::vector<std::string>& args);

    void commandACCU_TEMP(std::vector<std::string>& args);

    void commandACCU_CHARGE(std::vector<std::string>& args);

    void commandMOTOR_LOAD(std::vector<std::string>& args);

    void commandMAX_POWER(std::vector<std::string>& args);

    void commandPOWER_LIMIT_KP(std::vector<std::string>& args);

    void commandPOWER_LIMIT_KI(std::vector<std::string>& args);

    std::string currentLevel;
};

#endif //ECU_UASRACING_TERMINAL_HPP
