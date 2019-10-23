#include "Morphology.h"
#include <iostream>

Morphology::Morphology(const Settings& st){
    settings.reset(new Settings(st));
    if(!load_fct_exp_plugin<Control::Factory>
            (controlFactory,settings->exp_plugin_name,"controlFactory"))
        exit(1);
}

std::vector<std::shared_ptr<ER_Module>> Morphology::getCreatedModules()
{
	std::vector<std::shared_ptr<ER_Module>> nullVec;
	return nullVec;
}
