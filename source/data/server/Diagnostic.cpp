
#include "data/server/Diagnostic.hpp"

#include <deque>

using namespace nlohmann;

// CONSTRUCTOR #################################################################

Diagnostic::Diagnostic(settings_t* settings)
    : UDP("Diagnostic", inBuffer, outBuffer, DIAGNOSTIC_BUFFER_SIZE),
      settings(settings){ PROFILING_FUNCTION();
}

Diagnostic::~Diagnostic(){ PROFILING_FUNCTION(); }

// METHODS: PRIVATE ############################################################

void Diagnostic::application(){ PROFILING_FUNCTION();

    static int sleep = 500;
    static int fastCounter = 1000;

    if (receive()){
        
        if(strstr(inBuffer, "control") != nullptr){
            overrideControl();
            fastCounter = 0;
        }
        else if (strstr(inBuffer, "cmd read logs") != nullptr) {  // send logs to UI
            sendLogs();
            fastCounter = 0;
        }
        else if (strstr(inBuffer, "cmd read settings") != nullptr) {   // send settings to UI
            sendSettings();
        }
        else if (strstr(inBuffer, "connection") != nullptr) {  // load settings to ECU
            saveSettings();
        }
    }

    if (fastCounter  > 100){   // go back to normal, when no control data
        sleep = 1000;
    }
    else{
        sleep = 10;
        fastCounter++;
    }
    

    this_thread::sleep_for(chrono::milliseconds(sleep));
}

void Diagnostic::overrideControl(){ PROFILING_FUNCTION();

    string message = "received";

    json control = json::parse(inBuffer);

    bool controlEnabled    = control["control"]["enabled"];
    bool frontLeftEnabled  = control["control"]["wheel"]["FL"];
    bool frontRightEnabled = control["control"]["wheel"]["FR"];
    bool backLeftEnabled   = control["control"]["wheel"]["BL"];
    bool backRightEnabled  = control["control"]["wheel"]["BR"];
    int  steering          = control["control"]["value"]["steering"];
    int  throttle          = control["control"]["value"]["throttle"];

    transmit(message);
}

void Diagnostic::sendLogs(){ PROFILING_FUNCTION();

    string message;
    int numberOfLogs = readBufferNumber();
    loadLogs(&message, numberOfLogs);

    transmit(message);
}

void Diagnostic::sendSettings(){ PROFILING_FUNCTION();

    string message;
    loadConfig(&message);

    transmit(message);
}

void Diagnostic::saveSettings(){ PROFILING_FUNCTION();

    string message = "received";
    saveConfig();

    transmit(message);
}

void Diagnostic::loadLogs(string* message, int logs){ PROFILING_FUNCTION();

    ifstream file("/var/log/ECU.log");
    string line;

    streampos index;
    vector<streampos> logIndex;

    int logCounter = 0;

    if (file.is_open()) {
        while (getline(file, line)) {
            if (line.find("ECU STARTED") != string::npos) {
                logIndex.push_back(index);
                logCounter++;
            }
            else{
                index = file.tellg();
            }
        }

        file.clear();
        if(logCounter-logs >= 0){
            file.seekg(logIndex[logCounter-logs]);
        }
        else{
            file.seekg(logIndex[0]);
        }
        
        *message = "";
        while (getline(file, line)) {
            *message += line + "\n";
        }

        file.close();
    } else {
        ECU_WARN("Diagnostic: Unable to open ECU.log")
    }
}

void Diagnostic::loadConfig(string* message){ PROFILING_FUNCTION();

    ifstream file(SETTINGS_PATH);
    json json = json::parse(file);
    file.close();

    *message = json.dump();
}

void Diagnostic::saveConfig(){ PROFILING_FUNCTION();

    json config = json::parse(inBuffer);

    ofstream file(SETTINGS_PATH);
    if (file.is_open()) {
        file << setw(4) << config << std::endl;
        file.close();
    }
    
    mtx.lock();
    settings->datalogger.port = config["connection"]["datalogger"]["port"];
    settings->driverless.port = config["connection"]["driverless"]["port"];
    settings->diagnostic.port = config["connection"]["diagnostic"]["port"];

    settings->datalogger.enabled = config["connection"]["datalogger"]["state"];
    settings->driverless.enabled = config["connection"]["driverless"]["state"];
    settings->diagnostic.enabled = config["connection"]["diagnostic"]["state"];
    mtx.unlock();
}

int Diagnostic::readBufferNumber(){

    int i;
    int j = 0;
    int start_index = -1;

    char num_str[3];
    int numberOfLogs = 1;

    for (i = 0; inBuffer[i] != '\0'; ++i) {
        if (isdigit(inBuffer[i])) {
            start_index = i;
            break;
        }
    }

    if (start_index != -1) {
       for (i = start_index; inBuffer[i] != '\0' && isdigit(inBuffer[i]); ++i) {
            num_str[j++] = inBuffer[i];
        }
        
        num_str[j] = '\0';

        numberOfLogs = atoi(num_str);
    }
    return numberOfLogs;
}