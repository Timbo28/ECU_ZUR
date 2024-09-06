/**
 * @file    database.hpp
 * @authors Marco Rau (rauamr02@students.zhaw.ch)
 *          Tim Ross  (roostim1@students.zhaw.ch)
 * @brief   This file contains the declaration of the Database class, which manages all sensor values.
 * @version v1.0.0
 * @date    2023-12-22
 * 
 * @copyright Copyright (c) 2023 Zurich UAS Racing
 * 
 */

#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <cstdint>

#define INIT_SERVER_STATE   false
#define INIT_SERVER_PORT    8080

#define NO_IP   nullptr
#define NO_PORT -1

#define SETTINGS_PATH (std::string(JSON_FOLDER_PATH) +"settings.json")
#define CAN_FRAME_FILE_PATH (std::string(JSON_FOLDER_PATH) + "CanFrames.json")
#define SENML_CONFIG_FILE_PATH (std::string(JSON_FOLDER_PATH) + "senMLConfig.json")


enum DRIVERLESS_COMMANDS_t{
    DV_NO_DATA,
    DV_CONTROL,
    DV_AS,
    DV_EBS,
    DV_AMI,
    DV_LAPS,
    DV_CONES,
    DV_ACCELERATION
};



//*** system_data_t ***************************************************************************************************
struct CAN_data_out_t {
    // Inverter data
    int16_t out_targetTorque_FL {0};  /**< Target velocity in RPM for front left wheel */
    int16_t out_targetTorque_FR {0};  /**< Target velocity in RPM for front right wheel */
    int16_t out_targetTorque_RL {0};  /**< Target velocity in RPM for rear left wheel */
    int16_t out_targetTorque_RR {0};  /**< Target velocity in RPM for rear right wheel */
    
    int16_t out_torqueLimitPos {0};               /**< Positive torque limit in Nm */
    int16_t out_torqueLimitNeg {0};                /**< Negative torque limit in Nm */
    
    bool out_inverterEnable_FL {false}; /**< Flag to enable the front left inverter. */
    bool out_inverterEnable_FR {false}; /**< Flag to enable the front right inverter. */
    bool out_inverterEnable_RL {false}; /**< Flag to enable the rear left inverter. */
    bool out_inverterEnable_RR {false}; /**< Flag to enable the rear right inverter. */

    bool out_inverterOn_FL {false}; /**< Flag to turn on the front left inverter. */
    bool out_inverterOn_FR {false}; /**< Flag to turn on the front right inverter. */
    bool out_inverterOn_RL {false}; /**< Flag to turn on the rear left inverter. */
    bool out_inverterOn_RR {false}; /**< Flag to turn on the rear right inverter. */

    bool out_inverterDCon_FL {false}; /**< Flag to turn on the front left inverter DC. */
    bool out_inverterDCon_FR {false}; /**< Flag to turn on the front right inverter DC. */
    bool out_inverterDCon_RL {false}; /**< Flag to turn on the rear left inverter DC. */
    bool out_inverterDCon_RR {false}; /**< Flag to turn on the rear right inverter DC. */

    bool out_inverterErrorReset_FL {false}; /**< Flag to reset the error of the front left inverter. */
    bool out_inverterErrorReset_FR {false}; /**< Flag to reset the error of the front right inverter. */
    bool out_inverterErrorReset_RL {false}; /**< Flag to reset the error of the rear left inverter. */
    bool out_inverterErrorReset_RR {false}; /**< Flag to reset the error of the rear right inverter. */

    // cockpit data
    bool out_TSLed {false};          /**< Flag indicating the status of the TS LED. */
    bool out_RTDLed {false};         /**< Flag indicating the status of the RTD LED. */
    
    bool out_ASSIred {false};        /**< Flag indicating the status of the ASSI red LED. */
    bool out_ASSIgreen {false};      /**< Flag indicating the status of the ASSI green LED. */
    bool out_ASSIblue {false};       /**< Flag indicating the status of the ASSI blue LED. */
    bool out_breakLightOn {false};
    bool out_JetsonOn{true};

    // HVCB (High Voltage Control Box) data
    bool out_dischargeRelay {false}; /**< Discharge relay status */
    bool out_RTDsoundOn {false};     /**< Flag indicating if the RTD sound is on. */

    // Accumulator data
    bool out_prechargeRelay {false}; /**< Precharge relay status */
    bool out_AIRPositive {false};    /**< Positive AIR (Accumulator Isolation Relay)status */
    bool out_AIRNegative {false};    /**< Negative AIR (Accumulator Isolation Relay)status */

