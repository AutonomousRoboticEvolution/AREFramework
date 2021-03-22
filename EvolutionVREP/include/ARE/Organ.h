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

#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif

class Organ{
private:

    /// \todo EB: This shouldn't be here!
    constexpr static const float VOXEL_SIZE = 0.0009; //m³ - 0.9mm³
    static const int VOXEL_MULTIPLIER = 22;
    constexpr static const float VOXEL_REAL_SIZE = VOXEL_SIZE * static_cast<float>(VOXEL_MULTIPLIER);
    static const int MATRIX_SIZE = (264 / VOXEL_MULTIPLIER);
    static const int MATRIX_HALF_SIZE = MATRIX_SIZE / 2;
    static const short int EMPTYVOXEL = 0;
    static const short int FILLEDVOXEL = 255;
    static const short int xHeadUpperLimit = 2;
    static const short int xHeadLowerLimit = -2;
    static const short int yHeadUpperLimit = 2;

    int organHandle;
    int organType;
    int connectorHandle;
    int graphicConnectorHandle;
    std::vector<float> connectorOri;
    bool organRemoved;
    bool organChecked;
    are::settings::ParametersMapPtr parameters;

public:

    /// \todo EB: For this to moved easily to private, multi-segmented robots might need to be refactored...
    std::vector<float> organPos;
    std::vector<float> connectorPos;
    std::vector<float> organOri;
    std::vector<int> objectHandles; // For collision detection purpose

    /// \todo EB: This can be easily moved to private if RobotManRes is deleted.
    bool organInsideSkeleton;
    bool organColliding;
    bool organGoodOrientation;
    bool organGripperAccess;

    Organ(int oT, std::vector<float> oP, std::vector<float> oO, const are::settings::ParametersMapPtr &param){
        organType = oT;
        organHandle = -1; connectorHandle = -1; graphicConnectorHandle = -1;
        organPos = oP;
        organOri = oO;
        connectorPos.clear();  connectorOri.clear();
        organInsideSkeleton = false; organColliding = false; organGoodOrientation = false;
        organGripperAccess = false; organRemoved = false; organChecked = false;
        objectHandles.clear();
        parameters = param;
    }
    /**
     * @brief This method creates, places and rotates the simulated organ. The parent will be the skeleton hangle passed.
     * @param skeletonHandle - parent skeleton
     */
    void createOrgan(int skeletonHandle);
    /**
     * @brief Creates the male connect just for visualization.
     * @param skeletonHandle - parent skeleton
     */
    void createMaleConnector(int skeletonHandle);
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
     * @brief This test check if the organ has a good orientation
     */
    void isOrganGoodOrientation();
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
    bool isOrganRemoved() const {return organRemoved;}
    bool isOrganChecked() const {return organChecked;}
};

#endif //ER_ORGAN_H
