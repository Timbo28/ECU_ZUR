#include <fstream> 
#include <cstdint>

#include "data/CanStructMapper.hpp"
#include "data/ecuJson.hpp"


CanStructMapper::CanStructMapper(std::string path){ PROFILING_FUNCTION();
    canInitFilePath = path;
    try{
        clearMappingJson();
        readCanFrames();
    }catch(const ecu::EcuException& e){
        ECU_ERROR( e.getErrorCode(),"Error in reading can frames: {}", e.what());
    }catch (const std::exception& exception) {
        ECU_ERROR(ERROR_IS_NOT_EXISTING,"{}",exception.what());
    }
}

CanStructMapper::CanStructMapper() 
    :CanStructMapper(CAN_FRAME_FILE_PATH){ PROFILING_FUNCTION();
}

void CanStructMapper::clearMappingJson(){ PROFILING_FUNCTION();
    nlohmann::json mappingJson;
    mappingJson.clear();
    ecu::setJsonFile(MAPPING_FILE_PATH, mappingJson);

}

void CanStructMapper::readCanFrames(){ PROFILING_FUNCTION();
    std::string analogName;
    std::string digitalName;
    std::string frameName;
    in_out_t direction;
    nlohmann::json frames;
    nlohmann::json analogData;
    nlohmann::json digitalData;
    canNames.clear();

    nlohmann::json canJson = ecu::getJsonFile(canInitFilePath);

    for (auto& can: canJson.items()) {
        frames = can.value();

        // add frame to frameCanMap which is used for transmission to identify which message needs to be send at which can bus---------------------------------
        uint8_t canPortNr;
        std::string port = can.key();
        // get port number
        char lastChar = port.back();
        canPortNr = atoi(&lastChar);
	    if (canPortNr < 0 || canPortNr > 3){
		ECU_ERROR(CAN_IS_NOT_EXISTING, "CAN port '{}' is not existing.",port);
	    }else{
            for(auto& frame: frames){
                if(ecu::getJsonValue<std::string>(frame, JSON_KEY_IN_OUT) == "OUT"){
                   frameName = ecu::getJsonValue<std::string>(frame, JSON_KEY_NAME);
                    frameCanMap[std::hash<std::string>{}(frameName)] = canPortNr; // add frame to map
                }
            }
           
        }
        //---------------------------------------------------------------------------------------------------------------------------------------------------
        // add frame to structNames-------------------------------------
        for(auto& frame: frames){
            std::string inOut = ecu::getJsonValue<std::string>(frame, JSON_KEY_IN_OUT);
            if(inOut == "IN"){
                direction = IN;
            }else if(inOut == "OUT"){
                direction = OUT;
            }else{
                auto ID = ecu::getJsonValue<std::string>(frame, JSON_KEY_CAN_ID);
                throw ecu::EcuException(MAPPING_INIT_ERROR,"Unknown direction: '" + inOut+"' at the frame with the ID " + ecu::decToHexStr(ID) + " in "+canInitFilePath);
            }
            
            analogData = ecu::getJsonValue<nlohmann::json>(frame, JSON_KEY_ANALOG);
            for(auto& analog  : analogData){
                analogName = ecu::getJsonValue<std::string>(analog, JSON_KEY_NAME);
                uniquePushBack(canNames,analogName,direction);
            }
            digitalData = ecu::getJsonValue<nlohmann::json>(frame, JSON_KEY_DIGITAL);
            for(auto& digital  : digitalData){
                digitalName = ecu::getJsonValue<std::string>(digital, JSON_KEY_NAME);
                uniquePushBack(canNames,digitalName,direction);
            }    
        }


        //--------------------------------------------------------------
    }
}


void CanStructMapper::uniquePushBack(std::unordered_map<std::string,in_out_t>& map, const std::string& element, in_out_t direction){ PROFILING_FUNCTION();
    if(map.find(element) == map.end()){
        map.emplace(element, direction);
    }else{
        //TODO: do something with the duplicate element
        //throw ecu::EcuException(CAN_FRAME_DUPLICATE_ERROR,"Element '" + element + "' already exists in the set. Duplicate 'Name' element in " + CAN_FRAME_FILE_PATH + " file");
    }
}



