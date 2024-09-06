#include "ProfilingTimer.hpp"


ProfilingTimer::ProfilingTimer(const char* name){
    this->name = name;
    this->stopped = false;
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    startTimepoint = std::chrono::high_resolution_clock::now();
}

ProfilingTimer::~ProfilingTimer(){
    if (!stopped)
        Stop();
}

void ProfilingTimer::Stop(){
    auto endTimepoint = std::chrono::high_resolution_clock::now();

    long long start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimepoint).time_since_epoch().count();
    long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

    uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
    Profiling::Get().WriteProfile({ name, start, end, threadID });

    stopped = true;
}
#include <mutex>
#include "ProfilingManager.hpp"

std::mutex g_outputStreamMutex; // Mutex to Synchronise the outputStream



Profiling::Profiling(){
    currentSession = nullptr;
    profileCount = 0;
}

void Profiling::BeginSession(const std::string& name, const std::string& filepath ){
    profileCount = 0;
    outputStream.open(filepath);
    WriteHeader();
    currentSession = new profiling_session_t{ name };
}

void Profiling::EndSession(){
    WriteFooter();
    outputStream.close();
    delete currentSession;
    currentSession = nullptr;
    profileCount = 0;
}

void Profiling::WriteProfile(const ProfileResult& result){
    std::lock_guard<std::mutex> lock(g_outputStreamMutex);
    if (profileCount++ > 0)
        outputStream << ",";

    std::string name = result.Name;
    std::replace(name.begin(), name.end(), '"', '\'');

    outputStream << "{";
    outputStream << "\"cat\":\"function\",";
    outputStream << "\"dur\":" << (result.End - result.Start) << ',';
    outputStream << "\"name\":\"" << name << "\",";
    outputStream << "\"ph\":\"X\",";
    outputStream << "\"pid\":0,";
    outputStream << "\"tid\":" << result.ThreadID << ",";
    outputStream << "\"ts\":" << result.Start;
    outputStream << "}";

    outputStream.flush();
}

void Profiling::WriteHeader(){
    outputStream << "{\"otherData\": {},\"traceEvents\":[";
    outputStream.flush();
}

void Profiling::WriteFooter(){
    outputStream << "]}";
    outputStream.flush();
}

Profiling& Profiling::Get(){
    static Profiling instance;
    return instance;
}

