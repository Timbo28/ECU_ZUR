#include <iostream>
#include "Error.hpp"



std::vector<error_code_t> ecu::Error::errorBuffer;
const std::map<error_code_t, ecu::error_t> ecu::Error::errorMap = {
    {ERROR_IS_NOT_EXISTING, {"Error", ERROR_IS_NOT_EXISTING, true, "Error is not existing"}},
    // database
    {STRUCT_ORDER_ERROR, {"Database", STRUCT_ORDER_ERROR, true, "Element is not in the correct order. Check the database.hpp file. Order needs to be _FL, _FR, _RL, _RR"}},
    // JSON
    {JSON_FILE_NOT_EXISTING, {"JSON", JSON_FILE_NOT_EXISTING, true, "JSON file is not existing or the path is wrong"}},
    {JSON_PARSE_ERROR, {"JSON", JSON_PARSE_ERROR, true, "Error in parsing JSON file"}}, 
    {JSON_WRITE_ERROR, {"JSON", JSON_WRITE_ERROR, true, "Error in writing JSON file"}}, 
    {JSON_KEY_NOT_EXISTING, {"JSON", JSON_KEY_NOT_EXISTING, true, "Key is not existing in the JSON file check the JSON file for the correct key"}},
    // Mapping
    {CAN_FRAME_MAPPING_ERROR, {"Mapping", CAN_FRAME_MAPPING_ERROR, true, "Error in mapping the CanFrame to the struct element. Check the mapping file and the canFrame file"}},
    {MAPPING_INIT_ERROR, {"Mapping", MAPPING_INIT_ERROR, true, "Error in initialising the mapping file"}},
    {CAN_FRAME_DUPLICATE_ERROR, {"Mapping", CAN_FRAME_DUPLICATE_ERROR, true, "Duplicate 'Name' element in CanMapping.json file"}},
    {STRUCT_DUPLICATE_ERROR, {"Mapping", STRUCT_DUPLICATE_ERROR, true, "Duplicate 'Name' element in the database.hpp"}},
    {MAPPING_DATA_TYPE_NOT_EXISTING, {"Mapping", MAPPING_DATA_TYPE_NOT_EXISTING, true, "Data type is not existing in the mapping file"}},
    // CAN
    {CAN_IS_NOT_EXISTING, {"Can", CAN_IS_NOT_EXISTING, true, "Can is not existing"}},
    {CAN_FRAME_NOT_EXISTING, {"Can", CAN_FRAME_NOT_EXISTING, true, "Can frame is not existing in the frame builder Map of any Can"}},
    {CAN0_INIT_SOCKET_ERROR, {"Can0", CAN0_INIT_SOCKET_ERROR, true, "Socket could not be initialised"}},
    {CAN1_INIT_SOCKET_ERROR, {"Can1", CAN1_INIT_SOCKET_ERROR,true, "Socket could not be initialised"}},
    {CAN2_INIT_SOCKET_ERROR, {"Can2", CAN2_INIT_SOCKET_ERROR,true, "Socket could not be initialised"}},
    
    {CAN0_SOCKET_BIND_ERROR, {"Can0", CAN0_SOCKET_BIND_ERROR,true, "Socket could not be bind"}},
    {CAN1_SOCKET_BIND_ERROR, {"Can1", CAN1_SOCKET_BIND_ERROR,true, "Socket could not be bind"}},
    {CAN2_SOCKET_BIND_ERROR, {"Can2", CAN2_SOCKET_BIND_ERROR,true, "Socket could not be bind"}},
    
    {CAN0_CLOSE_SOCKET_ERROR, {"Can0", CAN0_CLOSE_SOCKET_ERROR,true, "Socket could not be closed"}},
    {CAN1_CLOSE_SOCKET_ERROR, {"Can1", CAN1_CLOSE_SOCKET_ERROR,true, "Socket could not be closed"}},
    {CAN2_CLOSE_SOCKET_ERROR, {"Can2", CAN2_CLOSE_SOCKET_ERROR,true, "Socket could not be closed"}},
    
    {CAN0_TRANSMIT_ERROR, {"Can0", CAN0_TRANSMIT_ERROR,true, "Error in transmitted message, check Can0"}},
    {CAN1_TRANSMIT_ERROR, {"Can1", CAN1_TRANSMIT_ERROR,true, "Error in transmitted message, check Can1"}},
    {CAN2_TRANSMIT_ERROR, {"Can2", CAN2_TRANSMIT_ERROR,true, "Error in transmitted message, check Can2"}},
    
    {CAN0_TRANSMIT_FRAME_BUILDER_ERROR, {"Can0", CAN0_TRANSMIT_FRAME_BUILDER_ERROR,true, "Transmit frame could not be built. Check Can0 frame builder"}},
    {CAN1_TRANSMIT_FRAME_BUILDER_ERROR, {"Can1", CAN1_TRANSMIT_FRAME_BUILDER_ERROR,true, "Transmit frame could not be built. Check Can1 frame builder"}},
    {CAN2_TRANSMIT_FRAME_BUILDER_ERROR, {"Can2", CAN2_TRANSMIT_FRAME_BUILDER_ERROR,true, "Transmit frame could not be built. Check Can2 frame builder"}},

    {CAN0_INIT_FRAME_BUILDER_ERROR, {"Can0", CAN0_INIT_FRAME_BUILDER_ERROR,true, "Frame builder could not be initialised"}},    
    {CAN1_INIT_FRAME_BUILDER_ERROR, {"Can1", CAN1_INIT_FRAME_BUILDER_ERROR,true, "Frame builder could not be initialised"}},
    {CAN2_INIT_FRAME_BUILDER_ERROR, {"Can2", CAN2_INIT_FRAME_BUILDER_ERROR,true, "Frame builder could not be initialised"}},

    // UDP
    {DATALOGGER_INIT_SOCKET_ERROR, {"Datalogger", DATALOGGER_INIT_SOCKET_ERROR,false, "Socket could not be initialised"}},
    {DRIVERLESS_INIT_SOCKET_ERROR, {"Driverless", DRIVERLESS_INIT_SOCKET_ERROR,true, "Socket could not be initialised"}},
    {DIAGNOSTIC_INIT_SOCKET_ERROR, {"Diagnostic", DIAGNOSTIC_INIT_SOCKET_ERROR,false, "Socket could not be initialised"}},

    {DATALOGGER_SOCKET_BIND_ERROR, {"Datalogger", DATALOGGER_SOCKET_BIND_ERROR,false, "Socket could not be bind"}},
    {DRIVERLESS_SOCKET_BIND_ERROR, {"Driverless", DRIVERLESS_SOCKET_BIND_ERROR,true, "Socket could not be bind"}},
    {DIAGNOSTIC_SOCKET_BIND_ERROR, {"Diagnostic", DIAGNOSTIC_SOCKET_BIND_ERROR,false, "Socket could not be bind"}},

    {DATALOGGER_RECEIVE_ERROR, {"Datalogger", DATALOGGER_RECEIVE_ERROR,false, "Error in received message"}},
    {DRIVERLESS_RECEIVE_ERROR, {"Driverless", DRIVERLESS_RECEIVE_ERROR,true, "Error in received message"}},
    {DIAGNOSTIC_RECEIVE_ERROR, {"Diagnostic", DIAGNOSTIC_RECEIVE_ERROR,false, "Error in received message"}},
    {UDP_TX_BUFFER_TOSMALL, {"UDP", UDP_TX_BUFFER_TOSMALL, false, "UDP transmit buffer is too small"}},
    // Pedalbox
    {PEDAL_DEVIATION_ERROR, {"Pedalbox", PEDAL_DEVIATION_ERROR,true, "Deviation between Acceleration sensors"}},
    {PEDAL_SHORTAGE_RIGHT_ERROR, {"Pedalbox", PEDAL_SHORTAGE_RIGHT_ERROR,true, "Right side pedal sensor has any shortage"}},
    {PEDAL_SHORTAGE_LEFT_ERROR, {"Pedalbox", PEDAL_SHORTAGE_LEFT_ERROR,true, "Left side pedal sensor has any shortage"}},
    {PEDAL_SHORTAGE_LEFT_TO_RIGHT_ERROR, {"Pedalbox", PEDAL_SHORTAGE_LEFT_TO_RIGHT_ERROR,true, "Left side pedal sensor has any shortage to right side pedal sensor"}},
    {PEDAL_LEFT_OUT_OF_RANGE_ERROR, {"Pedalbox", PEDAL_LEFT_OUT_OF_RANGE_ERROR,true, "Left side pedal sensor is out of range"}},
    {PEDAL_RIGHT_OUT_OF_RANGE_ERROR, {"Pedalbox", PEDAL_RIGHT_OUT_OF_RANGE_ERROR,true, "Right side pedal sensor is out of range"}},
    {PEDAL_OUT_OF_RANGE_ERROR, {"Pedalbox", PEDAL_OUT_OF_RANGE_ERROR,true, "Pedal is out of range. More than 100% throttle or less than 0%"}},
    {BREAK_FRONT_OUT_OF_RANGE_ERROR, {"Break", BREAK_FRONT_OUT_OF_RANGE_ERROR,true, "Front break pressure sensor is out of range"}},
    {BREAK_REAR_OUT_OF_RANGE_ERROR, {"Break", BREAK_REAR_OUT_OF_RANGE_ERROR,true, "Rear break pressure sensor is out of range"}},
    {BREAK_PRESSURE_DEVIATION_ERROR, {"Break", BREAK_PRESSURE_DEVIATION_ERROR,true, "Break pressure difference is too high. Check break system!"}},
    // Motor
    {MOTOR_OVERHEAT_ERROR, {"Motor", MOTOR_OVERHEAT_ERROR,true, "Motor is overheating temperature is over 80°C"}},   
    {INVERTER_OVERHEAT_ERROR, {"Inverter", INVERTER_OVERHEAT_ERROR,true, "Inverter is overheating temperature is over 80°C"}},
    {IGBT_OVERHEAT_ERROR, {"IGBT", IGBT_OVERHEAT_ERROR,true, "IGBT is overheating temperature is over 80°C"}},
    {PRECHARGE_CIRCUIT_TIMEOUT_ERROR, {"Precharge", PRECHARGE_CIRCUIT_TIMEOUT_ERROR,true, "Precharge circuit was closed for more than 5s "}},
    {PRECHARGE_TIMEOUT_ERROR, {"Precharge", PRECHARGE_TIMEOUT_ERROR,true, "The precharge state took more than 7s "}},
    {HVCOUPLING_TIMEOUT_ERROR, {"HVcoupling", HVCOUPLING_TIMEOUT_ERROR,true, "HVCoupling state took more than 3s "}},
    {INVERTER_RESET_ERROR, {"Inverter", INVERTER_RESET_ERROR,true, " Inverter error reset was not successful"}},
    {MOTOR_STATE_ERROR, {"Motor", MOTOR_STATE_ERROR,true, "Inverter reset was successful but old motor state is unknown"}},
    {MOTOR_PEDAL_ERROR, {"Motor", MOTOR_PEDAL_ERROR,true, "Motor went to error state because of pedal error"}},
    {MOTOR_BREAK_ERROR, {"Motor", MOTOR_BREAK_ERROR,true, "Motor went to error state because of break error"}},
    // Display
    {DISPLAY_INIT_SOCKET_ERROR, {"Display", DISPLAY_INIT_SOCKET_ERROR,true, "Socket could not be initialised"}},

};

