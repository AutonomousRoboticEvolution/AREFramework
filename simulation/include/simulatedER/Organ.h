//
// Created by ebb505 on 12/03/2021.
//

#ifndef ER_ORGAN_H
#define ER_ORGAN_H

#include "PolyVox/RawVolume.h"
#include <vector>
#include <stdio.h>
#include <assert.h>
#include "ARE/Settings.h"
#include "ARE/morphology_descriptors.hpp"

#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif

namespace are {

namespace sim{

class Organ{
private:
    int organHandle;
    int organType;
    int connectorHandle;
    int physics_connector_handle;
    std::vector<float> connectorOri;
    bool organRemoved;
    bool organChecked;
    are::settings::ParametersMapPtr parameters;
    std::vector<float> connector_frame_pos;

public:

    /// \todo EB: For this to moved easily to private, multi-segmented robots might need to be refactored...
    std::vector<float> organPos;
    std::vector<float> connectorPos;
    std::vector<float> organOri;
    std::vector<int> objectHandles; // For collision detection purpose

    /// \todo EB: This can be easily moved to private if RobotManRes is deleted.
    bool organInsideSkeleton;
    bool organColliding;
    bool organGripperAccess;

    Organ(int oT, std::vector<float> oP, std::vector<float> oO, const are::settings::ParametersMapPtr &param){
        organType = oT;
        organHandle = -1; connectorHandle = -1; physics_connector_handle = -1;
        organPos = oP;
        organOri = oO;
        connectorPos.clear();  connectorOri.clear();
        organInsideSkeleton = false; organColliding = false;
        organGripperAccess = false; organRemoved = false; organChecked = false;
        objectHandles.clear();
        parameters = param;
    }
    Organ(const Organ& o):
        organHandle(o.organHandle),
        organType(o.organType),
        connectorHandle(o.connectorHandle),
        physics_connector_handle(o.physics_connector_handle),
        connectorOri(o.connectorOri),
        organRemoved(o.organRemoved),
        organChecked(o.organChecked),
        parameters(o.parameters),
        organPos(o.organPos),
        connectorPos(o.connectorPos),
        organOri(o.organOri),
        organInsideSkeleton(o.organInsideSkeleton),
        organColliding(o.organColliding),
        organGripperAccess(o.organGripperAccess){}
    /**
     * @brief This method creates, places and rotates the simulated organ. The parent will be the skeleton hangle passed.
     * @param skeletonHandle - parent skeleton
     */
    void createOrgan(int skeletonHandle);
    /**
     * @brief Creates the male connect just for visualization.
     */
    void createMaleConnector();
    /**
     * @brief This method applies all the manufacturability tests.
     * @param skeletonMatrix - Skeleton of the main body
     * @param gripperHandle - Handle of the gripper
     * @param skeletonHandles - All skeletons including main and children.
     * @param organList - All organs to compare to.
     */
    void testOrgan(PolyVox::RawVolume<uint8_t> &skeletonMatrix, int gripperHandle, const std::vector<int>& skeletonHandles, const std::vector<Organ>& organList);
    /**
     * @brief This test if the organ is colliding with another different organ or with an skeleton.
     * @param skeletonHandles - All skeletons including main and children.
     * @param organList - All organs to compare to.
     */
    void IsOrganColliding(const std::vector<int>& skeletonHandles, const std::vector<Organ>& organList);
    /**
     * @brief This test checks if the gripper connecting the organ is colliding with anything.
     * @param gripperHandle - Handle of the gripper
     * @param skeletonHandles - All skeletons including main and children.
     * @param organList - All organs to compare to.
     */
    void isGripperCollision(int gripperHandle, const std::vector<int>& skeletonHandles, const std::vector<Organ>& organList);
    /**
     * @brief This checks of the organ is inside of the main skeleton. This method was written because isOrganColliding does'nt
     * work when one organ is inside a second organ.
     * @param skeletonMatrix - Skeleton of the main body
     */
    void isOrganInsideMainSkeleton(PolyVox::RawVolume<uint8_t> &skeletonMatrix);
    /**
     * @brief Removes the organ from the final phenotype.
     */
    void repressOrgan();
    /// Getters and setters
    int getOrganHandle() const {return organHandle;}
    int getOrganType() const {return organType;}
    std::vector<float> get_connector_frame_pos() const {return connector_frame_pos;}
    int get_graphical_connector_handle() const {return physics_connector_handle;}
    bool isOrganRemoved() const {return organRemoved;}
    bool isOrganChecked() const {return organChecked;}
    void set_organ_removed(bool oR) {organRemoved = oR;}
    void set_organ_checked(bool oC) {organChecked = oC;}
};

}//sim

}//are

#endif //ER_ORGAN_H
