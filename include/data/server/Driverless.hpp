/**
 * @file	Driverless.hpp
 * @authors	Marco Rau (raumar02@students.zhaw.ch)
 * 		    Tim Roos  (roostim1@students.zhaw.ch)
 * @brief 	
 * @version	v1.0.0
 * @date	2024-02-24
 *
 * @copyright Copyright (c) 2024 Zurich UAS Racing
 *
 */

#ifndef DRIVERLESS_HPP
#define DRIVERLESS_HPP

#include "data/database.hpp"
#include "data/server/UDP.hpp"
#include "data/DataMapper.hpp"
#include "main.hpp"

// PARAMETERS #################################################################

#define CLIENT_IP   NO_IP
#define CLIENT_PORT NO_PORT

#define BUFFER_SIZE 1024

#define TIME_OUT 10 //ms

class Driverless : UDP
{

// VARIABLES ####################################################

private:

    // DATA -----------------------------------------------------
    char inBuffer[BUFFER_SIZE];
    char outBuffer[BUFFER_SIZE];
    int size = BUFFER_SIZE;

    std::shared_ptr<DataMapper> dataMapper;
// CONSTRUCTORS AND METHODS #####################################

public:
   
    Driverless(std::shared_ptr<DataMapper> dataMapper);
    ~Driverless();

    using UDP::startApplication;
    using UDP::stopApplication;

private:

    /**
     * @brief Receives data and converts it to store it
     * 
     * @note for more information look at convert()
     */
    void application() override;


};

#endif // DRIVERLESS_HPP