void ecu::Error::setErrorCode(uint16_t errorCode){ PROFILING_FUNCTION();
    error_code_t error = static_cast<error_code_t>(errorCode);
    setErrorCode(error);
};

void ecu::Error::setErrorCode(error_code_t errorCode){ PROFILING_FUNCTION();
    // check if Error exists in errorMap
    if(errorMap.find(errorCode) != errorMap.end()){
        //check if error is already in errorCodeBuffer
        for (auto &error : errorBuffer){
            if (error == errorCode){
                return;
            }
        }
        errorBuffer.push_back(errorCode);
        #ifdef DEBUG
            ecu::Error::printErrors();
        #endif
    }else{
        ECU_ERROR(ERROR_IS_NOT_EXISTING,"Error '{}' not found in errorMap",static_cast<uint16_t>(errorCode));
    }
};



uint16_t ecu::Error::numberOfErrors(){ PROFILING_FUNCTION();
    return errorBuffer.size();
};

void ecu::Error::flushErrorCodeBuffer(){ PROFILING_FUNCTION();
    errorBuffer.clear();
};

const std::map<error_code_t, ecu::error_t>& ecu::Error::getErrorMap(){ PROFILING_FUNCTION();
    return errorMap;
};

bool ecu::Error::NoErrors(){ PROFILING_FUNCTION();
    return errorBuffer.empty();
};

