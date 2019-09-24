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
    generateVoxels(volData);
    // Marching cubes - we might not need this step at the beginning.
    auto mesh = PolyVox::extractMarchingCubesMesh(&volData, volData.getEnclosingRegion());
    // I'm not sure if we need this step.
    auto decodedMesh = PolyVox::decodeMesh(mesh);
    // Get vertices and indices.
    std::vector<simFloat> vertices;
    std::vector<simInt> indices;
    vertices.reserve(decodedMesh.getNoOfVertices());
    indices.reserve(decodedMesh.getNoOfIndices());
    getIndicesVertices(decodedMesh,vertices,indices);
    // Generate mesh file (stl). We don't have to generate the mesh file here but we can use the function elsewhere.
    exportMesh(vertices,indices);
    // Import mesh to V-REP
    mainHandle = simCreateMeshShape(2, 20.0f*3.1415f/180.0f, vertices.data(), vertices.size(), indices.data(), indices.size(), nullptr);
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
    // Convex decomposition with V-HACD
    int convDecomIntPams[] = {1,500,100,0,0,10000,20,4,4,20};
    float convDecomFloatPams[] = {0.001,30,0.25,0.0,0.0,0.0025,0.05,0.05,0.00125,0.0001};
    int handle = simConvexDecompose(mainHandle, 129, convDecomIntPams, convDecomFloatPams); // Convex decomposition
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

/// Generate CPPN
void ER_VoxelInterpreter::setCPPN(NEAT::NeuralNetwork neuralNetwork)
{
    cppn = neuralNetwork;
}
/// Generate matrix with voxels
void ER_VoxelInterpreter::generateVoxels(PolyVox::RawVolume<int8_t>& volData)
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
/// Export mesh from list of vertices and indices
void ER_VoxelInterpreter::exportMesh(std::vector<simFloat> vertices, std::vector<simInt> indices)
{
    const auto** verticesMesh=new const simFloat*[2];
    auto* verticesSizesMesh=new simInt[2];
    const auto** indicesMesh=new const simInt*[2];
    auto* indicesSizesMesh=new simInt[2];
    verticesMesh[0] = vertices.data();
    verticesSizesMesh[0] = vertices.size();
    indicesMesh[0] = indices.data();
    indicesSizesMesh[0] = indices.size();
    simExportMesh(3,"example.stl",0,1,1,verticesMesh,verticesSizesMesh,indicesMesh,indicesSizesMesh,NULL,NULL);
}
/// Get indices and vertices from mesh file
void ER_VoxelInterpreter::getIndicesVertices(PolyVox::Mesh<PolyVox::Vertex<signed char>> &decodedMesh,
                                             std::vector<simFloat>& vertices, std::vector<simInt>& indices)
{
    for (unsigned int i=0; i < decodedMesh.getNoOfVertices(); i++) {
        const auto &pos = decodedMesh.getVertex(i).position;
        vertices.emplace_back(pos.getX());
        vertices.emplace_back(pos.getY());
        vertices.emplace_back(pos.getZ());
    }
    for (unsigned int i=0; i < decodedMesh.getNoOfIndices(); i++) {
        indices.emplace_back(decodedMesh.getIndex(i));
    }
}

