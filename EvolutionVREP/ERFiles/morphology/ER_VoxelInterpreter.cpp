#include "ER_VoxelInterpreter.h"

ER_VoxelInterpreter::ER_VoxelInterpreter(unsigned int id)
    : id(id), EvolvedMorphology ()
{
    mainHandle = 0;
}

ER_VoxelInterpreter::~ER_VoxelInterpreter()
{
    simRemoveObject(mainHandle);
}

void ER_VoxelInterpreter::init(NEAT::NeuralNetwork &neuralNetwork, bool decompose)
{
    // Create voxel-matrix
    PolyVox::RawVolume<AREVoxel> areMatrix(PolyVox::Region(PolyVox::Vector3DInt32(-MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE), PolyVox::Vector3DInt32(MATRIX_HALF_SIZE, MATRIX_HALF_SIZE, MATRIX_HALF_SIZE)));
    PolyVox::RawVolume<uint8_t > skeletonMatrix(PolyVox::Region(PolyVox::Vector3DInt32(-MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE), PolyVox::Vector3DInt32(MATRIX_HALF_SIZE, MATRIX_HALF_SIZE, MATRIX_HALF_SIZE)));
    // Decode genome
    genomeDecoder(areMatrix, neuralNetwork);
    /// Post-processing functions
    generateSkeleton(areMatrix, skeletonMatrix); // Generate skeleton without modifications
    regionCounter(areMatrix);
    emptySpaceForHead(skeletonMatrix); // Make space for head organ.

    // Marching cubes - we might not need this step at the beginning.
    auto mesh = PolyVox::extractMarchingCubesMesh(&skeletonMatrix, skeletonMatrix.getEnclosingRegion());
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
    if (!vertices.empty()) {
        mainHandle = simCreateMeshShape(2, 20.0f * 3.1415f / 180.0f, vertices.data(), vertices.size(), indices.data(),
                                        indices.size(), nullptr);
        if (mainHandle == -1) {
            std::cout << "Importing mesh NOT succesful!" << std::endl;
        }
        std::ostringstream name;
        name << "VoxelBone" << this->id;
        simSetObjectName(mainHandle, name.str().c_str());
        // Set starting point
        simFloat objectPosition[3];
        objectPosition[0] = 0.0;
        objectPosition[1] = 0.0;
        objectPosition[2] = 0.15;
        simSetObjectPosition(mainHandle, -1, objectPosition);
#define VISUALIZE_RAW_CPPN_SHAPE 0
#ifdef VISUALIZE_RAW_CPPN_SHAPE
        simAddObjectToSelection(sim_handle_single, mainHandle);
        simCopyPasteSelectedObjects();
        simInt reference_object = simGetObjectLastSelection();
        name << "_copy";
        simSetObjectName(reference_object, name.str().c_str());
        simFloat reference_object_pos[3];
        simGetObjectPosition(mainHandle, -1, reference_object_pos);
        reference_object_pos[0] -= 0.4;
        simSetObjectPosition(reference_object, -1, reference_object_pos);
#endif
        // Object is collidable
        simSetObjectSpecialProperty(mainHandle, sim_objectspecialproperty_collidable);
        // Object is dynamic
        simSetObjectInt32Parameter(mainHandle, sim_shapeintparam_static, 0);
        // Object is respondable
        simSetObjectInt32Parameter(mainHandle, sim_shapeintparam_respondable, 1);
        // Convex decomposition with V-HACD
        if (decompose) {
            // Resolution of 1000000 seems to work better than defaul (10000). However it makes it slower.
            int convDecomIntPams[] = {1, 500, 100, 0, 0, //HACD
                                      1000000, 20, 4, 4, 64}; //V-HACD
            float convDecomFloatPams[] = {0.001, 30, 0.25, 0.0, 0.0,//HACD
                                          0.0025, 0.05, 0.05, 0.00125, 0.0001};//V-HACD
            try {
                mainHandle = simConvexDecompose(mainHandle, 1u | 128u, convDecomIntPams,
                                                convDecomFloatPams); // Convex decomposition
            } catch (std::exception &e) {
                //TODO if decomposition fails, discard the robot.
                std::clog << "Decomposition failed: why? " << e.what() << std::endl;
            }
        } else {
            std::cout << "not decomposing" << std::endl;
        }
        // Recompute mass and ineratia to fix object vibration
        // skeleton of PLA is 1.210–1.430 g·cm−3 cit. Wikipedia
        simComputeMassAndInertia(mainHandle, 1.3f);
        int brainHandle = simLoadModel("models/C_BrainC.ttm");
        simSetObjectParent(brainHandle,mainHandle,1);
        int collectionHandle = simCreateCollection("collection",0);
        simAddObjectToCollection(collectionHandle,mainHandle,sim_handle_single,0);

    } else {
        //TODO no mesh data, CPPN generated no volume.
    }
}

