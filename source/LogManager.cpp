#include "LogManager.hpp"



std::shared_ptr<spdlog::sinks::sink> LogManager::consoleSink;
std::shared_ptr<spdlog::sinks::sink> LogManager::fileSink;


void LogManager::init() {  
    consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_pattern("[%l] %^[%d-%m-%Y %H:%M:%S.%e] %v%$");
    consoleSink->set_level(spdlog::level::trace);

    auto max_size = 10485760;
    auto max_files = 0;
    fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("/var/log/ECU.log",max_size,max_files);
    fileSink->set_pattern("[%l] %^[%d-%m-%Y %H:%M:%S.%e] %v%$");
    fileSink->set_level(spdlog::level::trace);

    std::vector<spdlog::sink_ptr> sinks{consoleSink, fileSink};
    auto logger = std::make_shared<spdlog::logger>(LOGGER_NAME, sinks.begin(), sinks.end());
    logger->set_level(spdlog::level::trace);
    logger->flush_on(spdlog::level::trace);
    spdlog::register_logger(logger);

}



void LogManager::shutdown() {  
    spdlog::shutdown();
}

void LogManager::setConsoleLogLevel(const std::string &strLevel) {  
    spdlog::level::level_enum level = LogManager::stringToLogLevel(strLevel);
    consoleSink->set_level(level);
}

void LogManager::setFileLogLevel(const std::string &strLevel)  {  
    spdlog::level::level_enum level = LogManager::stringToLogLevel(strLevel);
    fileSink->set_level(level);
}

std::string LogManager::getConsoleLogLevel() {  
    return logLevelToString(LogManager::consoleSink->level());
}

std::string LogManager::getFileLogLevel() {  
    return logLevelToString(LogManager::fileSink->level());
}

spdlog::level::level_enum LogManager::stringToLogLevel(const std::string &levelStr) {  
    if (levelStr == "trace") {
        return spdlog::level::trace;
    } else if (levelStr == "debug") {
        return spdlog::level::debug;
    } else if (levelStr == "info") {
        return spdlog::level::info;
    } else if (levelStr == "warn") {
        return spdlog::level::warn;
    } else if (levelStr == "error") {
        return spdlog::level::err;
    } else if (levelStr == "critical") {
        return spdlog::level::critical;
    } else if (levelStr == "off") {
        return spdlog::level::off;
    }  else {
        throw std::invalid_argument("Arg2: The entered log level is not existing.");
    }
}

std::string LogManager::logLevelToString(spdlog::level::level_enum logLevel) {  
    if (logLevel == spdlog::level::trace) {
        return "trace" ;
    } else if (logLevel == spdlog::level::debug) {
        return "debug" ;
    } else if (logLevel == spdlog::level::info) {
        return"info" ;
    } else if (logLevel == spdlog::level::warn) {
        return "warn" ;
    } else if (logLevel == spdlog::level::err) {
        return "error" ;
    } else if (logLevel == spdlog::level::critical) {
        return "critical";
    }else{
        return "off";
    }
}




