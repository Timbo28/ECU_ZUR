#include <functional>
#include <stdexcept> 

#include "data/CanFrameBuilder.hpp"

#ifdef DEBUG
    #include <bitset>
    void can_frame_t::print(){ PROFILING_FUNCTION();
        cout << "------------------------------------------------------------------------\n";
        cout << "can frame name: " << name << endl;
        cout << "can frame id: " << hex << CanBusID << dec << endl;
        cout << "sync frame: " << syncFrame << endl;
        cout << "Analog Data:\n";
        for (auto &i : AnalogData){
            cout << "name: " << i.second.name <<  ", value: " << i.second.value << endl;
        }
        cout << "Digital Data:\n";
        for (auto &i : DigitalData){
            cout << "name: " << i.second.name <<", value: " << static_cast<bool>(i.second.value) << endl;
        }
        cout << "------------------------------------------------------------------------\n";

    };


 
#endif

CanFrameBuilder::CanFrameBuilder(const uint8_t canPortNr)
    : CanFrameBuilder(canPortNr, CAN_FRAME_FILE_PATH){ PROFILING_FUNCTION();
}

CanFrameBuilder::CanFrameBuilder(const uint8_t canPortNr, const string initFilePath){ PROFILING_FUNCTION();
    this->canPortNr = canPortNr;
    this->initFilePath = initFilePath;
    try{
        initBuilder();
    }catch(const ecu::EcuException& e){
        ECU_ERROR(e.getErrorCode(),"Could not init CAN{} Frame Builder: {}", canPortNr, e.what());
    }
    
}

void CanFrameBuilder::initBuilder(){ PROFILING_FUNCTION();
    nlohmann::json can;
    nlohmann::json initData = ecu::getJsonFile(initFilePath);
    switch (canPortNr){
        case 0:
            can = ecu::getJsonValue<nlohmann::json>(initData,JSON_KEY_CAN0);
            break;
        case 1:
            can = ecu::getJsonValue<nlohmann::json>(initData,JSON_KEY_CAN1);
            break;
        case 2:
            can = ecu::getJsonValue<nlohmann::json>(initData,JSON_KEY_CAN2);
            break;
        case 3:
            can = ecu::getJsonValue<nlohmann::json>(initData,JSON_KEY_CAN3);
            break;
        default:
            ECU_ERROR(CAN_IS_NOT_EXISTING,"CAN port number not found in the CanFrames.json file");
            break;
    }
    try{
        for (size_t i = 0; i < can.size(); i++){
            addFrame(can[i]);
        }
        addInitFrame();
    }catch(const std::exception& e){
        ECU_ERROR(CAN0_INIT_FRAME_BUILDER_ERROR+canPortNr,"Could not init CAN{} Frame Builder: {}", canPortNr, e.what());
    }
}

void CanFrameBuilder::addInitFrame(){ PROFILING_FUNCTION();
    nlohmann::json initFramesFile = ecu::getJsonFile(CAN_INIT_FRAME_FILE_PATH);
    can_frame initFrame;


    // loop through the initFramesFile and add the frame to the initFrames vector
    for (auto& frame : initFramesFile){
        if(ecu::getJsonValue<uint8_t>(frame,JSON_KEY_BUS_NR) == canPortNr){
            initFrame.can_id = std::stoi(ecu::getJsonValue<string>(frame,JSON_KEY_CAN_ID), nullptr, 16); // string to hex number
            nlohmann::json data = ecu::getJsonValue<nlohmann::json>(frame,JSON_KEY_DATA);
            initFrame.can_dlc = data.size();
            uint8_t i = 0;
            for (auto& byte:data){
                try{
                    std::string byteStr = byte.get<std::string>();
                    if (byteStr.substr(0, 2) == "0b") {
                        initFrame.data[i] = std::stoi(byteStr.substr(2), nullptr, 2);
                    } else if(byteStr.substr(0, 2) == "0x"){
                        initFrame.data[i] = std::stoi(byteStr.substr(2), nullptr, 16);
                    }else{
                        initFrame.data[i] = std::stoi(byteStr, nullptr, 10);
                    }
                    i++;
                }catch(const std::exception& e){
		            std::string hexID = ecu::decToHexStr(initFrame.can_id); 
                    ECU_ERROR(CAN0_INIT_FRAME_BUILDER_ERROR+canPortNr,"Could not init CAN{} Frame Builder: was not able to read init frame with ID = '{}' from CanInitFrames.json file", canPortNr, hexID);
                }
            }
            initFrames.push_back(initFrame);
        }
        
    }
}

