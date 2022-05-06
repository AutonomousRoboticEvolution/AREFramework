//
// Created by ebb505 on 06/05/2021.
//

#ifndef ARE_BLUEPRINT_H
#define ARE_BLUEPRINT_H

#include "simulatedER/Organ.h"

namespace are
{

namespace sim
{

class Blueprint
{
private:
    std::vector<int> organTypes;
    std::vector<std::vector<float>> organPosList;
    std::vector<std::vector<float>> organOriList;
public:
    void createBlueprint(std::vector<Organ> organList)
    {
        for (std::vector<Organ>::iterator it = organList.begin(); it != organList.end(); it++) {
            if (!it->isOrganRemoved() && it->isOrganChecked()) {
                organTypes.push_back(it->getOrganType());
                organPosList.push_back(it->get_connector_frame_pos());
                organOriList.push_back(it->organOri);
            }
        }
        organPosList.at(0).at(2)  = 0.0054;
    }

    std::vector<int> getOrganTypes()
    { return organTypes;}

    std::vector<std::vector<float>> getOrganPosList()
    { return organPosList;}

    std::vector<std::vector<float>> getOrganOriList()
    { return organOriList;}
};
}
}
#endif //ARE_BLUEPRINT_H
