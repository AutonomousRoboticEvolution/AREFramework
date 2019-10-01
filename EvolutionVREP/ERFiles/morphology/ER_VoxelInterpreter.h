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
    ER_VoxelInterpreter(unsigned int id);
    ~ER_VoxelInterpreter();

    // Essentials
    void init(NEAT::NeuralNetwork &neuralNetwork, bool decompose = true);
    void mutate() override;
    void update() override;
    void create() override;

    /// Generate matrix with voxels
    void generateVoxels(PolyVox::RawVolume<uint8_t>& volData, NEAT::NeuralNetwork &network);
    /// Export mesh from list of vertices and indices
    void exportMesh(std::vector<simFloat> vertices, std::vector<simInt> indices);
    /// Get indices and vertices from mesh file
    void getIndicesVertices(PolyVox::Mesh<PolyVox::Vertex<uint8_t>>& decodedMesh, std::vector<simFloat>& vertices, std::vector<simInt>& indices);

    /// Voxel-matrix post-processing methods.
    /// Empty space for head organ.
    void emptySpaceForHead(PolyVox::RawVolume<uint8_t>& volData);

    std::shared_ptr<Morphology> clone();

    int getMainHandle() override;

    //TODO size of this region should be the size of the printing bed, with resolution as a multiple of 0.9mm per block
    const float VOXEL_SIZE = 0.0009; //m³ - 0.9mm³
    const int VOXEL_MULTIPLIER = 4; // voxel of 3.4cm³
    const float VOXEL_REAL_SIZE = VOXEL_SIZE * static_cast<float>(VOXEL_MULTIPLIER); // voxel of 3.4cm³
    const int MATRIX_SIZE = (264 / VOXEL_MULTIPLIER); // 66
    const int MATRIX_HALF_SIZE = MATRIX_SIZE / 2; // 33

    // results into 23.76x23.76x23.76 cm³ - in reality is 28x28x25 cm³
    const simFloat SHAPE_SCALE_VALUE = VOXEL_REAL_SIZE;

private:
    unsigned int id;
};