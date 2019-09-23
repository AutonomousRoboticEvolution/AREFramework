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

void ER_VoxelInterpreter::init()
{
    // TODO: EB - Finish implementing when MultiNEAT ready.
    // Create voxel-matrix
    PolyVox::RawVolume<int8_t> volData(PolyVox::Region(PolyVox::Vector3DInt32(-10, -10, -10), PolyVox::Vector3DInt32(10, 10, 10)));
    std::vector<double> input{0,0,0}; // Vector used as input of the Neural Network (NN).
    double output; // Variable used to store the output of the NN.
    // Create NN
    NEAT::NeuralNetwork neuralNetwork;
    // Generate NN from template
    // genome.BuildPhenotype(neuralNetwork);
    // Generate voxel matrix
    for(int z = -1; z <= 1; z += 0.1){
        for(int y = -1; y < 1; y += 0.1){
            for(int x = -1; x <= 1; x += 0.1){
                input[0] = x;
                input[1] = y;
                input[2] = z;
                // Set inputs to NN
                neuralNetwork.Input(input);
                // Activate NN
                neuralNetwork.Activate();
                // Take output from NN and store it.
                output = neuralNetwork.Output()[0];
                // If output greater than threshold write voxel.
                uint8_t uVoxelValue = 0;
                if(output > 0.5){
                    uVoxelValue = 255;
                }
                volData.setVoxel(x*10, y*10, z*10, uVoxelValue);
            }
        }
    }
    // Marching cubes - we might not need this step at the beginning.
    auto mesh = PolyVox::extractMarchingCubesMesh(&volData, volData.getEnclosingRegion());
    // I'm not sure if we need this step.
    auto decodedMesh = PolyVox::decodeMesh(mesh);

    unsigned int n_vertices = decodedMesh.getNoOfVertices();
    unsigned int n_indices = decodedMesh.getNoOfIndices();
    std::vector<simFloat> vertices;
    std::vector<simInt> indices;
    vertices.reserve(n_vertices);
    indices.reserve(n_indices);
    for (unsigned int i=0; i < n_vertices; i++) {
        const auto &pos = decodedMesh.getVertex(i).position;
        vertices.emplace_back(pos.getX());
        vertices.emplace_back(pos.getY());
        vertices.emplace_back(pos.getZ());
    }
    for (unsigned int i=0; i < n_indices; i++) {
        vertices.emplace_back(decodedMesh.getIndex(i));
    }

    // Import mesh to V-REP
    int handle;
    handle = simCreateMeshShape(0, 20.0f*3.1415f/180.0f, vertices.data(), n_vertices, indices.data(), n_indices, nullptr);
    if(handle == -1){
        std::cout << "Importing mesh NOT succesful!" << std::endl;
    }
}

void ER_VoxelInterpreter::mutate()
{

}

std::shared_ptr<Morphology> ER_VoxelInterpreter::clone()
{
    return std::shared_ptr<Morphology>();
}

void ER_VoxelInterpreter::create()
{

}
