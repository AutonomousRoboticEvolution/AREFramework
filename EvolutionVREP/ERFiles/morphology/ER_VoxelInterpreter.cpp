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
    simRemoveObject(mainHandle);
}

void ER_VoxelInterpreter::init()
{
    // TODO: EB - Finish implementing when MultiNEAT ready.
    // Create voxel-matrix
    PolyVox::RawVolume<int8_t> volData(PolyVox::Region(PolyVox::Vector3DInt32(0, 0,0 ), PolyVox::Vector3DInt32(10, 10, 10)));
    // Generate voxels
    //generateVoxels(volData);
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
        indices.emplace_back(decodedMesh.getIndex(i));
    }

    // Import mesh to V-REP
    mainHandle = simCreateMeshShape(2, 20.0f*3.1415f/180.0f, vertices.data(), n_vertices, indices.data(), n_indices, nullptr);
    if(mainHandle == -1){
        std::cout << "Importing mesh NOT succesful!" << std::endl;
    }
    // Scale down object
    simScaleObject(mainHandle, 0.1, 0.1, 0.1, 0);
    // Object is collidable
    simSetObjectSpecialProperty(mainHandle, sim_objectspecialproperty_collidable);
    // Object is dynamic
    simSetObjectInt32Parameter(mainHandle,sim_shapeintparam_static,0);
    // Object is respondable
    simSetObjectInt32Parameter(mainHandle,sim_shapeintparam_respondable,1);
    //int convDecomIntPams[] = {1,500,100,0,0,10000,20,4,4,20};
    //float convDecomFloatPams[] = {0.001,30,0.25,0.0,0.0,0.0025,0.05,0.05,0.00125,0.0001};
    //int handle = simConvexDecompose(mainHandle, 3, convDecomIntPams, convDecomFloatPams); // Convex decomposition
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

void ER_VoxelInterpreter::setCPPN(NEAT::NeuralNetwork neuralNetwork)
{
    cppn = neuralNetwork;
}

void ER_VoxelInterpreter::generateVoxels(PolyVox::RawVolume<uint8_t>& volData)
{
    std::vector<double> input{0,0,0}; // Vector used as input of the Neural Network (NN).
    double output; // Variable used to store the output of the NN.
    // Generate voxel matrix
    for(float z = 1; z < 10; z++){
        for(float y = 1; y < 10; y++){
            for(float x = 1; x < 10; x++){
                input[0] = x;
                input[1] = y;
                input[2] = z;
                // Set inputs to NN
                cppn.Input(input);
                // Activate NN
                cppn.Activate();
                // Take output from NN and store it.
                output = cppn.Output()[0];
                // If output greater than threshold write voxel.
                uint8_t uVoxelValue = 0;
                if(output > 0.5){
                    uVoxelValue = 255;
                }
                volData.setVoxel(x*10, y*10, z*10, uVoxelValue);
            }
        }
    }
}
