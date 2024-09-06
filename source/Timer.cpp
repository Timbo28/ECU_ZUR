#include <stdexcept>
#include <iostream>
#include <functional>

#include "Timer.hpp"

Timer::Timer(int autoStopTime) { PROFILING_FUNCTION();
    this->autoStopTime = autoStopTime;
    timerRunning = false;
}

void Timer::start() { PROFILING_FUNCTION();
    if(timerRunning){
        if(autoStopCheck()){
            start();
        }
    }else{
        startTime = high_resolution_clock::now();
        timerRunning = true;
    }
}

void Timer::restart() { PROFILING_FUNCTION();
    startTime = high_resolution_clock::now();
    timerRunning = true;
}


void Timer::stop() {
    timerRunning = false;
}


double Timer::pastTime() { PROFILING_FUNCTION();
    if(!autoStopCheck()) {
        if (timerRunning) {
            auto nowTime = high_resolution_clock::now();
            auto duration = duration_cast<nanoseconds>(nowTime - startTime);
            return static_cast<int>(duration.count() / timeResolution); // zeit in ms standard resolution of nothing else is set

        }
    }
    return 0;
}

bool Timer::isActive() { PROFILING_FUNCTION();
    autoStopCheck();
    return timerRunning;
}

bool Timer::isStopped()  { PROFILING_FUNCTION();
    autoStopCheck();
    return !timerRunning;
}

double Timer::unprotectedPastTime() { PROFILING_FUNCTION();
        auto nowTime = high_resolution_clock::now();
        auto duration = duration_cast<nanoseconds>(nowTime - startTime);
        return duration.count() / timeResolution; // zeit in ms standard resolution of nothing else is set
}

bool Timer::autoStopCheck(){ PROFILING_FUNCTION();
    if(timerRunning){
        if (autoStopTime != 0){ //check if autoStopTime is set on
            if(unprotectedPastTime() >= autoStopTime){
                stop();
                return true;
            }
        }
    }
    return false;
}

void Timer::setTimerResolution(TimeResolution Resolution) { PROFILING_FUNCTION();
    switch (Resolution) {
        case s:
            timeResolution = s;
            break;
        case ms:
            timeResolution = ms;
            break;
        case us:
            timeResolution = us;
            break;
        case ns:
            timeResolution = ns;
            break;
    }
}
