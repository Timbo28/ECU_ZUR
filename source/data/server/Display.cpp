#include "data/server/Display.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include "Timer.hpp"

// CONSTRUCTOR #################################################################

Display::Display(std::shared_ptr<DataMapper> dataMapper)
    : UDP("Display", inBuffer, outBuffer, BUFFER_SIZE),
    dataMapper(dataMapper){ PROFILING_FUNCTION();
}

Display::~Display(){ PROFILING_FUNCTION(); }

// METHODS: PRIVATE ############################################################

void Display::application(){ PROFILING_FUNCTION();
    auto startUpdateCycle = std::chrono::high_resolution_clock::now();
    if(receive()){
       // inBuffer to json
       try{
            nlohmann::json request = nlohmann::json::parse(inBuffer);
            std::cout << request.dump() << std::endl; 
            nlohmann::json respons = dataMapper->getSenML(request);
            transmit(respons.dump());
       }catch(nlohmann::json::parse_error& e){
           ECU_WARN("Display: received invalid json: {}", e.what());
       }catch( std::exception& e){
            ECU_WARN("Display: {}", e.what());           
       }
    }

    std::this_thread::sleep_until(startUpdateCycle + std::chrono::milliseconds(100));
}