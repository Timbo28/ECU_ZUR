/**
 * @file	log.hpp
 *
 * \note This is necessary so that the log message can contain the file name and the line number.
 *
 * \n
 * @authors	Marco Rau (raumar02@students.zhaw.ch)
 * 		    Tim Roos  (roostim1@students.zhaw.ch)
 * @brief 	Contains Definitions of Macros for logging.
 * @version	v1.0.0
 * @date	2023-12-22
 *
 * @copyright Copyright (c) 2023 Zurich UAS Racing
 *
 */

#ifndef ECU_UASRACING_LOG_HPP
#define ECU_UASRACING_LOG_HPP

#include "spdlog/spdlog.h"
#include "Error.hpp"

#include "main.hpp"

#define LOGGER_NAME "ECU_Logger"



#ifndef LOGGER_OFF
/**
 * @brief Logging for Trace-messages.
 *
 * This macro function is used to log trace messages.
 *
 * @param string Message string.
 * @param ... Multiple arguments which will be placed at marked positions("{}") in the string.
 */
#define ECU_TRACE(string, ...)  if(spdlog::get(LOGGER_NAME) != nullptr) \
                                {spdlog::get(LOGGER_NAME)->trace("[{}:{}] " string, \
                                std::strrchr(__FILE__, '/') ? std::strrchr(__FILE__, '/') + 1 : __FILE__, __LINE__, ##__VA_ARGS__);}
/**
 * @brief Logging for Debug-messages.
 *
 * This macro function is used to log debug messages.
 *
 * @param string Message string.
 * @param ... Multiple arguments which will be placed at marked positions("{}") in the string.
 */
#define ECU_DEBUG(string,...)      if(spdlog::get(LOGGER_NAME) != nullptr) \
                                    {spdlog::get(LOGGER_NAME)->debug("[{}:{}] " string, \
                                std::strrchr(__FILE__, '/') ? std::strrchr(__FILE__, '/') + 1 : __FILE__, __LINE__, ##__VA_ARGS__);}
/**
 * @brief Logging for Info-messages.
 *
 * This macro function is used to log info messages.
 *
 * @param string Message string.
 * @param ... Multiple arguments which will be placed at marked positions("{}") in the string.
 */
#define ECU_INFO(string,...)      if(spdlog::get(LOGGER_NAME) != nullptr) \
                                    {spdlog::get(LOGGER_NAME)->info("[{}:{}] " string, \
                                std::strrchr(__FILE__, '/') ? std::strrchr(__FILE__, '/') + 1 : __FILE__, __LINE__, ##__VA_ARGS__);}
/**
 * @brief Logging for Warn-messages.
 *
 * This macro function is used to log warn messages.
 *
 * @param string Message string.
 * @param ... Multiple arguments which will be placed at marked positions("{}") in the string.
 */
#define ECU_WARN(string,...)      if(spdlog::get(LOGGER_NAME) != nullptr) \
                                    {spdlog::get(LOGGER_NAME)->warn("[{}:{}] " string, \
                                std::strrchr(__FILE__, '/') ? std::strrchr(__FILE__, '/') + 1 : __FILE__, __LINE__, ##__VA_ARGS__);}
/**
 * @brief Logging for Error-messages.
 *
 * This macro function is used to log error messages.
 *
 * @param string Message string.
 * @param ... Multiple arguments which will be placed at marked positions("{}") in the string.
 */
#define ECU_ERROR(errorCode,string,...)      if(spdlog::get(LOGGER_NAME) != nullptr) \
                                    {spdlog::get(LOGGER_NAME)->error("[{}:{}] " string, \
                                std::strrchr(__FILE__, '/') ? std::strrchr(__FILE__, '/') + 1 : __FILE__, __LINE__, ##__VA_ARGS__);\
                                ecu::Error::setErrorCode(errorCode);}
/**
 * @brief Logging for Critical-messages.
 *
 * This macro function is used to log critical messages.
 *
 * @param string Message string.
 * @param ... Multiple arguments which will be placed at marked positions("{}") in the string.
 */
#define ECU_CRITICAL(errorCode,string,...)      if(spdlog::get(LOGGER_NAME) != nullptr) \
                                    {spdlog::get(LOGGER_NAME)->critical("[{}:{}] " string, \
                                std::strrchr(__FILE__, '/') ? std::strrchr(__FILE__, '/') + 1 : __FILE__, __LINE__, ##__VA_ARGS__); \
                                ecu::Error::setErrorCode(errorCode);}

#else
//Disable logging

#define ECU_TRACE(...)  (void)0
#define ECU_DEBUG(...)  (void)0
#define ECU_INFO(...)  (void)0
#define ECU_WARN(...)  (void)0
#define ECU_ERROR(...)  (void)0
#define ECU_CRITICAL(...)  (void)0

#endif //LOGGER_OFF
#endif //ECU_UASRACING_LOG_HPP
