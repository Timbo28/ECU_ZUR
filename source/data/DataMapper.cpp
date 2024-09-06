#include <iostream>
#include "data/DataMapper.hpp"
#include "data/ecuJson.hpp"
#include "log.hpp"


DataMapper::DataMapper(uint8_t nrStructsToBeRegistered)
                        : DataMapper(SENML_CONFIG_FILE_PATH,nrStructsToBeRegistered){}

DataMapper::DataMapper(std::string path, uint8_t nrStructsToBeRegistered)
                        : configFilePath(path),
                        nrStructsToBeRegistered(nrStructsToBeRegistered){}

void DataMapper::readMessageContentFile(){
    std::string bn,n,vs;
    nlohmann::json configFile = ecu::getJsonFile(configFilePath);

    for(auto& message : configFile){
        
        bn = ecu::getJsonValue<std::string>(message[0],"bn");
        n = ecu::getJsonValue<std::string>(message[0],"n");
        vs = ecu::getJsonValue<std::string>(message[0],"vs");

        message_content_t& newMessageContent = messageContent[bn][n][vs];
        
        for(int i = 1; i < message.size(); i++){
            std::string pointerName = message[i].back();
            message[i].back() = 0;
            if(!pointerName.empty()){
                if(data.find(pointerName) != data.end()){
                    newMessageContent.pointer.push_back(data[pointerName].pointer);
                    newMessageContent.size.push_back(data[pointerName].size);
                    newMessageContent.isFloat.push_back(data[pointerName].isFloat);
                    newMessageContent.readOnly.push_back(data[pointerName].readOnly);
                }else{
                    //TODO error handling pointer to pointer name is not existing in struct
                    ECU_WARN("Pointer to pointer name '{}' is not existing in struct",pointerName);
                }
            }
            
        }
        newMessageContent.senML = message;
    }
}

nlohmann::json DataMapper::handelDataRequest(std::string bn, std::string n, std::string vs){
    auto badRequest = requestValidation(bn,n,vs);
    if(badRequest.has_value()){
        return badRequest.value();
    }
    //TODO add mutex
    nlohmann::json emptyMessage = messageContent[bn][n][vs].senML;
    if(n == "error"){
        if(vs == "code"){
            emptyMessage[1].at("v") = ecu::Error::getErrorCode();
        }else if(vs == "map"){
            return ecu::Error::getErrorMapJson();
        }else{
            //TODO error handling
            ECU_WARN("Error in getSenML");
        }
    }else{
        for(int i = 1; i < emptyMessage.size(); i++){
            if(emptyMessage[i].find("v") != emptyMessage[i].end()){
                if(messageContent[bn][n][vs].isFloat[i-1]){
                    if(messageContent[bn][n][vs].size[i-1] == sizeof(float)){
                        emptyMessage[i].at("v") = *static_cast<float*>(messageContent[bn][n][vs].pointer[i-1]);
                    }else if(messageContent[bn][n][vs].size[i-1] == sizeof(double)){
                        emptyMessage[i].at("v") = *static_cast<double*>(messageContent[bn][n][vs].pointer[i-1]);
                    }else{
                        //TODO error handling
                        ECU_WARN("Error in getSenML");
                    }
                }else{
                    uint64_t value = 0;
                    std::memcpy(&value,messageContent[bn][n][vs].pointer[i-1],messageContent[bn][n][vs].size[i-1]);
                    emptyMessage[i].at("v") = value;
                }
            }else if(emptyMessage[i].find("vb") != emptyMessage[i].end()){
                emptyMessage[i].at("vb") = *static_cast<bool*>(messageContent[bn][n][vs].pointer[i-1]);
            }else{
                //TODO error handling
                ECU_WARN("Error in getSenML");
            }
        }
    }
    
    return emptyMessage;
}

nlohmann::json DataMapper::getSenML(nlohmann::json& requestData){
    std::string bn,n,vs;
    nlohmann::json response = nlohmann::json::array();
    nlohmann::json controlRequest = nlohmann::json::array();

    for(int i = 0; i < requestData.size(); i++){
        try{
           
            bn = ecu::getJsonValue<std::string>(requestData[i][0],"bn");
            n = ecu::getJsonValue<std::string>(requestData[i][0],"n");
            vs = ecu::getJsonValue<std::string>(requestData[i][0],"vs");
            if(requestData[i].size() == 1){
                // senML data request
                response.push_back(handelDataRequest(bn,n,vs));
            }else{
                // senML control request
                response.push_back(handelControlRequest(bn,n,vs, requestData[i]));
            }
            
        }catch(const ecu::EcuException& e){
            ECU_WARN("Bad senML request: {}", e.what());
        }catch (const nlohmann::json::exception& e) {
            ECU_WARN("Bad senML request: {}", e.what());
        }catch (const std::exception& exception) {
            ECU_WARN("Bad senML request: format is wronge every request must have a 'bn,n,vs' and must be in an array []:  {}", exception.what());
        }
    }
    return response;
}

nlohmann::json DataMapper::handelControlRequest (std::string bn, std::string n, std::string vs, nlohmann::json& controlRequest){
    auto badRequest = requestValidation(bn,n,vs);
    if(badRequest.has_value()){
        return badRequest.value();
    }
  
    for(int i = 1; i < controlRequest.size(); i++){   
        if(!messageContent[bn][n][vs].readOnly[i-1]){

            if(controlRequest[i].find("v") != controlRequest[i].end()){  

                if(messageContent[bn][n][vs].isFloat[i-1]){
                    if(messageContent[bn][n][vs].size[i-1] == sizeof(float)){
                        *static_cast<float*>(messageContent[bn][n][vs].pointer[i-1]) = controlRequest[i].at("v");
                    }else if(messageContent[bn][n][vs].size[i-1] == sizeof(double)){
                        *static_cast<double*>(messageContent[bn][n][vs].pointer[i-1]) = controlRequest[i].at("v");
                    }else{
                        //TODO error handling
                        ECU_WARN("Error in handelControlRequest");
                    }
                }else{
                    uint64_t value = controlRequest[i].at("v");
                    std::memcpy(messageContent[bn][n][vs].pointer[i-1],&value,messageContent[bn][n][vs].size[i-1]);
                }  
            }else if(controlRequest[i].find("vb") != controlRequest[i].end()){
                *static_cast<bool*>(messageContent[bn][n][vs].pointer[i-1]) = controlRequest[i].at("vb");
            }else{
                //TODO error handling
                ECU_WARN("Error in handelControlRequest");
            }
        }else{
            //TODO error handling
            ECU_WARN("pointer is read only");
        }
    }
    nlohmann::json response = nlohmann::json::array();
    response.push_back(controlRequest[0]);
    return response;

}

std::optional<nlohmann::json> DataMapper::requestValidation(std::string bn, std::string n, std::string vs){
     nlohmann::json badRequest;

    if (messageContent.find(bn) == messageContent.end()){
        return badRequest = {{"bn",bn},{"n",n},{"vs",vs},{"badRequest","bn: " + bn + " is not existing"}};
    }
    if (messageContent[bn].find(n) == messageContent[bn].end()){
        return badRequest = {{"bn",bn},{"n",n},{"vs",vs},{"badRequest","n: " + n + " is not existing"}};
    }
    if (messageContent[bn][n].find(vs) == messageContent[bn][n].end()){
        return badRequest = {{"bn",bn},{"n",n},{"vs",vs},{"badRequest","vs: " + vs + " is not existing"}};
    }
    return std::nullopt;
}