void CanFrameBuilder::addFrame(nlohmann::json& frame){ PROFILING_FUNCTION();
    can_frame_t canFrameTemplate;

        canFrameTemplate.name = stringHash(ecu::getJsonValue<string>(frame,JSON_KEY_NAME));
        canFrameTemplate.CanBusID = std::stoi(ecu::getJsonValue<string>(frame,JSON_KEY_CAN_ID), nullptr, 16); // string to hex number
        
        if(frame.find(JSON_KEY_EXTENDED_CAN_ID) == frame.end()){
            canFrameTemplate.extendedID = false;
        }else{
            canFrameTemplate.extendedID = ecu::getJsonValue<bool>(frame,JSON_KEY_EXTENDED_CAN_ID);
        }

        // check if the frame is a sync frame
        auto it = frame.find(JSON_KEY_SYNC_FRAME);
        if (it != frame.end()){
            canFrameTemplate.syncFrame = stringHash(it->get<string>());
        }else{
            canFrameTemplate.syncFrame = 0;
        }
        
        uint8_t lastFrameBit=0;
        nlohmann::json analogData = frame[JSON_KEY_ANALOG];   
        for (size_t j = 0; j < analogData.size(); j++){
            analog_data_t analogValue;
            analogValue.name = stringHash(ecu::getJsonValue<string>(analogData[j],JSON_KEY_NAME));
            analogValue.startBit = ecu::getJsonValue<uint8_t>(analogData[j],JSON_KEY_START_BIT);
            analogValue.size = ecu::getJsonValue<uint8_t>(analogData[j],JSON_key_SIZE);//TODO '-1' is working because start bit + size != last bit it is one to big maybe rework
            analogValue.value = 0;
            
            if(analogData[j].find(JSON_KEY_BYTE_INVERTED) == analogData[j].end()){
                analogValue.byteInverted = false;
            }else{
                analogValue.byteInverted = ecu::getJsonValue<bool>(analogData[j],JSON_KEY_BYTE_INVERTED);
            }
            
            canFrameTemplate.AnalogData[analogValue.name] = analogValue; 
            if(analogValue.startBit + analogValue.size-1 > lastFrameBit){
                lastFrameBit = analogValue.startBit + analogValue.size-1;
            }
        }  
        nlohmann::json digitalData = frame[JSON_KEY_DIGITAL];
        for (size_t j = 0; j < digitalData.size(); j++){
            digital_data_t digitalValue;
            digitalValue.name = stringHash(ecu::getJsonValue<string>(digitalData[j],JSON_KEY_NAME));
            digitalValue.startBit = ecu::getJsonValue<uint8_t>(digitalData[j],JSON_KEY_START_BIT);
            digitalValue.value = false;
            canFrameTemplate.DigitalData[digitalValue.name] = digitalValue;
            if(digitalValue.startBit > lastFrameBit){
                lastFrameBit = digitalValue.startBit;
            }
        }
        if(lastFrameBit > 63){
            auto strFrameName = ecu::getJsonValue<string>(frame,JSON_KEY_NAME);
            ECU_ERROR(CAN0_INIT_FRAME_BUILDER_ERROR+canPortNr,"Could not init CAN{} Frame Builder: The frame '{}' has a size of {} bits, which is too big", canPortNr, strFrameName , lastFrameBit);
        }else{
            canFrameTemplate.frameSize = (lastFrameBit/8) + 1;
        }
        
        
        #ifdef DEBUG
            std::cout << "Added frame to the CanFrameBuilder:";
            canFrameTemplate.print();            //?only for debugging
        #endif
        // check if the frame is an input or output frame
        auto inOut = ecu::getJsonValue<string>(frame,JSON_KEY_IN_OUT);
        checkForDataOverlapping(canFrameTemplate, inOut);
        if ( inOut == "IN"){
            canReceiveFrames[canFrameTemplate.CanBusID] = canFrameTemplate;
        }else if( inOut == "OUT"){
             canTransmitFrames[canFrameTemplate.name] = canFrameTemplate;
        }else{
            ECU_ERROR(CAN0_INIT_FRAME_BUILDER_ERROR+canPortNr,"Could not init CAN{} Frame Builder: The '{}' key of the frame '{}' is not 'IN' or 'OUT' ", canPortNr , JSON_KEY_IN_OUT, it->get<string>() );
        }
        
}




