#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <optional>

#include "main.hpp"


#define ERROR(error) ecu::Error::setErrorCode(error);
#define CLEAR_ERRORS() ecu::Error::flushErrorCodeBuffer();

enum error_code_t{
    ERROR_IS_NOT_EXISTING=1,
    // database
    STRUCT_ORDER_ERROR,
    // JSON
    JSON_FILE_NOT_EXISTING,
    JSON_WRONG_DATA_TYPE,
    JSON_PARSE_ERROR,
    JSON_WRITE_ERROR,
    JSON_KEY_NOT_EXISTING,
    // Mapping
    CAN_FRAME_MAPPING_ERROR,
    MAPPING_INIT_ERROR,
    CAN_FRAME_DUPLICATE_ERROR,
    STRUCT_DUPLICATE_ERROR,
    MAPPING_DATA_TYPE_NOT_EXISTING,
    // CAN
    CAN_IS_NOT_EXISTING,
    CAN_FRAME_NOT_EXISTING,
    CAN0_INIT_SOCKET_ERROR,
    CAN1_INIT_SOCKET_ERROR,
    CAN2_INIT_SOCKET_ERROR,
    CAN0_SOCKET_BIND_ERROR,
    CAN1_SOCKET_BIND_ERROR,
    CAN2_SOCKET_BIND_ERROR,
    CAN0_CLOSE_SOCKET_ERROR,
    CAN1_CLOSE_SOCKET_ERROR,
    CAN2_CLOSE_SOCKET_ERROR,
    CAN0_TRANSMIT_ERROR,
    CAN1_TRANSMIT_ERROR,
    CAN2_TRANSMIT_ERROR,
    CAN0_TRANSMIT_FRAME_BUILDER_ERROR,
    CAN1_TRANSMIT_FRAME_BUILDER_ERROR,
    CAN2_TRANSMIT_FRAME_BUILDER_ERROR,
    CAN0_INIT_FRAME_BUILDER_ERROR,
    CAN1_INIT_FRAME_BUILDER_ERROR,
    CAN2_INIT_FRAME_BUILDER_ERROR,
    // UDP
    DATALOGGER_INIT_SOCKET_ERROR,
    DRIVERLESS_INIT_SOCKET_ERROR,
    DIAGNOSTIC_INIT_SOCKET_ERROR,
    DATALOGGER_SOCKET_BIND_ERROR,
    DRIVERLESS_SOCKET_BIND_ERROR,
    DIAGNOSTIC_SOCKET_BIND_ERROR,
    DATALOGGER_RECEIVE_ERROR,
    DRIVERLESS_RECEIVE_ERROR,
    DIAGNOSTIC_RECEIVE_ERROR,
    UDP_TX_BUFFER_TOSMALL,

    // Pedal
    PEDAL_DEVIATION_ERROR,
    PEDAL_SHORTAGE_RIGHT_ERROR,
    PEDAL_SHORTAGE_LEFT_ERROR,
    PEDAL_SHORTAGE_LEFT_TO_RIGHT_ERROR,
    PEDAL_LEFT_OUT_OF_RANGE_ERROR,
    PEDAL_RIGHT_OUT_OF_RANGE_ERROR,
    PEDAL_OUT_OF_RANGE_ERROR,
    // Break
    BREAK_FRONT_OUT_OF_RANGE_ERROR,
    BREAK_REAR_OUT_OF_RANGE_ERROR,
    BREAK_PRESSURE_DEVIATION_ERROR,
    // Motor
    MOTOR_OVERHEAT_ERROR,
    INVERTER_OVERHEAT_ERROR,
    IGBT_OVERHEAT_ERROR,
    PRECHARGE_CIRCUIT_TIMEOUT_ERROR,
    PRECHARGE_TIMEOUT_ERROR,
    HVCOUPLING_TIMEOUT_ERROR,
    INVERTER_RESET_ERROR,
    MOTOR_STATE_ERROR,
    MOTOR_PEDAL_ERROR,
    MOTOR_BREAK_ERROR,
    //Display
    DISPLAY_INIT_SOCKET_ERROR

    //

};



namespace ecu{

    struct error_t {
        std::string location;
        uint16_t code;
        bool critical;   // true = Critical error: Whole system is affected and Car has to stop
                         // false = Not critical error: Car can continue to drive
        std::string description;
    };



    class Error {
    public:
        static void setErrorCode(uint16_t error);
        static void setErrorCode(error_code_t errorCode);
        static uint16_t numberOfErrors();
        static void flushErrorCodeBuffer();
        static const std::map<error_code_t, error_t>& getErrorMap();
        static bool NoErrors();
        static bool hasCriticalError();
        static const std::vector<error_code_t>& getErrorBuffer();
        static uint16_t getErrorCode();
        static void printErrors();
        static std::optional<nlohmann::json> getErrorCodeJson();
        static nlohmann::json getErrorMapJson();
    private:
        Error() = delete;
        static std::vector<error_code_t> errorBuffer;
        static const std::map<error_code_t, error_t> errorMap;
        
    };

}


#endif // ERROR_HPP