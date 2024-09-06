#ifndef CAN_STRUCT_MAPPER_HPP
#define CAN_STRUCT_MAPPER_HPP

#include <nlohmann/json.hpp>
#include <unordered_map>
#include <unordered_set>
#include "boost/pfr.hpp"
#include <iostream>

#include "data/database.hpp"
#include "data/CanFrameBuilder.hpp"
#include "EcuException.hpp"
#include "main.hpp"

#define MAPPING_FILE_PATH (std::string(JSON_FOLDER_PATH) + "CanMapping.json")


class CanStructMapper {
public:
    CanStructMapper();
    CanStructMapper(std::string path);


    template <typename T>
    void registerStruct(T& structToRegister){
        try{
            loopStruct(structToRegister);
            updateMappingFile();
        }catch(const ecu::EcuException& e){
            ECU_ERROR(e.getErrorCode(),"Error in registering struct: {}", e.what());
        }catch (const std::exception& exception) {
        ECU_ERROR(ERROR_IS_NOT_EXISTING,"{}",exception.what());
        }
    }

    void mapInputCanFrame(const can_frame_t& canFrame);
    void mapOutputCanFrame(can_frame_t& canFrame);
    std::unordered_map<std::size_t, uint8_t>& getFrameCanMap();


    
private:
    template <typename T>
    void loopStruct(T& Struct){
        auto names = boost::pfr::names_as_array<T>();
        boost::pfr::for_each_field(Struct, [this,&names](auto& field, std::size_t index) {
            if constexpr (std::is_aggregate_v<std::decay_t<decltype(field)>>)  {
                this->loopStruct(field);
            } else {
                std::string name = std::string(names[index]);
                if(structNames.find(name) == structNames.end()){
                    structNames.emplace(name);
                    if (canNames.find(name) != canNames.end()) {
                        in_out_t inOut = canNames[name];
                        switch (inOut) {
                            case in_out_t::IN:
                                structMapIn.emplace(std::hash<std::string>{}(name), &field);
                                break;
                            case in_out_t::OUT:
                                structMapOut.emplace(std::hash<std::string>{}(name),&field);
                        }
                    }
                }else{
                    throw ecu::EcuException(STRUCT_DUPLICATE_ERROR,"Duplicate element in structs. Element: '" + name + "' is existing twice in the database.hpp" );
                }
            }
        });
    }

    void loadCanFrameToStruct(const can_frame_t& canFrame);
    void clearMappingJson();
    void readCanFrames();
    void updateMappingFile();
    void uniquePushBack(std::unordered_map<std::string,in_out_t>& map, const std::string& element, in_out_t direction);
    bool checkForMissingElements(std::string name);


    std::unordered_set<std::string> structNames;// TODO delete after init
    std::unordered_map<std::string,in_out_t> canNames;// TODO delete after init

    std::unordered_map<std::size_t, uint8_t> frameCanMap;// stores which frame needs to be send on which can port key = hashed frame name, value = can port
    std::unordered_map<std::size_t, void*> structMapIn;// stores the pointer to the struct element key = hashed struct name, value = pointer to struct element
    std::unordered_map<std::size_t, void*> structMapOut;// stores the pointer to the struct element key = hashed struct name, value = pointer to struct element

    std::vector<can_frame_t> syncBuffer; // Buffer for frames that need to be loaded to the struct at the same time as another frame
    
    std::string canInitFilePath;

};


#endif // CAN_STRUCT_MAPPER_HPP