void ER_VoxelInterpreter::mutate()
{

}

void ER_VoxelInterpreter::update()
{
    EvolvedMorphology::update();
}

void ER_VoxelInterpreter::create()
{

}

std::shared_ptr<Morphology> ER_VoxelInterpreter::clone()
{
    return std::shared_ptr<Morphology>(this);
}

void ER_VoxelInterpreter::genomeDecoder(PolyVox::RawVolume<AREVoxel>& areMatrix, NEAT::NeuralNetwork &cppn)
{
    std::vector<double> input{0,0,0}; // Vector used as input of the Neural Network (NN).
    AREVoxel areVoxel;
    // Generate voxel matrix
    auto region = areMatrix.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                input[0] = static_cast<double>(x); // x/(areMatrix.getDepth()/2.0);
                input[1] = static_cast<double>(y); //y/(areMatrix.getHeight()/2.0);
                input[2] = static_cast<double>(z); //z/(areMatrix.getWidth()/2.0);
                // Set inputs to NN
                cppn.Input(input);
                // Activate NN
                cppn.Activate();
                // Take output from NN and store it.
                areVoxel.bone = 0;
                areVoxel.wheel = 0;
                if(cppn.Output()[0] > 0.5){
                    areVoxel.bone = 255;
                }
                if(cppn.Output()[1] > 0.5){
                    areVoxel.wheel = 255;
                }
                areMatrix.setVoxel(x, y, z, areVoxel);

            }
        }
    }
}

void ER_VoxelInterpreter::getIndicesVertices(PolyVox::Mesh<PolyVox::Vertex<uint8_t >> &decodedMesh,
                                             std::vector<simFloat>& vertices, std::vector<simInt>& indices)
{
    const unsigned int n_vertices = decodedMesh.getNoOfVertices();
    const unsigned int n_indices = decodedMesh.getNoOfIndices();

    if (n_vertices <= 0) {
        //TODO fail viability test
        std::clog << "Generated voxel mesh has no volume" << std::endl;
        std::clog << "TODO: fail viability test" << std::endl;
        return;
    }

    const PolyVox::Vector3DFloat *prev = nullptr;
    bool pointObject = true;
    for (unsigned int i=0; i < n_vertices; i++) {
        const PolyVox::Vector3DFloat &pos = decodedMesh.getVertex(i).position;
        if (pointObject) {
            if (prev != nullptr and (*prev) != pos) pointObject = false;
            prev = &pos;
        }
        vertices.emplace_back(pos.getX() * SHAPE_SCALE_VALUE);
        vertices.emplace_back(pos.getY() * SHAPE_SCALE_VALUE);
        vertices.emplace_back(pos.getZ() * SHAPE_SCALE_VALUE);
    }

    // If all vectors are the same, we have an object the size of point. This is considered a failed viability.
    // and it makes the vertex decomposition to crash badly.
    if (pointObject) {
        vertices.clear();
        indices.clear();
        return;
    }

    for (unsigned int i=0; i < n_indices; i++) {
        indices.emplace_back(decodedMesh.getIndex(i));
    }
}

void ER_VoxelInterpreter::exportMesh(std::vector<simFloat> vertices, std::vector<simInt> indices)
{
    const auto **verticesMesh = new const simFloat *[2];
    auto *verticesSizesMesh = new simInt[2];
    const auto **indicesMesh = new const simInt *[2];
    auto *indicesSizesMesh = new simInt[2];
    verticesMesh[0] = vertices.data();
    verticesSizesMesh[0] = vertices.size();
    indicesMesh[0] = indices.data();
    indicesSizesMesh[0] = indices.size();

    std::ostringstream filename;
    filename << "morphology_" << this->id << ".stl";

    //fileformat: the fileformat to export to:
    //  0: OBJ format
    //  3: TEXT STL format
    //  4: BINARY STL format
    //  5: COLLADA format
    //  6: TEXT PLY format
    //  7: BINARY PLY format
    simExportMesh(3, filename.str().c_str(),
            0, 1.0f, 1,
            verticesMesh, verticesSizesMesh,
            indicesMesh, indicesSizesMesh,
            nullptr, nullptr);

    delete[] verticesMesh;
    delete[] verticesSizesMesh;
    delete[] indicesMesh;
    delete[] indicesSizesMesh;
}

