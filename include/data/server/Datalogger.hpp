/**
 * @file	Datalogger.hpp
 * @authors	Marco Rau (raumar02@students.zhaw.ch)
 * 		    Tim Roos  (roostim1@students.zhaw.ch)
 * @brief 	
 * @version	v1.0.0
 * @date	2024-02-24
 *
 * @copyright Copyright (c) 2024 Zurich UAS Racing
 *
 */

#ifndef DATALOGGER_HPP
#define DATALOGGER_HPP

#include "data/database.hpp"
#include "data/server/UDP.hpp"
#include "main.hpp"

// PARAMETERS #################################################################

#define CLIENT_IP   NO_IP
#define CLIENT_PORT NO_PORT

#define BUFFER_SIZE 1024

#define TIME_OUT 10 //ms

class Datalogger : UDP
{

// VARIABLES ####################################################

private:

    // DATA -----------------------------------------------------
    char inBuffer[BUFFER_SIZE];
    char outBuffer[BUFFER_SIZE];
    int size = BUFFER_SIZE;

// CONSTRUCTORS AND METHODS #####################################

public:
   
    Datalogger();
    ~Datalogger();
    
    using UDP::startApplication;
    using UDP::stopApplication;

private:

    /**
     * @brief Sends data to the datalogger
     * 
     */
    void application() override;
};

#endif // DATALOGGER_HPP
