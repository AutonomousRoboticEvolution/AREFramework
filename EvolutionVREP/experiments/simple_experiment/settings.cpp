#include "ARE/Settings.h"

class ExpSettings : public Settings {
public:
    ExpSettings() : Settings(){

        evolutionType = STEADY_STATE; //or EA_MULTINEAT
        exp_plugin_name = "/usr/local/lib/libsimple_experiment.so";
    }


};


extern "C" ExpSettings* create_settings()
{
    return new ExpSettings;
}