//method needs to be in try catch block
void CanStructMapper::updateMappingFile(){ PROFILING_FUNCTION();
    nlohmann::json mappingJson;
    mappingJson = ecu::getJsonFile(MAPPING_FILE_PATH);

    //add new elements to the mapping file
    for (auto& name : structNames) {
        if (mappingJson.find(name) == mappingJson.end()) {
            mappingJson.emplace(name,checkForMissingElements(name));
        }else{
            mappingJson[name] = checkForMissingElements(name);
        }
    }
 
    ecu::setJsonFile(MAPPING_FILE_PATH, mappingJson);
}

//check if element in struct is mapped to can frame
bool CanStructMapper::checkForMissingElements(std::string name){ PROFILING_FUNCTION();
        if(canNames.find(name) == canNames.end() ){
            ECU_WARN("Element '{}'in the struct is not mapped. See the {} file",name,MAPPING_FILE_PATH);
            return false;
        }else{
            return true;
        }
}

void CanStructMapper::mapInputCanFrame(const can_frame_t& canFrame){ PROFILING_FUNCTION();
    // check if frame needs to be synchronized-------------------------------------
    
    if(canFrame.syncFrame != 0){
        // check if syncBuffer is empty
        if(syncBuffer.empty()){
            syncBuffer.push_back(canFrame);
            return;
        }else{
            // check if the to be synchronized frame is in the buffer
            for(auto frame = syncBuffer.begin(); frame != syncBuffer.end(); frame++){
                if(frame->name == canFrame.syncFrame){
                    loadCanFrameToStruct(*frame);
                    loadCanFrameToStruct(canFrame);
                    syncBuffer.erase(frame);
                    return;
                  
                }else if(frame->name == canFrame.name){
                    auto hexID = ecu::decToHexStr(canFrame.CanBusID);
                    //ECU_WARN("Frame with ID '{}' is already in the syncBuffer",hexID);
                    return;
                }
            }
            syncBuffer.push_back(canFrame);
        }
    }else{
        loadCanFrameToStruct(canFrame);
    }
    //------------------------------------------------------------------------------
    
}

void CanStructMapper::loadCanFrameToStruct(const can_frame_t& canFrame){ PROFILING_FUNCTION();
    for(auto& [key,value]: canFrame.AnalogData){
        if(structMapIn.find(key) != structMapIn.end()){
            std::memcpy(structMapIn[key], &value.value, (value.size+7)/8);
        }else{
            auto hexID = ecu::decToHexStr(canFrame.CanBusID);
            ECU_ERROR(CAN_FRAME_MAPPING_ERROR,"Frame with ID '{}' could not be mapped to a struct element. See the {} file",hexID,MAPPING_FILE_PATH)
        }
    }
    for(auto& [key,value]: canFrame.DigitalData){
        if(structMapIn.find(key) != structMapIn.end()){
            *static_cast<bool*>(structMapIn[key]) = value.value;
        }else{
            auto hexID = ecu::decToHexStr(canFrame.CanBusID);
            ECU_ERROR(CAN_FRAME_MAPPING_ERROR,"Frame with ID '{}' could not be mapped to a struct element. See the {} file",hexID,MAPPING_FILE_PATH)
        }
    }
}


void CanStructMapper::mapOutputCanFrame(can_frame_t& canFrame){ PROFILING_FUNCTION();
    for(auto& [key,value]: canFrame.AnalogData){
        if(structMapOut.find(key) != structMapOut.end()){
            std::memcpy(&value.value, structMapOut[key], (value.size+7)/8);
        }else{
            ECU_ERROR(CAN_FRAME_MAPPING_ERROR,"Frame with ID '{}' could not be mapped to a struct element. See the {} file",canFrame.CanBusID,MAPPING_FILE_PATH)
        }
    }
    for(auto& [key,value]: canFrame.DigitalData){
        if(structMapOut.find(key) != structMapOut.end()){
            value.value = *static_cast<bool*>(structMapOut[key]);
        }else{
            ECU_ERROR(CAN_FRAME_MAPPING_ERROR,"Frame with ID '{}' could not be mapped to a struct element. See the {} file",canFrame.CanBusID,MAPPING_FILE_PATH)
        }
    }
}

std::unordered_map<std::size_t, uint8_t>& CanStructMapper::getFrameCanMap(){ PROFILING_FUNCTION();
    return frameCanMap;
}




