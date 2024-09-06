
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <fcntl.h>

#include "data/server/UDP.hpp"
#include <iostream>

using namespace std;


// CONSTRUCTOR #################################################################
UDP::UDP(const char* name, char *inBuffer,char *outBuffer, int size)
    : name(name), port(port), inBuffer(inBuffer), outBuffer(outBuffer), size(size){ PROFILING_FUNCTION();
}

UDP::~UDP(){ PROFILING_FUNCTION();

    stopApplication();
}

// METHODS: PUBLIC #############################################################

bool UDP::startApplication(int port){ PROFILING_FUNCTION();
    error_code_t errorCode;
    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        if(name == "Datalogger"){
            errorCode = DATALOGGER_INIT_SOCKET_ERROR;
        }else if(name == "Driverless"){
            errorCode = DRIVERLESS_INIT_SOCKET_ERROR;
        }else if(name == "Diagnostic"){
            errorCode = DIAGNOSTIC_INIT_SOCKET_ERROR;
        }else if(name == "Display"){
            errorCode = DISPLAY_INIT_SOCKET_ERROR;
        }else{
            errorCode = ERROR_IS_NOT_EXISTING;
        }

        ECU_ERROR(errorCode,"{}: Socket could not be initialised.", name);
        return false;
    }

    memset((char *) &serverAddress,0, sizeof(serverAddress));

    serverAddress.sin_family      = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port        = htons(port);

    if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0){
        error_code_t errorCode;
        if(name == "Datalogger"){
            errorCode = DATALOGGER_SOCKET_BIND_ERROR;
        }else if(name == "Driverless"){
            errorCode = DRIVERLESS_SOCKET_BIND_ERROR;
        }else if(name == "Diagnostic"){
            errorCode = DIAGNOSTIC_SOCKET_BIND_ERROR;
        }else if(name == "Display"){
            errorCode = DISPLAY_INIT_SOCKET_ERROR;
        }else{
            errorCode = ERROR_IS_NOT_EXISTING;
        }
        ECU_ERROR(errorCode,"{}: Socket could not be bind.", name);
        return false;
    }

    listen(serverSocket, 5);

    clientLength = sizeof(clientAddress);

    //fcntl(serverSocket, F_SETFL, fcntl(serverSocket, F_GETFL, 0) | O_NONBLOCK);

    thread = std::thread(&UDP::loop, this);

    ECU_INFO("{}: Port {} open", name, port);

    return true;
}

void UDP::stopApplication(void){

    threadFlag = false;

    if (thread.joinable()) {
        thread.join();
    }

    close(serverSocket);

    ECU_INFO("{}: closed", name);
}

bool UDP::receive(void){

    int n = recvfrom(serverSocket, &inBuffer[0], size, MSG_DONTWAIT, (struct sockaddr *) &clientAddress, &clientLength);
    
    if(n > 0){
        inBuffer[n] = '\0';
        return true;
    }
    else if (n == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
        error_code_t errorCode;
        if (name == "Datalogger"){
            errorCode = DATALOGGER_RECEIVE_ERROR;
        }else if(name == "Driverless"){
            errorCode = DRIVERLESS_RECEIVE_ERROR;
        }else if(name == "Diagnostic"){
            errorCode = DIAGNOSTIC_RECEIVE_ERROR;
        }else if(name == "Display"){
            errorCode = DISPLAY_INIT_SOCKET_ERROR;
        }else{
            errorCode = ERROR_IS_NOT_EXISTING;
        }
        ECU_ERROR(errorCode,"{}: Error in received message", name);
        close(serverSocket);
        return false;
    }

    return false;
}

void UDP::transmit(const std::string& message){
    if(message.size() <= size){
        strncpy(&outBuffer[0], message.c_str(), message.size());
        sendto(serverSocket, &outBuffer[0], message.size(), MSG_DONTWAIT, (const struct sockaddr *) &clientAddress, clientLength); 
    }else{
        ECU_ERROR(UDP_TX_BUFFER_TOSMALL,"{}: Message is too long", name);
    
    }
}

// METHODS: PRIVATE ############################################################

void UDP::loop(void){

    threadFlag = true;

    while(threadFlag){
        
        application();
    }
}