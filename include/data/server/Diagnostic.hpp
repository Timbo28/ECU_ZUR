/**
 * @file	Diagnostic.hpp
 * @authors	Marco Rau (raumar02@students.zhaw.ch)
 * 		    Tim Roos  (roostim1@students.zhaw.ch)
 * @brief 	
 * @version	v1.0.0
 * @date	2024-02-24
 *
 * @copyright Copyright (c) 2024 Zurich UAS Racing
 *
 */

#ifndef DIAGNOSTIC_HPP
#define DIAGNOSTIC_HPP

#include <iostream>
#include <map>
#include <unordered_map>
#include <string>
#include <fstream>
#include <bitset>

#include <linux/can.h>
#include "nlohmann/json.hpp"


#include "data/database.hpp"
#include "data/server/UDP.hpp"
#include "main.hpp"

// PARAMETERS #################################################################

#define CLIENT_IP   NO_IP
#define CLIENT_PORT NO_PORT

#define DIAGNOSTIC_BUFFER_SIZE 1024

#define TIME_OUT 10 //ms

using namespace std;

class Diagnostic : UDP
{

// VARIABLES ####################################################

private:

    // shared data ---------------------------------------------
    settings_t* settings;
    mutex mtx;

    // UDP -----------------------------------------------------
    char inBuffer[DIAGNOSTIC_BUFFER_SIZE];
    char outBuffer[DIAGNOSTIC_BUFFER_SIZE];

// CONSTRUCTORS AND METHODS #####################################

public:
   
    Diagnostic(settings_t* settings);
    ~Diagnostic();

    using UDP::startApplication;
    using UDP::stopApplication;

private:

    /**
     * @brief Establishes a communication with the diagnostic tool
     * 
     */
    void application() override;

    void overrideControl();
    void sendLogs();

    void sendSettings();
    void saveSettings();

    void loadLogs(string* message, int logs);

    void loadConfig(string* message);
    void saveConfig();

    int  readBufferNumber();
};

#endif // DIAGNOSTIC_HPP
