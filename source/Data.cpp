#include "Data.hpp"
#include "data/CanStructMapper.hpp"




// CONSTRUCTOR #################################################################
Data::Data()
    : terminal(std::make_unique<Terminal>(settings,sensorData)),
      dataMapper(std::make_shared<DataMapper>(REGISTER_3_STRUCTS)),
      canStructMapper(std::make_shared<CanStructMapper>()),
      can{  std::make_unique<CAN>("can0", canStructMapper),
            std::make_unique<CAN>("can1", canStructMapper),
            std::make_unique<CAN>("can2", canStructMapper),
            std::make_unique<CAN>("can3", canStructMapper)},
      datalogger(std::make_unique<Datalogger>()),
      driverless(std::make_unique<Driverless>(dataMapper)),
      diagnostic(std::make_unique<Diagnostic>(&settings)),
      display(std::make_unique<Display>(dataMapper))
      { PROFILING_FUNCTION();
    numberOfCan = can.size();

    canStructMapper->registerStruct(sensorData);
    dataMapper->registerStruct(sensorData, true);
    dataMapper->registerStruct(driverlessCommands);
    
    initSettings();

    serverInit(datalogger, settings.datalogger.enabled, settings.datalogger.port);
    serverInit(driverless, settings.driverless.enabled, settings.driverless.port);
    serverInit(diagnostic, settings.diagnostic.enabled, settings.diagnostic.port);
    serverInit(display, settings.display.enabled, settings.display.port);
}
// METHODS: PUBLIC #############################################################

void Data::update(void){ PROFILING_FUNCTION();

    // To detect terminal input changes
    static bool dataloggerState = settings.datalogger.enabled;
    static bool driverlessState = settings.driverless.enabled;
    static bool diagnosticState = settings.diagnostic.enabled;
    static bool displayState = settings.display.enabled;

    
    int size[numberOfCan];

    // Server: turns applications(threads) on/off
    serverConfig(datalogger, dataloggerState, settings.datalogger.enabled, settings.datalogger.port);
    serverConfig(driverless, driverlessState, settings.driverless.enabled, settings.driverless.port);
    serverConfig(diagnostic, diagnosticState, settings.diagnostic.enabled, settings.diagnostic.port);
    serverConfig(display, displayState, settings.display.enabled, settings.display.port);

    // CAN: Read all input data of EV
    for(int port = 0; port < numberOfCan; port++){
        size[port] = can[port]->getBufferSize();
    }
    //find max size
    int* maxSize = std::max_element(size, size + numberOfCan);
    

    for (int i = 0; i < *maxSize; i++){
        for (int port = 0; port < numberOfCan; port++){
                std::optional<can_frame_t> frame = can[port]->getFrame();
                if(frame.has_value()){
                    canStructMapper->mapInputCanFrame(frame.value());
                }
        }
    }
    


}

void Data::CANTransmit(void){ PROFILING_FUNCTION();
    // Transmits all frames in the CAN_data_out_t struct

    std::unordered_map<std::size_t, uint8_t>& frameNames = canStructMapper->getFrameCanMap();
	
    for (auto [frameName,port] : frameNames){

		can[port]->transmit(frameName);
	}
}


void Data::CANTransmit(std::string frameName){ PROFILING_FUNCTION();
    auto it = canStructMapper->getFrameCanMap().find(std::hash<std::string>{}(frameName)); //TODO get rid of hashing
    if(it == canStructMapper->getFrameCanMap().end()){
        ECU_ERROR(CAN_FRAME_NOT_EXISTING ,"CAN Frame {} could not be transmitted, frame is not existing in the frame builder Map of any Can", frameName);
    }else{
        can[it->second]->transmit(frameName);
    }
}

sensor_data_t* Data::getSensorData(void){ PROFILING_FUNCTION();

    return &sensorData;
}

settings_t* Data::getSettings(void){ PROFILING_FUNCTION();

    return &settings;
}

driverless_commands_t* Data::getDriverlessCommands(void){ PROFILING_FUNCTION();

    return &driverlessCommands;
}

// METHODS: PRIVATE ############################################################

void Data::initSettings(void){ PROFILING_FUNCTION();

    ifstream initJsonStream(SETTINGS_PATH);
    nlohmann::json json = nlohmann::json::parse(initJsonStream);
    initJsonStream.close();

    settings.datalogger.port = json["connection"]["datalogger"]["port"];
    settings.driverless.port = json["connection"]["driverless"]["port"];
    settings.diagnostic.port = json["connection"]["diagnostic"]["port"];
    settings.display.port = json["connection"]["display"]["port"];

    settings.datalogger.enabled = json["connection"]["datalogger"]["state"];
    settings.driverless.enabled = json["connection"]["driverless"]["state"];
    settings.diagnostic.enabled = json["connection"]["diagnostic"]["state"];
    settings.display.enabled = json["connection"]["display"]["state"];
}



template <typename Application>
void Data::serverInit(const Application& server, bool& terminal, int port){ PROFILING_FUNCTION();
    if (server != nullptr){
        if (terminal){
            server->startApplication(port);
        }
    }
}

template <typename Application>
void Data::serverConfig(const Application& server, bool& actual, bool& terminal, int port){ PROFILING_FUNCTION();

    if(actual != terminal){
        actual = terminal;
        if (server != nullptr){
            if(actual == true){
                server->startApplication(port);
            }
            else{
                server->stopApplication();
            }
        }
        else{
            ECU_WARN("Server: was not created");
        }
    }
}

