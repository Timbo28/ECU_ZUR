#ifndef ECU_EXCEPTION_HPP
#define ECU_EXCEPTION_HPP

#include <stdexcept>
#include "Error.hpp"


namespace ecu{
    class EcuException : public std::runtime_error {
    public:
        explicit EcuException( error_code_t errorCode ,const std::string message) : 
                                std::runtime_error(message), errorCode(errorCode){}
        error_code_t getErrorCode() const noexcept{ 
            return errorCode; 
        }

    private:
        error_code_t errorCode;
    };

    template <typename T>
    std::string decToHexStr(T decNumber){
        std::stringstream stream;
        stream << std::hex << decNumber;
        return stream.str();
    }


}// namespace ecu

#endif // ECU_EXCEPTION_HPP

