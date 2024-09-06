#include "system/arrayCompareLogic.hpp"
#include "Profiling.hpp"
#include <boost/pfr.hpp>
#include <string>
#include "data/database.hpp"
#include "log.hpp"
#include "Error.hpp"
#include "main.hpp"

template <typename T>
void loopStruct(T& Struct){
    auto names = boost::pfr::names_as_array<T>();
    boost::pfr::for_each_field(Struct, [&names](auto& field, std::size_t index) {
        if constexpr (std::is_aggregate_v<std::decay_t<decltype(field)>>)  {
                loopStruct(field);
        } else {
            
            //check if the name has "_" in it
            std::array<std::string_view,4> position = {"_FL", "_FR", "_RL", "_RR"};
            if (names[index].find(position[0]) != std::string::npos){
                //check if the next 3 elements have _FR, _RL, _RR
                for(int i = 1; i < 4; i++){
                    if (names[index+i].find(position[i]) == std::string::npos){
                        auto found = names[index].find(position[0]);
                        std::string ErrorVariableName = std::string(names[index]).erase(found,position[0].length());
                        throw std::runtime_error("Element '" + ErrorVariableName + "' is not in the correct order. Check the database.hpp file. Order needs to be _FL, _FR, _RL, _RR");
                    }
                }
            }
        }      
    });
}

void checkStructArrayOrder(){
    //TODO check if all elements are bool or int16 or uint16
    // Check if the array is ordered correctly
    system_data_t systemData;
    sensor_data_t sensorData;
    // loop through all names
    try{
        loopStruct(systemData);
    }catch(const std::exception& e){
        ECU_ERROR(STRUCT_ORDER_ERROR,"Error in system_data_t struct: {}", e.what());
    }

    try{     
        loopStruct(sensorData); 
    }catch(const std::exception& e){
        ECU_ERROR(STRUCT_ORDER_ERROR,"Error in sensor_data_t struct: {}", e.what());
    }
}

bool allElementsTrueM(const bool *array,  const bool* mask,int size){ PROFILING_FUNCTION();
    for (int i = 0; i < size; ++i) {
        if (!array[i] && mask[i]) return false;
    }
    return true;
}

bool allElementsTrue(const bool *array, int size){ PROFILING_FUNCTION();
    for (int i = 0; i < size; ++i) {
        if (!array[i]) return false;
    }
    return true;
}

bool allElementsFalseM(const bool *array,  const bool* mask,int size){ PROFILING_FUNCTION();
    for (int i = 0; i < size; ++i) {
        if (array[i]&&mask[i]) return false;
    }
    return true;
}

bool allElementsFalse(const bool* array, int size){ PROFILING_FUNCTION();
    for (int i = 0; i < size; ++i) {
        if (array[i]) return false;
    }
    return true;
}
// Check if all elements in the array are greater than or equal to the value
bool allElementsGEM(const int16_t* firstElement, int value, const bool* mask, int size){ PROFILING_FUNCTION();
    for ( int i = 0; i < size; ++i) {
        if (firstElement[i] < value && mask[i]) return false;
    }
    return true;
    
}
// Check if all elements in the array are greater than or equal to the value
bool allElementsGE(const int16_t* firstElement, int value, int size){ PROFILING_FUNCTION();
    for ( int i = 0; i < size; ++i) {
        if (firstElement[i] < value) return false;
    }
    return true;
    
}
std::vector<int> getTrueElementPositionsM(const bool* array, const bool* mask, int size){ PROFILING_FUNCTION();
    std::vector<int> trueElements;
    for (int i = 0; i < size; ++i) {
        if (array[i] && mask[i]){
            trueElements.push_back(i);
        }
    }
    return trueElements;
}
std::vector<int> getTrueElementPositions(const bool* array, int size){ PROFILING_FUNCTION();
    std::vector<int> trueElements;
    for (int i = 0; i < size; ++i) {
        if (array[i]){
            trueElements.push_back(i);
        }
    }
    return trueElements;
}

std::vector<int> getFalseElementPositionsM(const bool* array,const bool* mask, int size){ PROFILING_FUNCTION();
    std::vector<int> falseElements;
    for (int i = 0; i < size; ++i) {
        if (!array[i] && mask[i]){
            falseElements.push_back(i);
        }
    }
    return falseElements;
}
std::vector<int> getFalseElementPositions(const bool* array, int size){ PROFILING_FUNCTION();
    std::vector<int> falseElements;
    for (int i = 0; i < size; ++i) {
        if (!array[i]){
            falseElements.push_back(i);
        }
    }
    return falseElements;
}