    // Steering
    int32_t out_steeringPosition {0}; /**< The position is int32 type, and the range -360000000-360000000 represents the position -36000°-36000° */
    int16_t out_steeringSpeed {0};  /**< The speed is int16 type, and the range -32768-32767 represents -327680-327680electrical speed */
    int16_t out_steeringAcceleration {0}; /**< The acceleration is int16 type, and the range 0-32767 represents 0-327670 electrical speed/s². 1 unit equals 10 electrical speed /s² */


};

struct led_button_box_t {
    bool TSButtonPressed {false};       /**< Flag indicating the status of the TS button. */
    bool RTDButtonPressed {false};      /**< Flag indicating the status of the RTD button. */
};

struct accu_management_box_t {
    bool accuPowerLimit {false}; /**< Flag indicating accumulator power limit status. */
    int16_t accuPower {0};           /**< Accumulator power in kW */
    int16_t averagePower {0};        /**< Average power over 500 ms in kW */
    int16_t accuVoltage {0};           /**< High voltage voltage in V */

    int16_t maxPower {0}; /**< Maximum power in kW */
    int16_t maxVoltage {0}; /**< Maximum voltage in V */
    int16_t maxCurrent {0}; /**< Maximum current in A */
};

struct pedal_box_t {
    bool APPSError {false};      /**< Flag indicating an error in the APPS (Accelerator Pedal Position Sensor). */
    uint16_t throttle {0};         /**< Current throttle position value in percentage. */

    bool breakPressed {false};   /**< Flag indicating if the break is pressed. */
    uint16_t breakPercentage {0};       /**< Break pressure in %. */
    bool breakError {false};     /**< Flag indicating an error in the break system. */
};

enum drive_mode_t {
    normalMode,
    slowMode,
    enduranceMode
};

enum motor_controller_state_t {
    DisableState,
    PrechargeState,
    HVCouplingState,
    ReadyToDriveState,
    DriveState,
    RecuperateState,
    OverheatState,
    InverterErrorResetState,
    ErrorState,
};

struct motor_controller_box_t {
    bool TSLed {false};          /**< Flag indicating the status of the TS LED. */
    bool RTDLed {false};         /**< Flag indicating the status of the RTD LED. */

    motor_controller_state_t currentState {DisableState}; /**< Current state of the motor controller. */
    drive_mode_t currentDriveMode {normalMode}; /**< Current drive mode. */

    int16_t maxRPM {0}; /**< Maximum RPM of all the motors. */
    int16_t maxRPM_FL {0}; /**< Maximum RPM of the front left motor. */
    int16_t maxRPM_FR {0}; /**< Maximum RPM of the front right motor. */
    int16_t maxRPM_RL {0}; /**< Maximum RPM of the rear left motor. */
    int16_t maxRPM_RR {0}; /**< Maximum RPM of the rear right motor. */

    int16_t maxTorque {0}; /**< Maximum torque of the motors. */
    int16_t maxTorque_FL {0}; /**< Maximum torque of the front left motor. */
    int16_t maxTorque_FR {0}; /**< Maximum torque of the front right motor. */
    int16_t maxTorque_RL {0}; /**< Maximum torque of the rear left motor. */
    int16_t maxTorque_RR {0}; /**< Maximum torque of the rear right motor. */

    int16_t maxMotorTemp {0}; /**< Maximum motor temperature of all the motors. */
    int16_t maxMotorTemp_FL {0}; /**< Maximum motor temperature of the front left motor. */
    int16_t maxMotorTemp_FR {0}; /**< Maximum motor temperature of the front right motor. */
    int16_t maxMotorTemp_RL {0}; /**< Maximum motor temperature of the rear left motor. */
    int16_t maxMotorTemp_RR {0}; /**< Maximum motor temperature of the rear right motor. */

    int16_t maxInverterTemp {0}; /**< Maximum inverter temperature of all the inverters. */
    int16_t maxInverterTemp_FL {0}; /**< Maximum inverter temperature of the front left inverter. */
    int16_t maxInverterTemp_FR {0}; /**< Maximum inverter temperature of the front right inverter. */
    int16_t maxInverterTemp_RL {0}; /**< Maximum inverter temperature of the rear left inverter. */
    int16_t maxInverterTemp_RR {0}; /**< Maximum inverter temperature of the rear right inverter. */

    int16_t maxIGBTTemp {0}; /**< Maximum IGBT temperature of all the inverters. */
    int16_t maxIGBTTemp_FL {0}; /**< Maximum IGBT temperature of the front left inverter. */
    int16_t maxIGBTTemp_FR {0}; /**< Maximum IGBT temperature of the front right inverter. */
    int16_t maxIGBTTemp_RL {0}; /**< Maximum IGBT temperature of the rear left inverter. */
    int16_t maxIGBTTemp_RR {0}; /**< Maximum IGBT temperature of the rear right inverter. */    
};