bool ecu::Error::hasCriticalError(){ PROFILING_FUNCTION();
    for (const auto &error : errorBuffer){
        if (errorMap.find(error)->second.critical){
            return true;
        }
    }
    return false;
};

const std::vector<error_code_t>& ecu::Error::getErrorBuffer(){ PROFILING_FUNCTION();
    return errorBuffer;
};

uint16_t ecu::Error::getErrorCode(){ PROFILING_FUNCTION();
    if (!errorBuffer.empty()){
        return errorBuffer.front();
    }
    return 0;
};

void ecu::Error::printErrors(){ PROFILING_FUNCTION();
    if (!errorBuffer.empty()){
        std::cout << "--------------------------------" << std::endl;
        std::cout << "ERRORS: " << std::endl;
        std::cout << "--------------------------------" << std::endl;
        for (const auto &error : errorBuffer){ 
            auto errorObject  = errorMap.find(error)->second;
            std::cout << "Location: "       << errorObject.location       << std::endl
                      << " Code: "          << errorObject.code           << std::endl
                      << " Critical Error: "<< errorObject.critical       << std::endl
                      << " Description: "   << errorObject.description    << std::endl;
        }
        std::cout << "--------------------------------" << std::endl;
        std::cout<<std::endl;
    }else{
        std::cout << "No Errors" << std::endl;
    }
};



std::optional<nlohmann::json> ecu::Error::getErrorCodeJson(){ PROFILING_FUNCTION();
    nlohmann::json errorJson;
    nlohmann::json errorArray = nlohmann::json::array(); 
    if (!errorBuffer.empty()){
        for (const auto &error : errorBuffer){
            errorArray.push_back(error); 
        }
        errorJson["ErrorCodes"] = errorArray; 
        return errorJson;
    }
    return std::nullopt;
}

nlohmann::json ecu::Error::getErrorMapJson(){ PROFILING_FUNCTION();
    nlohmann::json errorArray = nlohmann::json::array();
    nlohmann::json errorHeader = nlohmann::json::object(
                                    {{"bn","ECU"},{"n","error"},{"vs","map"}}
    ); 
    errorArray.push_back(errorHeader);
    for (const auto& error : errorMap) {
        nlohmann::json errorObj;
        errorObj = nlohmann::json::object({
            {"location", error.second.location},
            {"code", error.second.code},
            {"critical", error.second.critical},
            {"description", error.second.description}
        });
        errorArray.push_back(errorObj);
    }

    return errorArray; 
};


