/**
 * @file	Server.hpp
 * @authors	Marco Rau (raumar02@students.zhaw.ch)
 * 		    Tim Roos  (roostim1@students.zhaw.ch)
 * @brief 	
 * @version	v1.0.0
 * @date	2024-02-24
 *
 * @copyright Copyright (c) 2024 Zurich UAS Racing
 *
 */

#ifndef UDP_HPP
#define UDP_HPP

#include <netinet/in.h>
#include <thread>

#include "data/database.hpp"
#include "main.hpp"

/**
 * @class UDP
 * @brief Base class for Datalogger, Driverless and Diagnostic.
 * 
 * It consists of some basic tools for a UDP server
 *
 */
class UDP
{

// VARIABLES ####################################################

private:

    const char* name;

    // THREAD ---------------------------------------------------
    std::thread thread;
    bool threadFlag;

    // DATA -----------------------------------------------------
    char* inBuffer;
    char* outBuffer;
    int size;

    // CONNECTION -----------------------------------------------
    sockaddr_in serverAddress;
    sockaddr_in clientAddress;

    socklen_t clientLength;

    // SOCKET ---------------------------------------------------
    int serverSocket;
    int port;

    int clientSocket;

    const char* clientIP;
    int clientPort;

// CONSTRUCTORS AND METHODS #####################################

public:
   
    explicit UDP(const char* name, char *inBuffer,char *outBuffer, int size);
    ~UDP();

    /**
     * @brief Thread that runs in a loop
     * 
     */
    virtual void application(void){}


    /**
     * @brief Configures socket and starts thread
     * 
     * @return true  configure was successful
     * @return false problem with the configuration
     */
    bool startApplication(int port);

    /**
     * @brief Stops thread and closes socket
     * 
     */
    void stopApplication(void);

    /**
     * @brief  Writes data into buffer
     * 
     * @return true  data was read
     * @return false data wasn't available
     */
    bool receive(void);

    /**
     * @brief Sends buffer data to client.
     * 
     * @param size 
     */
    void transmit(const std::string& message);

    void test(int size);

private:

    /**
     * @brief will run application() in a loop
     * 
     */
    void loop(void);
};

#endif // UDP_HPP