struct system_data_t {
    // CAN output data
    CAN_data_out_t CANOutput;
    // LED and button data
    led_button_box_t ledButtonBox;
    // BMS (Battery Management System) data
    accu_management_box_t accuManagementBox;
    // Pedal box data
    pedal_box_t pedalBox;
    // Motor controller data
    motor_controller_box_t motorControllerBox;
 };
 
 
//*** sensor_data_t ***************************************************************************************************



struct pedal_t
{
    uint16_t throttleLeftMax   {500};
    uint16_t throttleLeftMin   {400};
    uint16_t throttleLeftValue {484};

    uint16_t throttleRightMax   {200};
    uint16_t throttleRightMin   {100};
    uint16_t throttleRightValue {184};

    uint16_t breakPressureFront {150}; /**< Break pressure at the front of the car in bar */
    uint16_t breakPressureRear {150};  /**< Break pressure at the rear of the car in bar */
};

struct inverter_t
{   
    int16_t motorTemp_FL {0};   /**< Motor temperature in °C for front left */
    int16_t motorTemp_FR {0};   /**< Motor temperature in °C for front right */
    int16_t motorTemp_RL {0};   /**< Motor temperature in °C for rear left */
    int16_t motorTemp_RR {0};   /**< Motor temperature in °C for rear right */

    int16_t inverterTemp_FL {0};    /**< Inverter temperature in °C for front left */
    int16_t inverterTemp_FR {0};    /**< Inverter temperature in °C for front right */
    int16_t inverterTemp_RL {0};    /**< Inverter temperature in °C for rear left */
    int16_t inverterTemp_RR {0};    /**< Inverter temperature in °C for rear right */

    int16_t IGBTTemp_FL {0};    /**< IGBT temperature in °C for front left */
    int16_t IGBTTemp_FR {0};    /**< IGBT temperature in °C for front right */
    int16_t IGBTTemp_RL {0};    /**< IGBT temperature in °C for rear left */
    int16_t IGBTTemp_RR {0};    /**< IGBT temperature in °C for rear right */

    int16_t invertersVoltage_FL {0}; /**< Inverter voltage in V*/ 
    int16_t invertersVoltage_FR {0}; /**< Inverter voltage in V*/
    int16_t invertersVoltage_RL {0}; /**< Inverter voltage in V*/
    int16_t invertersVoltage_RR {0}; /**< Inverter voltage in V*/

    int16_t inverterActualRPM_FL {0}; /**< Actual RPM for front left */
    int16_t inverterActualRPM_FR {0}; /**< Actual RPM for front right */
    int16_t inverterActualRPM_RL {0}; /**< Actual RPM for rear left */
    int16_t inverterActualRPM_RR {0}; /**< Actual RPM for rear right */

    int16_t inverterActualTorque_FL {0}; /**< Actual torque in Nm for front left */
    int16_t inverterActualTorque_FR {0}; /**< Actual torque in Nm for front right */
    int16_t inverterActualTorque_RL {0}; /**< Actual torque in Nm for rear left */
    int16_t inverterActualTorque_RR {0}; /**< Actual torque in Nm for rear right */

    int16_t inverterErrorCode_FL {0}; /**< Error code for front left inverter */
    int16_t inverterErrorCode_FR {0}; /**< Error code for front right inverter */
    int16_t inverterErrorCode_RL {0}; /**< Error code for rear left inverter */
    int16_t inverterErrorCode_RR {0}; /**< Error code for rear right inverter */

    bool inverterSystemReady_FL {false}; /**< Flag indicating the status of the inverter system for front left */
    bool inverterSystemReady_FR {false}; /**< Flag indicating the status of the inverter system for front right */
    bool inverterSystemReady_RL {false}; /**< Flag indicating the status of the inverter system for rear left */
    bool inverterSystemReady_RR {false}; /**< Flag indicating the status of the inverter system for rear right */

    bool inverterDCon_FL {false}; /**< Flag indicating the status of the inverter DC for front left */
    bool inverterDCon_FR {false}; /**< Flag indicating the status of the inverter DC for front right */
    bool inverterDCon_RL {false}; /**< Flag indicating the status of the inverter DC for rear left */
    bool inverterDCon_RR {false}; /**< Flag indicating the status of the inverter DC for rear right */

    bool inverterOn_FL {false}; /**< Flag indicating the status of the inverter for front left */
    bool inverterOn_FR {false}; /**< Flag indicating the status of the inverter for front right */
    bool inverterOn_RL {false}; /**< Flag indicating the status of the inverter for rear left */
    bool inverterOn_RR {false}; /**< Flag indicating the status of the inverter for rear right */

    bool inverterError_FL {false}; /**< Error flag for front left inverter */
    bool inverterError_FR {false}; /**< Error flag for front right inverter */
    bool inverterError_RL {false}; /**< Error flag for rear left inverter */
    bool inverterError_RR {false}; /**< Error flag for rear right inverter */

