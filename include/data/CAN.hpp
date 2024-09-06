/**
 * @file    CAN.hpp
 * @author  Marco Rau (raumar02@students.zhaw.ch)
 * @brief   CAN-Bus
 * @version v1.0.0
 * @date    2023-12-22
 * 
 * @copyright Copyright (c) 2023 Zurich UAS Racing
 * 
 */

#ifndef CAN_HPP
#define CAN_HPP

#include <list>

#include <thread>

#include <pthread.h>
#include <sched.h>
#include <mutex>

#include "data/database.hpp"
#include "data/CanFrameBuilder.hpp"
#include "data/CanStructMapper.hpp"
#include "main.hpp"

class CAN {

// VARIABLES ####################################################

private:

    uint8_t canPortNr; /**< The port for CAN communication. */

    // THREAD --------------------------------------------------
    pthread_t canThread;
    std::mutex mtx;  /**< Mutex for ensuring thread safety. */

    // FLAGS ---------------------------------------------------
    bool flagRun;   /**< Flag indicating whether the thread is running. */
    bool flagRead;

    // VARIABLES -----------------------------------------------
    std::list<can_frame_t> frameBuffer; /**< Buffer for storing CAN frames. */
    // CONNECTION ----------------------------------------------
    int canSocket;  /**< The socket for CAN communication. */

    int nBytes;
    
    std::unique_ptr<CanFrameBuilder> canFrameBuilder;
    std::shared_ptr<CanStructMapper> canStructMapper;

// CONSTRUCTORS AND METHODS #####################################

public:

    // CONSTRUCTOR ---------------------------------------------
    
    /**
     * @brief Constructs a CAN object.
     * @param port The port for CAN communication.
     */
    CAN(const char *port, std::shared_ptr<CanStructMapper> canStructMapper);

    /**
     * @brief Destructor for CAN.
     * 
     * Joins the thread when the programm is stoped.
     */
    ~CAN();

    // CONNECTION ----------------------------------------------

    /**
     * @brief Transmits data over the CAN bus.
     * @param id    The ID of the CAN message.
     * @param data  The data to be transmitted.
     * @param bytes The number of bytes in the data.
     */
    void transmit(std::size_t frameName);

    void transmit(std::string frameName);


    // GETTER --------------------------------------------------

     /**
     * @brief   Retrieves the size of the buffer.
     * @return  The size of the buffer.
     */
    int getBufferSize(void) const;

    /**
     * @brief   Reads the oldest frame from the buffer.
     * @note    The oldest frame will be deleted from the buffer.
     * @return  The frame read from the buffer.
     */
    std::optional<can_frame_t> getFrame(void);

private:

    // CAN -----------------------------------------------------
    
    /**
     * @brief Initializes the CAN communication.
     * @param port The port for CAN communication.
     */
    void init(const char *port);

    /**
     * @brief Receives data from the CAN bus.
     */
    void receive(void);



    /**
     * @brief Runs the thread for receiving data from the CAN bus.
     * @note  Stop with stop()
     */
    static void* run(void* arg);
    //void run();

    // SETTER --------------------------------------------------


    void frameToBuffer(can_frame_t frame);

};

#endif // CAN_HPP