    /*
  ______          _      _        _    _          _____    _____            _             
 |___  /         (_)    | |      | |  | |  /\    / ____|  |  __ \          (_)            
    / /_   _ _ __ _  ___| |__    | |  | | /  \  | (___    | |__) |__ _  ___ _ _ __   __ _ 
   / /| | | | '__| |/ __| '_ \   | |  | |/ /\ \  \___ \   |  _  // _` |/ __| | '_ \ / _` |
  / /_| |_| | |  | | (__| | | |  | |__| / ____ \ ____) |  | | \ \ (_| | (__| | | | | (_| |
 /_____\__,_|_|  |_|\___|_| |_|   \____/_/    \_\_____/   |_|  \_\__,_|\___|_|_| |_|\__, |
                                                                                     __/ |
                                                	                                |___/ 
*/
/**
 * @file	main.c
 * @authors	Marco Rau (raumar02@students.zhaw.ch)
 * 			Tim Roos  (roostim1@students.zhaw.ch)
 * @brief 	electronic control unit
 * @version	v1.0.0
 * @date	2023-12-22
 * 
 * @copyright Copyright (c) 2023 Zurich UAS Racing
 * 
 */

#include "System.hpp"
#include "Data.hpp"
#include "LogManager.hpp"
#include "main.hpp"

int main() {
    

    LogManager::init();
    PROFILING_BEGIN_SESSION("RunMode");
    {

        ECU_INFO("ECU STARTED");
        Data data;
        System system(&data);

        while (!data.getSettings()->shutdown){
            auto startUpdateCycle = std::chrono::high_resolution_clock::now();
            data.update();
            system.update();
            
            
            std::this_thread::sleep_until(startUpdateCycle + std::chrono::milliseconds(ECU_CYCLE_TIME));
        }
    }
    PROFILING_END_SESSION();

    LogManager::shutdown();

    return 0;
}