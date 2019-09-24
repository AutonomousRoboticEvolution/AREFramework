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

    /// Generate CPPN
    void setCPPN(NEAT::NeuralNetwork neuralNetwork);
    /// Generate matrix with voxels
    void generateVoxels(PolyVox::RawVolume<int8_t>& volData);
    /// Export mesh from list of vertices and indices
    void exportMesh(std::vector<simFloat> vertices, std::vector<simInt> indices);
    /// Get indices and vertices from mesh file
    void getIndicesVertices(PolyVox::Mesh<PolyVox::Vertex<signed char>>& decodedMesh, std::vector<simFloat>& vertices, std::vector<simInt>& indices);

    int mainHandle; // Main handle

    std::shared_ptr<Morphology> clone();

private:
    NEAT::NeuralNetwork cppn;
};