    bool inverterDerating_FL {false}; /**< Derating flag for front left inverter */
    bool inverterDerating_FR {false}; /**< Derating flag for front right inverter */
    bool inverterDerating_RL {false}; /**< Derating flag for rear left inverter */
    bool inverterDerating_RR {false}; /**< Derating flag for rear right inverter */
};

struct accu_t
{
    int16_t accuStateOfCharge {0};          /**< Accumulator power in kW */
    int16_t accuTemperature {0};            /**< Accumulator temperature in °C */

    int16_t HVvoltage {0};          /**< High voltage voltage in V */
    int16_t HVcurrent {0};          /**< High voltage current in A */

    int32_t current {0};   /**< Current in mA*/
    

    bool prechargeRelay {false}; /**< Precharge relay status */ 
    bool AIRPositive {false};    /**< Positive AIR (Accumulator Isolation Relay)status */
    bool AIRNegative {false};    /**< Negative AIR (Accumulator Isolation Relay)status */
};

struct cockpit_t
{
    bool TSButton {false};       /**< Flag indicating the status of the TS button. */
    bool RTDButton {false};      /**< Flag indicating the status of the RTD button. */

};

struct HVCB_t{
    uint16_t LV_Battery_24V {0};
    uint16_t DC_DC_24V{0};
    uint16_t input_12V{0};
};


enum steering_error_t{
    noFault,
    overTemperatureFault,
    overCurrentFault,
    overVoltageFault,
    underVoltageFault,
    encoderFault,
    phaseCurrentUnbalanceFault
};
struct steering_t{
    int16_t steeringPosition        {0}; /**< the position is int16 type, and the range -32000-32000 represents the position -3200°-3200°*/
    int16_t steeringSpeed           {0}; /**<  the speed is int16 type, and the range -32000-32000 represents -320000-320000rpm electrical speed*/
    int16_t steeringCurrent           {0}; /**<the current is of type int16, and the value -6000-6000 represents -60-60A */
    int8_t  steeringTemperature     {0}; /**< the temperature is int8 type, and the range of -20-127 represents the temperature of the driver board: -20℃-127℃ */
    steering_error_t  steeringErrorCode       {noFault}; /**< the error code is uint8 type, 0 means no fault, 1 means over temperature fault, 2 means over current fault, 3 means over voltage fault, 4 means under voltage fault, 5 means encoder fault, 6 means phase current unbalance fault (The hardware may be damaged) */
};

/**
 * @struct sensor_data_t
 * @brief A structure that holds various sensor data.
 */
struct sensor_data_t
{
    bool SDC_Pressed = true;      /**< Flag indicating the status of the SDC (Shut Down Circuit) if false red shut down button is pressed. */
    bool DVon = true;      /**< Flag indicating the status of the DV (Driverless) if false the car is not ready to drive. */
    pedal_t pedal;
    inverter_t inverter;
    accu_t accu;
    cockpit_t cockpit;
    HVCB_t HVVB;
    steering_t steering;
};

// Terminal commands ****************************************************************************************************
struct server_t
{
    int  port    {0};
    bool enabled {false};
};


struct settings_t
{
    // genaeral Setting
    bool shutdown {false};

    // MotorController settings
    bool recuperationEnabled {false};
    int16_t recuperationLevel {10}; /**< Recuperation level in percentage. If the throttle  falls below this level, recuperation is activated. */
    bool motorEnable [4] {true, true, true, true}; /**< Motor enable flags for front left, front right, rear left and rear right. */
    int8_t motorLoad [4]{60, 60, 100, 100}; /**< Motor load in percentage for front left, front right, rear left and rear right. */
    int16_t  maxTorque {2100};  /**< Max torque in 0.1% from available motor torque ( = 100%) */
    int16_t maxRecuperrationTorque {-100};/**< Max recuperation torque in 0.1% from available motor torque ( = 10%) */
    int16_t maxPower {78}; /**< Max Power the Car is able to get from the accumulator in kW*/

    // throttle settings in Motor controller
    int8_t alpha {15};/**< Lowpass filter parameter value between 1 and 100  100 = no filter  1 = strong filer*/

    //throttle logic control maxPower 
    double powerLimit_kp {5}; /**< Proportional gain for the power limit control */
    double powerLimit_ki {0}; /**< Integral gain for the power limit control */

    // server applications
    server_t datalogger;
    server_t driverless;
    server_t diagnostic;
    server_t display;
};

// Driverless commands **************************************************************************************************
struct driverless_commands_t
{
    float dvBrake    {0};
    float steeringAngle {0};
    float dvThrottle {0};
};


#endif // DATABASE_HPP
