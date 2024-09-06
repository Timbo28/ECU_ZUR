#ifndef PROFILING_TIMER_HPP
#define PROFILING_TIMER_HPP

#include <chrono>


#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>
#include "main.hpp"


/**
 * @brief Structure to store profile results.
 */
struct ProfileResult
{
    std::string Name; ///< Name of the profile.
    long long Start; ///< Start time of the profile.
    long long End; ///< End time of the profile.
    uint32_t ThreadID; ///< Thread ID associated with the profile.
};
/**
 * @brief Structure representing a profiling session.
 */
struct profiling_session_t
{
    std::string Name;///< Name of the profiling session.
};





/**
 * @class Profiling
 * @brief Profiling class responsible for recording profiling data in a json file. This json file can than be uploadet to "chrome://tracing" in the chrome browser, to visualise the profiling. .
 */
class Profiling
{
private:
    profiling_session_t* currentSession; ///< Pointer to the current profiling session.
    std::ofstream outputStream; ///< Output stream for writing profiling data to a file.
    int profileCount; ///< Counter for profile data.
public:
    Profiling(); ///< Constructor for Profiling class.
     /**
     * @brief Begins a profiling session.
     * @param name Name of the profiling session.
     * @param filepath Filepath to save the profiling data (default path provided).
     */
    void BeginSession(const std::string& name, const std::string& filepath = "/home/nvidia/Desktop/ECU_UASRacing/profiling_output/results.json");
    /**
     * @brief Ends the current profiling session.
     */
    void EndSession();
    /**
     * @brief Writes profile data to the output stream.
     * @param result ProfileResult structure containing profile data.
     */
    void WriteProfile(const ProfileResult& result);
    /**
     * @brief Writes the header for the profiling output.
     */
    void WriteHeader();
    /**
     * @brief Writes the footer for the profiling output.
     */
    void WriteFooter();
    /**
     * @brief Retrieves the instance of the Profiling class.
     * @return Reference to the Profiling instance.
     */
    static Profiling& Get();
};


/**
 * @class ProfilingTimer
 * @brief ProfilingTimer class used for profiling execution time.
 */
class ProfilingTimer
{
public:
    /**
     * @brief Constructor for ProfilingTimer class.
     * @param name Name of the profiling timer.
     */
    ProfilingTimer(const char* name);
    /**
     * @brief Destructor for ProfilingTimer class.
     */
    ~ProfilingTimer();
    /**
     * @brief Stops the profiling timer.
     */
    void Stop();
private:
    const char* name;///< Name of the profiling timer.
    std::chrono::time_point<std::chrono::high_resolution_clock> startTimepoint;
    bool stopped;///< Flag to indicate if the timer has stopped.
};
#endif // PROFILING_TIMER_HPP