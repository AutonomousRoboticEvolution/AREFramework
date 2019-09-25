/**
	@file ER_VoxelInterpreter.h
    @authors Edgar Buchanan and Matteo de Carlo
	@brief .
*/
#pragma once

#include "Development.h"
#include "PolyVox/RawVolume.h"
#include "PolyVox/MarchingCubesSurfaceExtractor.h"

class ER_VoxelInterpreter : public Development
{
public:
    ER_VoxelInterpreter();
    ~ER_VoxelInterpreter();

    // Essentials
    void init(NEAT::NeuralNetwork &neuralNetwork);
    void mutate() override;
    void update() override;
    void create() override;

    std::shared_ptr<Morphology> clone();

    int getMainHandle() override;

private:
    simInt handle;
};