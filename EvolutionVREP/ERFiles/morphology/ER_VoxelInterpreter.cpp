/**
	@file ER_VoxelInterpreter.cpp
    @authors Edgar Buchanan and Matteo de Carlo
	@brief .
*/

#include "ER_VoxelInterpreter.h"

ER_VoxelInterpreter::ER_VoxelInterpreter()
{

}

ER_VoxelInterpreter::~ER_VoxelInterpreter()
{

}

void ER_VoxelInterpreter::init(NEAT::NeuralNetwork &neuralNetwork)
{
    // Create voxel-matrix
    //TODO size of this region should be the size of the printing bed, with resolution as a multiple of 0.9mm per block
    const int MATRIX_HALF_SIZE = 25;
    const int MATRIX_SIZE = MATRIX_HALF_SIZE * 2;
    const simFloat SHAPE_SCALE_VALUE = static_cast<simFloat>(MATRIX_SIZE);
    PolyVox::RawVolume<uint8_t> volData(PolyVox::Region(PolyVox::Vector3DInt32(-MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE), PolyVox::Vector3DInt32(MATRIX_HALF_SIZE, MATRIX_HALF_SIZE, MATRIX_HALF_SIZE)));

    std::vector<double> input{0,0,0}; // Vector used as input of the Neural Network (NN).
    double output; // Variable used to store the output of the NN.
    // Create NN
    // Generate NN from template
    // genome.BuildPhenotype(neuralNetwork);
    // Generate voxel matrix
    auto region = volData.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1){
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1){
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1){
                input[0] = x/(volData.getDepth()/2.0);
                input[1] = y/(volData.getHeight()/2.0);
                input[2] = z/(volData.getWidth()/2.0);
                // Set inputs to NN
                neuralNetwork.Input(input);
                // Activate NN
                neuralNetwork.Activate();
                // Take output from NN and store it.
                output = neuralNetwork.Output()[0];
//                output = sqrt(x*x + y*y + z*z);
                // If output greater than threshold write voxel.

#ifdef HARD_BOUNDARIES
                uint8_t uVoxelValue = 0;
                if(output > 0.5){
                    uVoxelValue = 255;
                }
#else
                output = std::min(1.0, std::max(0.0, output));
                uint8_t uVoxelValue = static_cast<uint8_t>(output*255.0);
#endif
                volData.setVoxel(x, y, z, uVoxelValue);
            }
        }
    }
    // Marching cubes - we might not need this step at the beginning.
    const auto mesh = PolyVox::extractMarchingCubesMesh(&volData, volData.getEnclosingRegion());
    // I'm not sure if we need this step.
    const auto decodedMesh = PolyVox::decodeMesh(mesh);

    const unsigned int n_vertices = decodedMesh.getNoOfVertices();
    const unsigned int n_indices = decodedMesh.getNoOfIndices();
    std::vector<simFloat> vertices;
    std::vector<simInt> indices;
    vertices.reserve(n_vertices);
    indices.reserve(n_indices);
    for (unsigned int i=0; i < n_vertices; i++) {
        const auto &pos = decodedMesh.getVertex(i).position;
        vertices.emplace_back(pos.getX() / SHAPE_SCALE_VALUE);
        vertices.emplace_back(pos.getY() / SHAPE_SCALE_VALUE);
        vertices.emplace_back(pos.getZ() / SHAPE_SCALE_VALUE);
    }
    for (unsigned int i=0; i < n_indices; i++) {
        indices.emplace_back(decodedMesh.getIndex(i));
    }

    // Import mesh to V-REP
    if (n_vertices > 0) {
        handle = simCreateMeshShape(0, 20.0f * 3.1415f / 180.0f,
                vertices.data(), n_vertices * 3,
                indices.data(), n_indices,
                nullptr);
    } else {
        //TODO fail viability test
        std::clog << "Generated voxel mesh has no volume" << std::endl;
        std::clog << "TODO: fail viability test" << std::endl;
    }
    if (handle == -1) {
        std::cout << "Importing mesh NOT succesful!" << std::endl;
    }
}

void ER_VoxelInterpreter::mutate()
{

}

int ER_VoxelInterpreter::getMainHandle()
{
    return handle;
}

std::shared_ptr<Morphology> ER_VoxelInterpreter::clone()
{
    return std::shared_ptr<Morphology>(this);
}

void ER_VoxelInterpreter::create()
{

}

void ER_VoxelInterpreter::update()
{
    Development::update();
}
