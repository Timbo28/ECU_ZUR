#ifndef ECU_JSON_HPP
#define ECU_JSON_HPP

#include <nlohmann/json.hpp>

#include "EcuException.hpp"
#include "main.hpp"

namespace ecu {

nlohmann::json getJsonFile(std::string filePath);
void setJsonFile(std::string filePath, const nlohmann::json& json);

template<typename Return, typename Key>
Return getJsonValue(const nlohmann::json& json, const Key& key) { PROFILING_FUNCTION();
    if (json.find(key) != json.end()) {
        try{        
            return json.template find(key).value().template get<Return>(); // Add 'template' keyword before find() and get()
        } catch (nlohmann::json::exception& e) {
            throw ecu::EcuException(JSON_WRONG_DATA_TYPE, std::string(e.what()) + std::string(". Check the json file for the correct data type"));
        }
    } else {
        throw ecu::EcuException(JSON_KEY_NOT_EXISTING,std::string("Key '") + std::string(key) + "' not found in the json file");
    }
    
    // Add a return statement here
    return Return();
}

} // namespace ecu

#endif // ECU_JSON_HPP