void CanFrameBuilder::checkForDataOverlapping(const can_frame_t& frame, string inOut){ PROFILING_FUNCTION();
    uint64_t overlapMap = 0;
    uint64_t dataPosition;
    uint64_t shiftVar;
    for (auto &analogValue : frame.AnalogData){
        shiftVar = 0;
        dataPosition = ~(~shiftVar << analogValue.second.size); // max bit size of analog data = 16 bit 
        dataPosition = dataPosition << analogValue.second.startBit;
        if(overlapMap & dataPosition){
            throw std::invalid_argument("Overlapping Analog data detected. Check the frame with the ID 0x" + ecu::decToHexStr(frame.CanBusID) + " in the " + CAN_FRAME_FILE_PATH + " file");
        }
        overlapMap |= dataPosition;
    }
    for (auto &digitalValue : frame.DigitalData){
        shiftVar = 0b1;
        dataPosition = shiftVar << digitalValue.second.startBit;
        if(overlapMap & dataPosition){
            throw std::invalid_argument("Overlapping Digital data detected. Check the frame with the ID 0x" + ecu::decToHexStr(frame.CanBusID) + " in the " + CAN_FRAME_FILE_PATH + " file");
        }
        overlapMap |= dataPosition;
    }
   
}


vector<can_frame>& CanFrameBuilder::buildInitFrames(){PROFILING_FUNCTION();
    return initFrames;
}

can_frame_t& CanFrameBuilder::buildReceiveCanFrame(const can_frame readFrame){ PROFILING_FUNCTION();
    auto it = canReceiveFrames.find(readFrame.can_id);
    if( it == canReceiveFrames.end()){
        throw std::runtime_error("Frame name of the received frame not found. Check the name in the CanFrames.json file");
    }
    can_frame_t& canFrame = it->second;
    int64_t message = 0;
    int64_t analogResult = 0;
    for (size_t i = 0; i < readFrame.can_dlc; i++) {
		message = message + (static_cast<uint64_t>(readFrame.data[i]) << (i*8));
	}
    #ifdef DEBUG
        cout << "Frame builder read message in binary: " << bitset<sizeof(message) * 8>(message) << endl;     //?only for debugging
    #endif
    for (auto &digitalValue : canFrame.DigitalData){
        digitalValue.second.value = (message >> digitalValue.second.startBit) & 0b1;
    }
    // update the analog data in the can frame with the sorted analog
    for (auto &analogValue : canFrame.AnalogData){

        analogResult = (message >> analogValue.second.startBit) & (~(0xFFFFFFFFFFFFFFFF << analogValue.second.size));
        
        if(analogValue.second.byteInverted){
            analogResult = __builtin_bswap64(analogResult);
            analogResult = analogResult >> (64 - analogValue.second.size);
            
        }

        analogValue.second.value = analogResult;      
    }
    
    return canFrame;
}


can_frame_t& CanFrameBuilder::buildTransmitCanFrame(size_t frameName){ PROFILING_FUNCTION();
    auto it = canTransmitFrames.find(frameName);
    if (it == canTransmitFrames.end()) {
        // Frame name not found
        throw std::runtime_error("Frame name of the transmission frame not found. Check the name in the CanFrames.json file");
    }

    // Frame name found
    can_frame_t& transmitFrame = it->second;
    return transmitFrame; 
}

can_frame CanFrameBuilder::convertToRawFrame(can_frame_t& frame){ PROFILING_FUNCTION();
    can_frame rawFrame;
    uint64_t message = 0;
    uint64_t analogResult = 0;

    rawFrame.can_id = frame.CanBusID;
    if(frame.extendedID){
        rawFrame.can_id |= CAN_EFF_FLAG;
    }
    rawFrame.can_dlc = frame.frameSize;

    for (auto &analogValue : frame.AnalogData){

        analogResult = analogValue.second.value;
        
        if(analogValue.second.byteInverted){
            analogResult = __builtin_bswap64(analogResult);
            analogResult = analogResult >> (64 - analogValue.second.size);
        }
        message |= analogResult << analogValue.second.startBit;
        
    }
    for (auto &digitalPin : frame.DigitalData){
        message |= (static_cast<uint64_t>(digitalPin.second.value) << digitalPin.second.startBit);
    }

    // convert 64 bit message to 8 byte can frame
    for (uint8_t i = 0; i < 8; i++){
        rawFrame.data[i] = (message >> (i*8)) & 0xFF;
    }


    return rawFrame;
}

