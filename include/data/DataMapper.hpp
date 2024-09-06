#ifndef DATAMAPPER_HPP
#define DATAMAPPER_HPP

#include <nlohmann/json.hpp>
#include <unordered_map>
#include <unordered_set>
#include "boost/pfr.hpp"

#include "data/database.hpp"
#include "EcuException.hpp"
#include "main.hpp"
#include <iostream>

#define REGISTER_4_STRUCTS 4
#define REGISTER_3_STRUCTS 3
#define REGISTER_2_STRUCTS 2
#define REGISTER_1_STRUCT 1

struct message_content_t{
    nlohmann::json senML;
    std::vector<void*> pointer;
    std::vector<uint8_t> size; //size of the pointer in bytes
    std::vector<bool> isFloat;
    std::vector<bool> readOnly;
};

struct struct_data_t{
    void* pointer;
    uint8_t size;
    bool isFloat;
    bool readOnly;
};

class DataMapper {
public:
    DataMapper(uint8_t nrStructsToBeRegistered = 1);
    DataMapper(std::string path, uint8_t nrStructsToBeRegistered = 1);


    template <typename T>
    void registerStruct(T& structToRegister, bool readOnly = false){
        registeredStructs++;
        try{
            loopStruct(structToRegister, readOnly);
            if(registeredStructs >= nrStructsToBeRegistered){
                readMessageContentFile();
            }
        }catch(const ecu::EcuException& e){
            ECU_ERROR(e.getErrorCode(),"Error in registering struct: {}", e.what());
        }catch (const std::exception& exception) {
        ECU_ERROR(ERROR_IS_NOT_EXISTING,"{}",exception.what());
        }
    }


nlohmann::json handelDataRequest(std::string bn, std::string n, std::string vs);
nlohmann::json getSenML(nlohmann::json& requestData);

nlohmann::json handelControlRequest (std::string bn, std::string n, std::string vs, nlohmann::json& controlRequest);

private:
template <typename T>
void loopStruct(T& Struct, bool readOnly){
    bool fieldIsFloat;
    auto names = boost::pfr::names_as_array<T>();
    boost::pfr::for_each_field(Struct, [this, &names, readOnly, &fieldIsFloat](auto& field, std::size_t index) {
        if constexpr (std::is_aggregate_v<std::decay_t<decltype(field)>>)  {
            this->loopStruct(field, readOnly);
        } else {
            std::string name = std::string(names[index]);
            if(data.find(name) == data.end()){
                // check if field is float/double 
                if(typeid(field) == typeid(float)){
                    fieldIsFloat = true;
                }else if(typeid(field) == typeid(double)){
                    fieldIsFloat = true;
                }else{
                    fieldIsFloat = false;
                }
                data.emplace(name, struct_data_t{&field,sizeof(field),fieldIsFloat, readOnly});
            } else {
                throw ecu::EcuException(STRUCT_DUPLICATE_ERROR, "Duplicate element in structs. Element: '" + name + "' is existing twice in the database.hpp");
            }
        }
    });
}

    void readMessageContentFile();
    std::optional<nlohmann::json> requestValidation(std::string bn, std::string n, std::string vs);

    std::string configFilePath;
    uint8_t nrStructsToBeRegistered;
    uint8_t registeredStructs = 0;
    // TODO delete after init
    std::unordered_map<std::string, struct_data_t> data; // stores the pointer to the single variables in the registered structs


    std::unordered_map< std::string, 
                    std::unordered_map<std::string, 
                    std::unordered_map<std::string, message_content_t>>> messageContent;


};



#endif // DATAMAPPER_HPP