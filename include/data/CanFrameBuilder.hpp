#ifndef CAN_FRAME_BUILDER_HPP
#define CAN_FRAME_BUILDER_HPP
#include <iostream>
#include <map>
#include <unordered_map>
#include <string>
#include <fstream>
#include <bitset>

#include <linux/can.h>

#include "data/database.hpp"
#include "data/ecuJson.hpp"
#include "main.hpp"

#define JSON_KEY_NAME "Name"
#define JSON_KEY_CAN_ID "ID"
#define JSON_KEY_EXTENDED_CAN_ID "Extended ID"
#define JSON_KEY_CYCLE_TIME "Cycle Time"
#define JSON_KEY_IN_OUT "IN/OUT Frame"
#define JSON_KEY_ANALOG "Analog"
#define JSON_KEY_DIGITAL "Digital"
#define JSON_key_SIZE "Bit Size"
#define JSON_KEY_START_BIT "Start Bit"
#define JSON_KEY_BYTE_INVERTED "Byte Inverted"
#define JSON_KEY_SYNC_FRAME "Sync Frame"
#define JSON_KEY_DATA "Data"
#define JSON_KEY_BUS_NR "Can Bus"
#define JSON_KEY_CAN0 "Can 0"
#define JSON_KEY_CAN1 "Can 1"
#define JSON_KEY_CAN2 "Can 2"
#define JSON_KEY_CAN3 "Can 3"


#define CAN_INIT_FRAME_FILE_PATH (std::string(JSON_FOLDER_PATH) + "CanInitFrames.json")


using namespace std;

struct analog_data_t{
    size_t name;
    uint8_t startBit;
    uint8_t size;
    bool byteInverted;
    uint64_t value;
};

struct digital_data_t{
    size_t name;
    uint8_t startBit;
    bool value;
};

struct can_frame_t{
    size_t name;
    uint32_t CanBusID;
    bool extendedID;
    size_t syncFrame; 
    uint8_t frameSize;                                   
    unordered_map<std::size_t, analog_data_t> AnalogData;
    unordered_map<std::size_t, digital_data_t> DigitalData;
    #ifdef DEBUG
        //? print the can frame for debugging
        void print();
    #endif
}; 



enum in_out_t{
    IN=0,
    OUT=1
};

class CanFrameBuilder{
public:
    CanFrameBuilder(const uint8_t canPortNr);
    CanFrameBuilder(const uint8_t canPortNr, const string initFilePath);
    can_frame_t& buildReceiveCanFrame(const can_frame readFrame);
    can_frame_t& buildTransmitCanFrame(size_t frameName);
    can_frame convertToRawFrame(can_frame_t& frame);
    vector<can_frame>& buildInitFrames();

     
private:

    void initBuilder();
    void addInitFrame();
    void addFrame(nlohmann::json& frame);
    void checkForDataOverlapping(const can_frame_t& frame, string inOut);

    hash<string> stringHash; // hash function for strings
    

    string initFilePath;
    uint8_t canPortNr;

    vector<can_frame> initFrames;
    unordered_map<uint32_t, can_frame_t> canReceiveFrames;
    unordered_map<size_t,  vector<tuple<uint8_t, string, uint8_t, int16_t>>> sortedAnalogPins;
    unordered_map<size_t, can_frame_t> canTransmitFrames;
};


#endif // CAN_FRAME_BUILDER_HPP