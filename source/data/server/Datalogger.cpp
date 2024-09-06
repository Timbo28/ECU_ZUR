#include "data/server/Datalogger.hpp"

// CONSTRUCTOR #################################################################

Datalogger::Datalogger()
    : UDP("Datalogger", inBuffer, outBuffer, BUFFER_SIZE){ PROFILING_FUNCTION();
}

Datalogger::~Datalogger(){ PROFILING_FUNCTION(); }

// METHODS: PRIVATE ############################################################

void Datalogger::application(){ PROFILING_FUNCTION();

    // CODE
}