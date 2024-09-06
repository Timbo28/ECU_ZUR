#include "system/throttleLogic.hpp"
#include "Profiling.hpp"

#define THROTTLE_UPPER_MARGIN 20
#define THROTTLE_LOWER_MARGIN 300


enum COMPARE_TYPE_t{
    GREATER,
    EQUAL,
    IN_RANGE_PERCENTAGE
};

/**
 * @brief Compare two values based on a specified comparison type.
 *
 * This function compares two values based on the specified comparison type
 * and throws a runtime error if the comparison condition is not met.
 *
 * @param compareValue1         The first value to compare.
 * @param type                  The comparison type (GREATER, EQUAL, IN_RANGE_PERCENTAGE).
 * @param compareValue2         The second value to compare.
 * @param tolerancePercentage   The tolerance percentage for IN_RANGE_PERCENTAGE comparison.
 *
 * @throw std::runtime_error    Thrown if the comparison condition is not met.
 */
void compareIf(double compareValue1, COMPARE_TYPE_t type, double compareValue2, double tolerancePercentage = 5){ PROFILING_FUNCTION();


    switch (type) {
        case GREATER:
            if (compareValue1 >= (compareValue2)){
                throw std::runtime_error("is not greater");
            }

            break;
        case EQUAL:
            if(compareValue1 == compareValue2){
                throw std::runtime_error("is equal");
            }
            break;
        case IN_RANGE_PERCENTAGE:
            if(!(compareValue1 > (compareValue2 - tolerancePercentage) &&
                 compareValue1 < (compareValue2 + tolerancePercentage))){
                throw std::runtime_error("is out of range");
            }
            break;
    }
}

/**
 * @brief Check for deviation between left and right acceleration sensors.
 *
 * This function checks if the deviation between left and right acceleration sensors
 * is within the specified tolerance range. It throws a runtime error if the deviation is
 * outside the allowed range.
 *
 * @param percentageLeft    Percentage value from the left sensor.
 * @param percentageRight   Percentage value from the right sensor.
 *
 * @throw std::runtime_error Thrown if the deviation is outside the allowed range.
 */
void checkSensorDeviation(double percentageLeft, double percentageRight) { PROFILING_FUNCTION();
    const int16_t APPSTolerance = 220;
    try {
        compareIf(percentageRight, IN_RANGE_PERCENTAGE, percentageLeft, APPSTolerance);

    } catch (const std::exception& exception) {
        throw ecu::EcuException(PEDAL_DEVIATION_ERROR,"Deviation between Acceleration sensors");
    }

}

/**
 * @brief Check for a shortage to the Vdc line.
 *
 * This function checks if any of the pedal position values (throttle) is greater
 * than or equal to the Vdc value. It throws a runtime error if any of these values
 * exceeds the Vdc limit.
 *
 * @param data Pointer to the pedal_box_t structure containing pedal position data.
 *
 * @throw std::runtime_error Thrown if a shortage to the Vdc line is detected.
 */
void shortageToVdc(const pedal_t *data) { PROFILING_FUNCTION();
    const short vdc = 1024;
    try {
        compareIf(data->throttleRightMax, GREATER, vdc);
        compareIf(data->throttleRightMin, GREATER, vdc);
        compareIf(data->throttleRightValue, GREATER, vdc);

    } catch (const std::exception& exception) {
        throw  ecu::EcuException(PEDAL_SHORTAGE_RIGHT_ERROR,"Right side shortage to Vdc line");
    }

    try {
        compareIf(data->throttleLeftMax, GREATER, vdc);
        compareIf(data->throttleLeftMin, GREATER, vdc);
        compareIf(data->throttleLeftValue, GREATER, vdc);

    } catch (const std::exception& exception) {
        throw  ecu::EcuException(PEDAL_SHORTAGE_LEFT_ERROR, "Left side shortage to Vdc line");
    }

}

/**
 * @brief Check for a shortage to the ground line.
 *
 * This function checks if all of the pedal position values (throttle) are equal to zero,
 * indicating a shortage to the ground line. It throws a runtime error if this condition is met.
 *
 * @param data Pointer to the pedal_box_t structure containing pedal position data.
 *
 * @throw std::runtime_error Thrown if a shortage to the ground line is detected.
 */
void shortageToGround(const pedal_t *data) { PROFILING_FUNCTION();
    const short ground = 0;
    try {
        compareIf(data->throttleLeftMax, EQUAL, ground);
        compareIf(data->throttleLeftMin, EQUAL, ground);
        compareIf(data->throttleLeftValue, EQUAL, ground);

    } catch (const std::exception& exception) {
        throw  ecu::EcuException(PEDAL_SHORTAGE_LEFT_ERROR,"Left side shortage to Ground Line");
    }

    try {
        compareIf(data->throttleRightMax, EQUAL, ground);
        compareIf(data->throttleRightMin, EQUAL, ground);
        compareIf(data->throttleRightValue, EQUAL, ground);

    } catch (const std::exception& exception) {
        throw  ecu::EcuException(PEDAL_SHORTAGE_RIGHT_ERROR ,"Right side shortage to Ground Line");
    }
}

