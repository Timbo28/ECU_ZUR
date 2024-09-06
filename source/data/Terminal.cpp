#include <iostream>
#include <sstream>
#include <unistd.h>

#include "LogManager.hpp"
#include "data/Terminal.hpp"


// CONSTRUCTOR #################################################################

Terminal::Terminal(settings_t& settingsRef,  sensor_data_t& sensorDataRef) : settings(settingsRef), sensorData(sensorDataRef) { PROFILING_FUNCTION();
    isRunning = true;
    terminalThread = std::thread(&Terminal::run, this);
    ECU_INFO("Terminal: started");
}

Terminal::~Terminal() { PROFILING_FUNCTION();

    isRunning = false;
    if (terminalThread.joinable()) {
        terminalThread.join();
    }
    ECU_INFO("Terminal: stopped");
}

// METHODS: PRIVATE ############################################################

void Terminal::run(){ PROFILING_FUNCTION();

    while (isRunning){
        std::vector<std::string> args;
        std::string command;

        readInput(command,args);
        
        if (command == NOTHING){
            // do nothing
        } else if (command == SHUTDOWN) {
            commandSHUTDOWN();
        } else if (command == SERVER){
           commandSERVER(args);
        } else if (command == LOG_LEVEL){
            commandLOG_LEVEL(args);
        } else if (command == VEHICLE_STATUS){
            commandVEHICLESTATUS(args);
        } else if(command == MOTOR_ENABLE){
            commandMOTOR_ENABLE(args);
        }else if(command == MOTOR_LOAD){
            commandMOTOR_LOAD(args);
        }else if(command == MAX_TORQUE){
            commandMAX_TORQUE(args);
        } else if(command == PEDAL_RESPONSIVENESS){
            commandPEDAL_RESPONSIVENESS(args);
        }else if(command == MOTOR_TEMP){
                commandMOTOR_TEMP(args);
        }else if(command == INVERTER_TEMP){
                commandINVERTER_TEMP(args);
        }else if(command == ACCU_TEMP){
                commandACCU_TEMP(args);
        }else if(command == ACCU_CHARGE){
                commandACCU_CHARGE(args);
        }else if(command == MAX_POWER){
                commandMAX_POWER(args);
        }else if(command == POWER_LIMIT_KP){
                commandPOWER_LIMIT_KP(args);
        }else if(command == POWER_LIMIT_KI){
                commandPOWER_LIMIT_KI(args);
        }else{
            std::cout << "Command '"<< command <<  "' was not found." << std::endl;
        }
        
    }
}

void Terminal::readInput(std::string& command, std::vector<std::string>& args){ PROFILING_FUNCTION();

        std::string input;
        char c;
        

        deactivateLineBuffer();

        // Loop to read input character by character
        while (std::cin.get(c)) {
            // If input is empty and the character is a visible ASCII character(not a SPACE or ENTER)
            if (input.empty() && c >32 && c < 127) {
                // Get the current log level and set the console log level to "off"
                currentLevel = LogManager::getConsoleLogLevel();
                LogManager::setConsoleLogLevel("off");
            }
            if (c == '\n') {
                // If input is not empty, restore the previous console log level and print a newline
                if (!input.empty()){
                    try{
                        LogManager::setConsoleLogLevel(currentLevel);
                    }catch(const std::invalid_argument& exception){
                        std::cout  <<std::endl;
                    }
                        
                }
                std::cout<<std::endl;
                break;// Exit the loop

            // If the character is a backspace(8) or delete(127) character
            }else if (c == 8 || c == 127) { 
                //If input is not empty, remove the last character from the input string and erase it from the console
                if (!input.empty()) {
                    input.pop_back(); 
                    std::cout << "\b \b"; 
                }
            } else {
                
                input += c;
                std::cout << c;
            }
        }

        activateLineBuffer();

        // Create an input string stream from the collected input
        std::istringstream iss(input);

        // Extract the first word from the input stream as a command
        iss >> command;

        // Extract remaining words from the input stream as arguments
        std::string argument;
        while (iss >> argument) {
            args.push_back(argument);
        }

}


void Terminal::deactivateLineBuffer(){ PROFILING_FUNCTION();

    tcgetattr(STDIN_FILENO, &originalTermios);// Get the current terminal settings for standard input
    newTermios = originalTermios;// Make a copy of the original terminal settings
    newTermios.c_lflag &= ~(ICANON | ECHO); // Disable line buffering and echo for input
    tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);// Set the modified terminal settings immediately for standard input
}

