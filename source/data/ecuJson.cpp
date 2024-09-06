#include "data/ecuJson.hpp"
#include <fstream>



nlohmann::json ecu::getJsonFile(std::string filePath){PROFILING_FUNCTION();
    // open file
    std::ifstream initJsonStream(filePath);
    if (!initJsonStream.is_open()) {
        throw ecu::EcuException(JSON_FILE_NOT_EXISTING,"Could not open file " + filePath);
    }


    try{
        nlohmann::json mappingJson = nlohmann::json::parse(initJsonStream); 
        return mappingJson;
    } catch (std::exception& e) {
        throw ecu::EcuException(JSON_PARSE_ERROR,"Could not parse file " + filePath);
    }
    
}

void ecu::setJsonFile(std::string filePath, const nlohmann::json& json){PROFILING_FUNCTION();
    std::ofstream initJsonStream(filePath);
    if (!initJsonStream.is_open()) {
        throw ecu::EcuException(JSON_FILE_NOT_EXISTING,"Could not open file " + filePath);
    }

    initJsonStream << json.dump(4);
    if(initJsonStream.fail()){
        throw ecu::EcuException(JSON_WRITE_ERROR,"Could not write to file " + filePath);
    }
}

