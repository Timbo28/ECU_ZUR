/**
 * @file	LogManager.hpp
 * @authors	Marco Rau (raumar02@students.zhaw.ch)
 * 		    Tim Roos  (roostim1@students.zhaw.ch)
 * @brief 	Managing the SPDLog library.
 * @version	v1.0.0
 * @date	2023-12-22
 *
 * @copyright Copyright (c) 2023 Zurich UAS Racing
 *
 */
#ifndef ECU_UASRACING_LOGMANAGER_HPP
#define ECU_UASRACING_LOGMANAGER_HPP

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"

#include "main.hpp"

/**
 * @class LogManager
 * @brief This class creates a logger with multiple sinks whit SPDLog library.
 *
 * This class is fully static and can be called from any where in the program.
 *
 *  Here is a list of the log levels in the spd library:\n
 *
 *   -trace: Detailed tracing of program execution.\n
 *   -debug: Debugging messages for code debugging.\n
 *   -info: General information about program execution.\n
 *   -warn: Warnings for potential issues.\n
 *   -error: Errors that don't halt program execution.\n
 *   -critical: Critical errors that might terminate the program.\n
 *   -off: Turns off all logging.\n
 *
 */
class LogManager {
public:
    /**
    * @brief Initialisation for the logger, a console and file sink,
     * with custom pattern and different log levels.
     *
     * costume log pattern example:\n
     * [warning] [17-11-2023 12:15:21.217] 'example log message'\n
     *\n
     * Default loglevel:\n
     * -consoleSink = warn\n
     * -fileSink = trace\n
    */
    static void init();
    /**
     * @brief Set the log level for the console sink.
     * @param strLevel A string representing the log level ("trace", "debug", "info", "warn", "error", "critical", "off").
     */
    static void setConsoleLogLevel(const std::string& strLevel);

    /**
     * @brief Set the log level for the file sink.
     * @param strLevel A string representing the log level ("trace", "debug", "info", "warn", "error", "critical", "off").
     */
    static void setFileLogLevel(const std::string& strLevel);

    /**
     * @brief Get the log level for the console sink.
     * @return A string representing the current log level for the console sink.
     */
    static std::string getConsoleLogLevel();

    /**
     * @brief Get the log level for the file sink.
     * @return A string representing the current log level for the file sink.
     */
    static std::string getFileLogLevel();

    /**
     * @brief Shutdown the logger when the program terminates.
     */
    static void shutdown();

private:
    /**
     * @brief Convert a string representation of log level to its enum counterpart.
     * @param levelStr A string representing the log level ("trace", "debug", "info", "warn", "error", "critical", "off").
     * @return The corresponding enum value of the log level.
     * @throw std::invalid_argument if an invalid log level string is passed.
     */
    static spdlog::level::level_enum stringToLogLevel(const std::string& levelStr);

    /**
     * @brief Convert a log level enum to its string representation.
     * @param logLevel The log level enum.
     * @return The string representation of the log level.
     */
    static std::string logLevelToString(spdlog::level::level_enum logLevel);

    static std::shared_ptr<spdlog::sinks::sink> consoleSink; /**< Pointer to the console sink.*/
    static std::shared_ptr<spdlog::sinks::sink> fileSink; /**< Pointer to the file sink.*/
};


#endif //ECU_UASRACING_LOGMANAGER_HPP
