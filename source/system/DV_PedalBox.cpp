#include "system/DV_PedalBox.hpp"

DV_PedalBox::DV_PedalBox(const driverless_commands_t *driverlessCommands, const settings_t *settings, system_data_t *systemData) {
    this->driverlessCommands = driverlessCommands;
    this->settings = settings;
    this->pedalBox = &systemData->pedalBox;
}

void DV_PedalBox::update(){
    // replace pedalBox throttle ouput with driverlessCommands throttle output
    pedalBox->throttle = driverlessCommands->dvThrottle * 10;
}