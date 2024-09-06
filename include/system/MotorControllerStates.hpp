//
// Created by timro on 15/02/2024.
//

#ifndef MOTORCONTROLLER_MOTORCONTROLLERSTATES_HPP
#define MOTORCONTROLLER_MOTORCONTROLLERSTATES_HPP

#include "MotorController.hpp"
#include "Timer.hpp"

#define AUTO_STOP_TIME_5s 5000
#define AUTO_STOP_TIME_6s 6000
#define AUTO_STOP_TIME_10s 10000



class Disable: public State<MotorController> {
public:
    void enterState(MotorController *motorController) override;
    void stateAction(MotorController* motorController) override;
    void updateState(MotorController* motorController) override;
    void exitState(MotorController *motorController) override;
    std::string toString() override;
    static State<MotorController>& getInstance();
private:
    Timer buttonPressTimer;
    bool buttonReleasedFlag = false;

    Disable(const Disable& other) = delete;
    Disable& operator=(const Disable& other) = delete;
    Disable(): buttonPressTimer(AUTO_STOP_TIME_5s){};

};

class Precharge: public State<MotorController> {
public:
    void enterState(MotorController *motorController) override;
    void stateAction(MotorController* motorController) override;
    void updateState(MotorController* motorController) override;
    void exitState(MotorController *motorController) override;
    std::string toString() override;
    static State<MotorController>& getInstance();
private:
    Timer prechargeTimer;
    Timer closedCircuitTimer;
    bool buttonReleasedFlag = false;

    Precharge(const Precharge& other) = delete;
    Precharge& operator=(const Precharge& other) = delete;
    Precharge(): prechargeTimer(AUTO_STOP_TIME_10s), closedCircuitTimer(AUTO_STOP_TIME_6s){}
};

class HVCoupling: public State<MotorController> {
public:
    void enterState(MotorController *motorController) override;
    void stateAction(MotorController* motorController) override;
    void updateState(MotorController* motorController) override;
    void exitState(MotorController *motorController) override;
    std::string toString() override;
    static State<MotorController>& getInstance();
private:
    Timer couplingTimer;

    HVCoupling(const HVCoupling& other) = delete;
    HVCoupling& operator=(const HVCoupling& other) = delete;
    HVCoupling(): couplingTimer(AUTO_STOP_TIME_5s){};
};

class ReadyToDrive: public State<MotorController> {
public:
    void enterState(MotorController *motorController) override;
    void stateAction(MotorController* motorController) override;
    void updateState(MotorController* motorController) override;
    void exitState(MotorController *motorController) override;
    std::string toString() override;
    static State<MotorController>& getInstance();
private:
    ReadyToDrive(const ReadyToDrive& other) = delete;
    ReadyToDrive& operator=(const ReadyToDrive& other) = delete;
    ReadyToDrive()= default;
};

class Drive: public State<MotorController> {
public:
    void enterState(MotorController *motorController) override;
    void stateAction(MotorController* motorController) override;
    void updateState(MotorController* motorController) override;
    void exitState(MotorController *motorController) override;
    std::string toString() override;
    static State<MotorController>& getInstance();
private:

    uint16_t throttleLP(uint16_t throttle, uint8_t alpha); // lowpass filter for throttle ALPHA = 1 => no filering  ALPHA = 0.1 big filtering
    int16_t calculateTorque(uint16_t throttle, int32_t maxTorque); // non lineal throttle torque EXPONENT = 1 => linear  EXPONENT > 1  => non linear

    double previousThrottle = 0;
    const std::string Positions[4] = {"front left", "front right", "rear left", "rear right"};
    Timer RTDsoundTimer;
    Timer inverterStartupTimer;
    Timer BSplausibilityTimer;
    bool RTDsoundRunningFlag = true;

    

    Drive(const Drive& other) = delete;
    Drive& operator=(const Drive& other) = delete;
    Drive(): inverterStartupTimer(AUTO_STOP_TIME_10s), RTDsoundTimer(AUTO_STOP_TIME_5s), BSplausibilityTimer(AUTO_STOP_TIME_5s){};
};

class Recuperate: public State<MotorController> {
public:
    void enterState(MotorController *motorController) override;
    void stateAction(MotorController* motorController) override;
    void updateState(MotorController* motorController) override;
    void exitState(MotorController *motorController) override;
    std::string toString() override;
    static State<MotorController>& getInstance();
private:
    const std::string Positions[4] = {"front left", "front right", "rear left", "rear right"};
    Timer inverterStartupTimer;

    Recuperate(const Recuperate& other) = delete;
    Recuperate& operator=(const Recuperate& other) = delete;
    Recuperate(): inverterStartupTimer(AUTO_STOP_TIME_10s){};
};

class Error: public State<MotorController> {
public:
    void enterState(MotorController *motorController) override;
    void stateAction(MotorController* motorController) override;
    void updateState(MotorController* motorController) override;
    void exitState(MotorController *motorController) override;
    std::string toString() override;
    static State<MotorController>& getInstance();
private:
    Timer errorResetTimer;

    Error(const Error& other) = delete;
    Error& operator=(const Error& other) = delete;
    Error(): errorResetTimer(AUTO_STOP_TIME_5s){};
};

class Overheat: public State<MotorController> {
public:
    void enterState(MotorController *motorController) override;
    void stateAction(MotorController* motorController) override;
    void updateState(MotorController* motorController) override;
    void exitState(MotorController *motorController) override;
    std::string toString() override;
    static State<MotorController>& getInstance();
private:
    Overheat(const Overheat& other) = delete;
    Overheat& operator=(const Overheat& other) = delete;
    Overheat()= default;
};

class InverterErrorReset: public State<MotorController> {
public:
    void enterState(MotorController *motorController) override;
    void stateAction(MotorController* motorController) override;
    void updateState(MotorController* motorController) override;
    void exitState(MotorController *motorController) override;
    std::string toString() override;
    static State<MotorController>& getInstance();
private:
    Timer resetTimer;
    bool resetSuccessFlag = false;
    InverterErrorReset(const InverterErrorReset& other) = delete;
    InverterErrorReset& operator=(const InverterErrorReset& other) = delete;
    InverterErrorReset()= default;
};


#endif //MOTORCONTROLLER_MOTORCONTROLLERSTATES_HPP