void Terminal::activateLineBuffer(){ PROFILING_FUNCTION();

    tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios); // Restore the original settings

}

void Terminal::commandSHUTDOWN(){ PROFILING_FUNCTION();

    isRunning = false;
    settings.shutdown = true;
}

void Terminal::commandSERVER(std::vector<std::string>& args){ PROFILING_FUNCTION();

    try {
        if (args.size()<1){ //check if ther are one argument entered
            throw std::invalid_argument("Arguments are missing.");
        }
        if(args[0] == "on"){
            if (args.size()<2){ //check if ther are one argument entered
                throw std::invalid_argument("Arguments are missing.");
            }
            else{
                //settings.serverPort = std::stoi(args[1]);
                settings.datalogger.enabled = true;
            }
        }else if (args[0] == "off"){
            settings.datalogger.enabled = false;
        }else{
            throw std::invalid_argument("Arg 1: The entered command is not existing.");
        }
    } catch (const std::invalid_argument& exception) {
        std::cout << exception.what()  <<std::endl;
    }
}

void Terminal::commandLOG_LEVEL(std::vector<std::string>& args){ PROFILING_FUNCTION();

    try {
        if (args.size()<2){ //check if ther are two argument entered
            throw std::invalid_argument("Arguments are missing.");
        }
        if(args[0] == "console"){
            if(args[1] == "get"){
                std::cout << "The Log-Level is '" << currentLevel << "'." << std::endl;
            }else{
                LogManager::setConsoleLogLevel(args[1]);
            }
        }else if (args[0] == "file"){
            if(args[1] == "get"){
                std::cout << "The Log-Level is '" << LogManager::getFileLogLevel() << "'." << std::endl;
            }else{
                LogManager::setFileLogLevel(args[1]);
            }
        }else{
            throw std::invalid_argument("Arg 1: The entered log sink is not existing.");
        }
    } catch (const std::invalid_argument& exception) {
        std::cout << exception.what()  <<std::endl;
        
    }

}

void Terminal::commandVEHICLESTATUS(std::vector<std::string>& args){ PROFILING_FUNCTION();

    try{
        if (args.size()<1){ //check if ther is at least one argument
            throw std::invalid_argument("Arguments are missing.");
        }
        if (args[0] == "off"){
            sensorData.SDC_Pressed = true;
        }else if (args[0] == "on"){
            sensorData.SDC_Pressed = false;
        }else if (args[0] == "reset"){
            sensorData.SDC_Pressed = true;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            sensorData.SDC_Pressed = false;
        }else if (args[0] == "get"){
            if(!sensorData.SDC_Pressed){
                std::cout << "Vehicle Status = "<< "on" << std::endl;
            }else{
                std::cout << "Vehicle Status = "<< "off" << std::endl;
            }
        }else{
            throw std::invalid_argument("Arg1: The entered argument is not existing.");
        }   
    }catch(const std::invalid_argument& exception){
        std::cout << std::endl<< exception.what()  <<std::endl;
    }

}

void Terminal::commandMOTOR_ENABLE(std::vector<std::string>& args){
    try{
        if (args.size()<1){ //check if ther is at least one argument
            std::cout << "Motor Enable: FL: "<<settings.motorEnable[0]<< ", FR: " << settings.motorEnable[1] << ", RL: "<<settings.motorEnable[2] << ", RR: "<< settings.motorEnable[3]<<std::endl;
        }else{
            if (args[0] == "fl"){
            if(args[1] == "1"){
                settings.motorEnable[0] = true;
            }else if(args[1] == "0"){
                settings.motorEnable[0] = false;
            }else{
                throw std::invalid_argument("Arg2: The entered argument is not existing.");
            }
        }else if (args[0] == "fr"){
            if(args[1] == "1"){
                settings.motorEnable[1] = true;
            }else if(args[1] == "0"){
                settings.motorEnable[1] = false;
            }else{
                throw std::invalid_argument("Arg2: The entered argument is not existing.");
            }
        }else if (args[0] == "rl"){
            if(args[1] == "1"){
                settings.motorEnable[2] = true;
            }else if(args[1] == "0"){
                settings.motorEnable[2] = false;
            }else{
                throw std::invalid_argument("Arg2: The entered argument is not existing.");
            }
        }else if (args[0] == "rr"){
            if(args[1] == "1"){
                settings.motorEnable[3] = true;
            }else if(args[1] == "0"){
                settings.motorEnable[3] = false;
            }else{
                throw std::invalid_argument("Arg2: The entered argument is not existing.");
            }
        }else{
            throw std::invalid_argument("Arg1: The entered argument is not existing.");
        }   
        std::cout << "Motor Enable: FL: "<<settings.motorEnable[0]<< ", FR: " << settings.motorEnable[1] << ", RL: "<<settings.motorEnable[2] << ", RR: "<< settings.motorEnable[3]<<std::endl;
        }
        
    }catch(const std::invalid_argument& exception){
        std::cout << std::endl<< exception.what()  <<std::endl;
    }
}

