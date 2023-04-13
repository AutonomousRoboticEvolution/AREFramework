#ifndef PHY_IO_HELPERS_HPP
#define PHY_IO_HELPERS_HPP

#include "ARE/io_helpers.hpp"

#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif

//namesapce fs = std::filesystem //if c++17
namespace fs = boost::filesystem;

namespace are {

namespace ioh{


template<class Ind>
/**
 * @brief write in the waiting_to_be_built folder all the blueprints of a given population
 * @param folder of the database
 * @param population
 */
void write_morph_blueprints(const std::string& folder,const std::vector<Individual::Ptr> &population){
    int id = 0;
    for(const auto& ind: population){
        id = ind->get_morph_genome()->id();

        // Export blueprint
        std::stringstream sst_blueprint;
        sst_blueprint << "blueprint_" << id << ".csv";
        std::ofstream ofs(folder + std::string("/waiting_to_be_built/")  + sst_blueprint.str());
        if(!ofs)
        {
            std::cerr << "unable to open : " << folder + std::string("/waiting_to_be_built/")  + sst_blueprint.str() << std::endl;
            return;
        }
        std::vector<int> tempOrganTypes = std::dynamic_pointer_cast<Ind>(ind)->getListOrganTypes();
        std::vector<std::vector<float>> tempOrganPos = std::dynamic_pointer_cast<Ind>(ind)->getListOrganPos();
        std::vector<std::vector<float>> tempOrganOri = std::dynamic_pointer_cast<Ind>(ind)->getListOrganOri();
        for (int i = 0; i < tempOrganTypes.size(); i++) {
            ofs << "0" << "," << tempOrganTypes.at(i) << ","
                          << tempOrganPos.at(i).at(0) << "," << tempOrganPos.at(i).at(1) << ","
                          << tempOrganPos.at(i).at(2) << ","
                          << tempOrganOri.at(i).at(0) << "," << tempOrganOri.at(i).at(1) << ","
                          << tempOrganOri.at(i).at(2) << ","
                          << std::endl;
        }
        ofs.close();
    }
}

template<class Ind>
/**
 * @brief write in the waiting_to_be_built folder  all the skeleton meshes of a given population
 * @param folder
 * @param population
 */
void write_morph_meshes(const std::string& folder, const std::vector<Individual::Ptr> &population){
    int id = 0;
    for(const auto& ind: population){
        id = ind->get_morph_genome()->id();

        const auto **verticesMesh = new const simFloat *[2];
        const auto **indicesMesh = new const simInt *[2];
        auto *verticesSizesMesh = new simInt[2];
        auto *indicesSizesMesh = new simInt[2];
        verticesMesh[0] = std::dynamic_pointer_cast<Ind>(ind)->getSkeletonListVertices().data();
        verticesSizesMesh[0] = std::dynamic_pointer_cast<Ind>(ind)->getSkeletonListVertices().size();
        indicesMesh[0] = std::dynamic_pointer_cast<Ind>(ind)->getSkeletonListIndices().data();
        indicesSizesMesh[0] = std::dynamic_pointer_cast<Ind>(ind)->getSkeletonListIndices().size();

        std::stringstream filepath;
        filepath << folder << "/waiting_to_be_built/mesh_" << id << ".stl";

        //fileformat: the fileformat to export to:
        //  0: OBJ format, 3: TEXT STL format, 4: BINARY STL format, 5: COLLADA format, 6: TEXT PLY format, 7: BINARY PLY format
        simExportMesh(3, filepath.str().c_str(), 0, 1.0f, 1, verticesMesh, verticesSizesMesh, indicesMesh, indicesSizesMesh, nullptr, nullptr);

        delete[] verticesMesh;
        delete[] verticesSizesMesh;
        delete[] indicesMesh;
        delete[] indicesSizesMesh;
    }
}

}//ioh
}//are

#endif //PHY_IO_HELPERS_HPP
