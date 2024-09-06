#include "data/database.hpp"

class DV_PedalBox {
public:
    // Constructor
    DV_PedalBox(const driverless_commands_t *driverlessCommands, const settings_t *settings, system_data_t *systemData);


    void update();

private:
    const driverless_commands_t *driverlessCommands;
    const settings_t *settings;
    pedal_box_t *pedalBox;
    

};