void Terminal::commandMAX_TORQUE(std::vector<std::string>& args){
    try{
        if (args.size()<1){ //check if ther is at least one argument
            throw std::invalid_argument("Arguments are missing.");
        }
        if (args[0] == "get"){
            std::cout << "Max Torque: " << settings.maxTorque << std::endl;
        }else if (args[0] == "set"){
            if (args.size()<2){ //check if ther is at least one argument
                throw std::invalid_argument("Arguments are missing.");
            }
            settings.maxTorque = std::stoi(args[1]);
            std::cout << "new Max Torque: " << settings.maxTorque << std::endl;
        }else{
            throw std::invalid_argument("Arg1: The entered argument is not existing.");
        }   
        
    }catch(const std::invalid_argument& exception){
        std::cout << std::endl<< exception.what()  <<std::endl;
    }
}

void Terminal::commandPEDAL_RESPONSIVENESS(std::vector<std::string>& args){
    try{
        if (args.size()<1){ //check if ther is at least one argument
            throw std::invalid_argument("Arguments are missing.");
        }
        if (args[0] == "get"){
            std::cout << "Pedal Responsiveness (alpha): " << (int)settings.alpha << std::endl;
        }else if (args[0] == "set"){
            if (args.size()<2){ //check if ther is at least one argument
                throw std::invalid_argument("Arguments are missing.");
            }
            settings.alpha = std::stoi(args[1]);
            std::cout << "new Pedal Responsiveness (alpha): " << (int)settings.alpha << std::endl;
        }else{
            throw std::invalid_argument("Arg1: The entered argument is not existing.");
        }   
        
    }catch(const std::invalid_argument& exception){
        std::cout << std::endl<< exception.what()  <<std::endl;
    }
}



void Terminal::commandMOTOR_TEMP(std::vector<std::string>& args){
    std::cout << "Motor temp: FL " << sensorData.inverter.motorTemp_FL << 
                           ", FR "<<sensorData.inverter.motorTemp_FR << 
                           " ,RL " << sensorData.inverter.motorTemp_RL << 
                           " ,RR " << sensorData.inverter.motorTemp_RR << std::endl;
}

void Terminal::commandINVERTER_TEMP(std::vector<std::string>& args){
    std::cout << "Inverter temp: FL " << sensorData.inverter.inverterTemp_FL << 
                           ", FR "<<sensorData.inverter.inverterTemp_FR << 
                           " ,RL " << sensorData.inverter.inverterTemp_RL << 
                           " ,RR " << sensorData.inverter.inverterTemp_RR << std::endl;

    std::cout << "IGBT temp: FL " << sensorData.inverter.IGBTTemp_FL << 
                           ", FR "<<sensorData.inverter.IGBTTemp_FR << 
                           " ,RL " << sensorData.inverter.IGBTTemp_RL << 
                           " ,RR " << sensorData.inverter.IGBTTemp_RR << std::endl;
}

void Terminal::commandACCU_TEMP(std::vector<std::string>& args){
    std::cout << "Accu temp: FL " << sensorData.accu.accuTemperature <<  std::endl;
}

void Terminal::commandACCU_CHARGE(std::vector<std::string>& args){
    std::cout << "Accu charge: " << sensorData.accu.accuStateOfCharge << std::endl;
}