int ER_VoxelInterpreter::getMainHandle()
{
    return mainHandle;
}


void ER_VoxelInterpreter::emptySpaceForHead(PolyVox::RawVolume<uint8_t> &skeletonMatrix)
{
    // Size of head slot: 11.4cm x 10.0 cm
    // This is roughly 33.5 by 29.4 voxels with voxel size of 3.4mm.
    const int xUpperLimit = 16;  // Was: 15
    const int xLowerLimit = -16; // Was: 15
    const int yUpperLimit = 18; // Was: 17
    const int yLowerLimit = -18; // Was: 17
    uint8_t uVoxelValue = 0;
    auto region = skeletonMatrix.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                if(x <= xUpperLimit && x >= xLowerLimit && y <= yUpperLimit && y >= yLowerLimit){
                    skeletonMatrix.setVoxel(x, y, z, uVoxelValue);
                }
            }
        }
    }

}

void ER_VoxelInterpreter::generateSkeleton(PolyVox::RawVolume<AREVoxel>& areMatrix, PolyVox::RawVolume<uint8_t>& skeletonMatrix)
{
    AREVoxel areVoxel;
    uint8_t uVoxelValue;
    // Generate voxel matrix
    auto region = areMatrix.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                areVoxel = areMatrix.getVoxel(x, y, z);
                // If output greater than threshold write voxel.
                // NOTE: Hard boundaries seem to work better with convex decomposition
#define HARD_BOUNDARIES 0
#ifdef HARD_BOUNDARIES
                if(areVoxel.bone > 0.5){
                    uVoxelValue = 255;
                }
                else{
                    uVoxelValue = 0;
                }
#else
                uVoxelValue = std::min(1.0, std::max(0.0, areVoxel.bone));
                uVoxelValue = static_cast<uint8_t>(uVoxelValue*255.0);
#endif
                skeletonMatrix.setVoxel(x, y, z, uVoxelValue);
            }
        }
    }
}

void ER_VoxelInterpreter::regionCounter(PolyVox::RawVolume<AREVoxel> &areMatrix)
{
    // This matrix stores the visited elements.
    PolyVox::RawVolume<bool > visitedVoxels(PolyVox::Region(PolyVox::Vector3DInt32(-MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE), PolyVox::Vector3DInt32(MATRIX_HALF_SIZE, MATRIX_HALF_SIZE, MATRIX_HALF_SIZE)));
    AREVoxel areVoxel;
    int blobCounter = 0;
    auto region = areMatrix.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 2) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 2) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 2) {
                areVoxel = areMatrix.getVoxel(x, y, z);
                if(areVoxel.bone > 125 && !visitedVoxels.getVoxel(x, y, z)){
                    blobCounter++;
                    exploration(areMatrix, visitedVoxels, x, y, z, areVoxel);
                }
            }
        }
    }
}

void ER_VoxelInterpreter::exploration(PolyVox::RawVolume<AREVoxel> &areMatrix, PolyVox::RawVolume<bool> &visitedVoxels, int32_t posX, int32_t posY, int32_t posZ, AREVoxel areVoxel)
{
    visitedVoxels.setVoxel(posX, posY, posZ, true); // Cell visited
    // Explore neighbourhood.
    for (int dz = -2; dz <= 2; dz+=2) {
        for (int dy = -2; dy <= 2; dy+=2) {
            for (int dx = -2; dx <= 2; dx+=2) {
                if (posX + dx > -MATRIX_HALF_SIZE && posX + dx < MATRIX_HALF_SIZE &&
                    posY + dy > -MATRIX_HALF_SIZE && posY + dy < MATRIX_HALF_SIZE &&
                    posZ + dz > -MATRIX_HALF_SIZE && posZ + dz < MATRIX_HALF_SIZE) {
                    areVoxel = areMatrix.getVoxel(posX + dx, posY + dy, posZ + dz);
                    if (!visitedVoxels.getVoxel(posX + dx, posY + dy, posZ + dz) && areVoxel.bone > 120) {
                        exploration(areMatrix, visitedVoxels, posX + dx, posY + dy, posZ + dz, areVoxel);
                    }
                }
            }
        }
    }
}
