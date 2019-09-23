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
    void init() override;
    void mutate() override;
    void update() override;
    void create() override;

    void setCPPN(NEAT::NeuralNetwork neuralNetwork);

    int mainHandle; // Main handle

    std::shared_ptr<Morphology> clone();

private:
    NEAT::NeuralNetwork cppn;
};