void Terminal::commandMOTOR_LOAD(std::vector<std::string>& args){
    try{
        if (args.size()<1){ //check if ther is at least one argument
            std::cout << "Motor Load: FL: "<< (int)settings.motorLoad[0]<< ", FR: " << (int)settings.motorLoad[1] << ", RL: "<<(int)settings.motorLoad[2] << ", RR: "<< (int)settings.motorLoad[3]<<std::endl;
        }else{
            if (args[0] == "fl"){
                if (std::stoi(args[1]) > 0 && std::stoi(args[1]) <= 100) {
                    settings.motorLoad[0] = std::stoi(args[1]);
                } else {
                    throw std::invalid_argument("Arg2: The entered argument is not within the valid range.");
                }
            }else if (args[0] == "fr"){
                if (std::stoi(args[1]) > 0 && std::stoi(args[1]) <= 100) {
                    settings.motorLoad[1] = std::stoi(args[1]);
                } else {
                    throw std::invalid_argument("Arg2: The entered argument is not within the valid range.");
                }
            }else if (args[0] == "rl"){
                if (std::stoi(args[1]) > 0 && std::stoi(args[1]) <= 100) {
                    settings.motorLoad[2] = std::stoi(args[1]);
                } else {
                    throw std::invalid_argument("Arg2: The entered argument is not within the valid range.");
                }
            }else if (args[0] == "rr"){
                if (std::stoi(args[1]) > 0 && std::stoi(args[1]) <= 100) {
                    settings.motorLoad[3] = std::stoi(args[1]);
                } else {
                    throw std::invalid_argument("Arg2: The entered argument is not within the valid range.");
                }
            }else{
                throw std::invalid_argument("Arg1: The entered argument is not existing.");
            }   
            std::cout << "Motor Enable: FL: "<<(int)settings.motorLoad[0]<< ", FR: " << (int)settings.motorLoad[1] << ", RL: "<<(int)settings.motorLoad[2] << ", RR: "<< (int)settings.motorLoad[3]<<std::endl;
        }
         }catch(const std::invalid_argument& exception){
        std::cout << std::endl<< exception.what()  <<std::endl;
    }
}

void Terminal::commandMAX_POWER(std::vector<std::string>& args){
    try{
        if (args.size()<1){ //check if ther is at least one argument
            throw std::invalid_argument("Arguments are missing.");
        }
        if (args[0] == "get"){
            std::cout << "Max Power: " << settings.maxPower << std::endl;
        }else if (args[0] == "set"){
            if (args.size()<2){ //check if ther is at least one argument
                throw std::invalid_argument("Arguments are missing.");
            }
            settings.maxPower = std::stoi(args[1]);
            std::cout << "new Max Power: " << settings.maxPower << std::endl;
        }else{
            throw std::invalid_argument("Arg1: The entered argument is not existing.");
        }   
        
    }catch(const std::invalid_argument& exception){
        std::cout << std::endl<< exception.what()  <<std::endl;
    }
}

void Terminal::commandPOWER_LIMIT_KP(std::vector<std::string>& args){
    try{
        if (args.size()<1){ //check if ther is at least one argument
            throw std::invalid_argument("Arguments are missing.");
        }
        if (args[0] == "get"){
            std::cout << "Power Limit KP: " << settings.powerLimit_kp << std::endl;
        }else if (args[0] == "set"){
            if (args.size()<2){ //check if ther is at least one argument
                throw std::invalid_argument("Arguments are missing.");
            }
            settings.powerLimit_kp = std::stof(args[1]);
            std::cout << "new Power Limit KP: " << settings.powerLimit_kp << std::endl;
        }else{
            throw std::invalid_argument("Arg1: The entered argument is not existing.");
        }   
        
    }catch(const std::invalid_argument& exception){
        std::cout << std::endl<< exception.what()  <<std::endl;
    }
}

void Terminal::commandPOWER_LIMIT_KI(std::vector<std::string>& args){
    try{
        if (args.size()<1){ //check if ther is at least one argument
            throw std::invalid_argument("Arguments are missing.");
        }
        if (args[0] == "get"){
            std::cout << "Power Limit KI: " << settings.powerLimit_ki << std::endl;
        }else if (args[0] == "set"){
            if (args.size()<2){ //check if ther is at least one argument
                throw std::invalid_argument("Arguments are missing.");
            }
            settings.powerLimit_ki = std::stof(args[1]);
            std::cout << "new Power Limit KI: " << settings.powerLimit_ki << std::endl;
        }else{
            throw std::invalid_argument("Arg1: The entered argument is not existing.");
        }   
        
    }catch(const std::invalid_argument& exception){
        std::cout << std::endl<< exception.what()  <<std::endl;
    }
}