/**
 * @brief Check for a shortage between the left and right pedal lines.
 *
 * This function checks if the pedal position values on the left and right sides are equal.
 * It throws a runtime error if this condition is met, indicating a shortage between the lines.
 *
 * @param data Pointer to the pedal_box_t structure containing pedal position data.
 *
 * @throw std::runtime_error Thrown if a shortage between the lines is detected.
 */
void shortageBetweenLines(const pedal_t *data){ PROFILING_FUNCTION();
    try {
        compareIf(data->throttleLeftValue, EQUAL, data->throttleRightValue);

    } catch (const std::exception& exception) {
        throw  ecu::EcuException(PEDAL_SHORTAGE_LEFT_TO_RIGHT_ERROR,"shortage between value lines");
    }

    try {
        compareIf(data->throttleLeftMax, EQUAL, data->throttleRightMax);

    } catch (const std::exception& exception) {
        throw  ecu::EcuException(PEDAL_SHORTAGE_LEFT_TO_RIGHT_ERROR,"shortage between max lines");
    }

    try {
        compareIf(data->throttleLeftMin, EQUAL, data->throttleRightMin);

    } catch (const std::exception& exception) {
        throw  ecu::EcuException(PEDAL_SHORTAGE_LEFT_TO_RIGHT_ERROR,"shortage between min lines");
    }
}


void checkForShortage(PedalBox *pedalBox) { PROFILING_FUNCTION();
    shortageToVdc(pedalBox->getPedalData());
    shortageToGround(pedalBox->getPedalData());
    shortageBetweenLines(pedalBox->getPedalData());
}


uint16_t calculateThrottle(PedalBox *pedalBox) { PROFILING_FUNCTION();
    static float integralError = 0;
    int16_t throttle;

    pedal_t data = *pedalBox->getPedalData();
    
    int rangeLeft = data.throttleLeftMax - data.throttleLeftMin;
    int rangeRight = data.throttleRightMax - data.throttleRightMin;
    double percentageRight = (1000.0 * (data.throttleRightValue - data.throttleRightMin) / rangeRight);
    double percentageLeft = (1000.0 * (data.throttleLeftValue - data.throttleLeftMin) / rangeLeft);
    

    
    
    if (percentageLeft < -10 || percentageLeft > 1000 ){
        throw ecu::EcuException(PEDAL_LEFT_OUT_OF_RANGE_ERROR,"Left potentiometer percentage out of range.");
    }
    if (percentageRight < -10 || percentageRight > 1000 ){
        throw ecu::EcuException(PEDAL_RIGHT_OUT_OF_RANGE_ERROR,"Right potentiometer percentage out of range.");
    }

    

    checkSensorDeviation(percentageLeft,percentageRight);

    
    throttle = (percentageLeft + percentageRight)/2;
    
    //Throttle pedal offset removal
    float pedalCorrection = (1000-THROTTLE_LOWER_MARGIN-THROTTLE_UPPER_MARGIN)/1000.0;
    throttle =1000 - (throttle-THROTTLE_UPPER_MARGIN)/pedalCorrection;
    
   
    if(throttle > 1000||throttle < 0){
        if(throttle < 1400 && throttle > 1000){
            throttle=1000;
        }else if(throttle > -450){
            throttle = 0;
        }else{
           throw ecu::EcuException(PEDAL_OUT_OF_RANGE_ERROR, "Pedal is out of range. More than 100% throttle or less than 0%.");
     
        }
    }
    //? PI Regler for powerlimitation
//?    if(pedalBox->getAccuManagementData()->accuPowerLimit) {
//?        int16_t error = pedalBox->getAccuManagementData()->accuPower - pedalBox->getSettings()->maxPower; 
//?
//?        float pTerm = pedalBox->getSettings()->powerLimit_kp   * error;
//?        integralError += error* (ECU_CYCLE_TIME/1000.0);
//?        float iTerm = pedalBox->getSettings()->powerLimit_ki * integralError;
//?
//?        throttle -= (pTerm + iTerm);
//?        if(throttle < 0) throttle = 0; 
//?        if(throttle > 1000) throttle = 1000;
//?        if(integralError > 500) integralError = 500;
//?        if(integralError < -500) integralError = -500;
//?
//?    }else{
//?        integralError = 0;
//?    }

    //? powerlimitation quickfix
    static bool powerDerating = false;
    if(pedalBox->getAccuManagementData()->accuPower >= pedalBox->getSettings()->maxPower && !powerDerating){
       // pedalBox->getSettings()->motorLoad[0] = 5;
       // pedalBox->getSettings()->motorLoad[1] = 5;
        powerDerating = true;
    }else if(throttle <= 50 && powerDerating){
       // pedalBox->getSettings()->motorLoad[0] = 60;
       // pedalBox->getSettings()->motorLoad[1] = 60;
        powerDerating = false;
    }
    
    
    //ECU_INFO("Throttle: {}", throttle);
    return throttle;
}