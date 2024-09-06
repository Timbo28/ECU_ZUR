
#include <algorithm>
#include <cstdint>
#include <iostream>

#include "data/server/Driverless.hpp"

// CONSTRUCTOR #################################################################

Driverless::Driverless(std::shared_ptr<DataMapper> dataMapper)
    : UDP("Driverless", inBuffer, outBuffer, BUFFER_SIZE),
    dataMapper(dataMapper) { PROFILING_FUNCTION();

}

Driverless::~Driverless(){ PROFILING_FUNCTION(); 
}

// METHODS: PRIVATE ############################################################

void Driverless::application(){
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

