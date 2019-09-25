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

    /// Generate CPPN
    void setCPPN(NEAT::NeuralNetwork neuralNetwork);
    /// Generate matrix with voxels
    void generateVoxels(PolyVox::RawVolume<uint8_t>& volData);
    /// Export mesh from list of vertices and indices
    void exportMesh(std::vector<simFloat> vertices, std::vector<simInt> indices);
    /// Get indices and vertices from mesh file
    void getIndicesVertices(PolyVox::Mesh<PolyVox::Vertex<uint8_t>>& decodedMesh, std::vector<simFloat>& vertices, std::vector<simInt>& indices);

    std::shared_ptr<Morphology> clone();

    int getMainHandle() override;

    const int MATRIX_HALF_SIZE = 25;
    const int MATRIX_SIZE = MATRIX_HALF_SIZE * 2;
    const simFloat SHAPE_SCALE_VALUE = static_cast<simFloat>(MATRIX_SIZE);

private:
    NEAT::NeuralNetwork cppn;
};