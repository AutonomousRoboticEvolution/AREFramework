#include "simulatedER/nn2/sq_genome.hpp"
#include <PolyVox/MarchingCubesSurfaceExtractor.h>
#include <pcl/features/normal_3d.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/filters/voxel_grid.h>
#define EIGEN_MAX_ALIGN_BYTES 0
using namespace are;

double comp_genome_params::_position_mutation_rate = 0.1;
double comp_genome_params::_type_mutation_rate = 0.1;
double comp_genome_params::_modify_comp_list_mutation_rate = 0.05;
double comp_genome_params::_add_remove_comp_prob = 0.5;


void sq_decoder::generate_organs_sites(skeleton::type &skeleton, pcl::PointCloud<pcl::PointNormal>::Ptr &sites_locations){
    auto sign = [](float x) -> int {return (x>0) - (x < 0);};
    PolyVox::Mesh<PolyVox::MarchingCubesVertex<uint8_t>> mesh =
        PolyVox::extractMarchingCubesMesh<skeleton::type>(&skeleton,skeleton.getEnclosingRegion());
    PolyVox::Mesh<PolyVox::Vertex<uint8_t>> surface_mesh = PolyVox::decodeMesh(mesh);
    pcl::PointCloud<pcl::PointNormal> surface_cloud;// = std::make_shared<pcl::PointCloud<pcl::PointNormal>>();
    std::cout << "number of points " << surface_mesh.getNoOfVertices() << std::endl;
    for(int i = 0; i < surface_mesh.getNoOfVertices(); i++){
        PolyVox::Vertex<uint8_t> vertex = surface_mesh.getVertex(i);
        int nx = fabs(vertex.normal.getX()) > 0.5 ? 1*sign(vertex.normal.getX()) : 0;
        int ny = fabs(vertex.normal.getY()) > 0.5 ? 1*sign(vertex.normal.getY()) : 0;
        if(nx == 0 && ny == 0)
            continue;
        int x = vertex.position.getX() - morph_const::matrix_size/2;
        int y = vertex.position.getY() - morph_const::matrix_size/2;
        int z = vertex.position.getZ() - morph_const::matrix_size/2;
        if(x <= morph_const::xHeadUpperLimit && x >= morph_const::xHeadLowerLimit
            && y <= morph_const::yHeadUpperLimit && y >= morph_const::yHeadLowerLimit
            && z <= morph_const::zHeadUpperLimit && z >= morph_const::zHeadLowerLimit)
            continue;
        if(x >= 5 || x <= -5 || y >= 5 || y <= -5)
            continue;
        surface_cloud.push_back(pcl::PointNormal(x,y,z,nx,ny,0));
    }

    pcl::VoxelGrid<pcl::PointNormal> voxel_grid;
    voxel_grid.setInputCloud(std::make_shared<pcl::PointCloud<pcl::PointNormal>>(surface_cloud));
    voxel_grid.setLeafSize(1.5,1.5,1.5);
    voxel_grid.filter(*sites_locations);
    std::vector<int> to_erase;
    for(int i = 0; i < sites_locations->size(); i++){
        int nx = fabs(sites_locations->at(i).normal_x) > 0.5 ? 1*sign(sites_locations->at(i).normal_x) : 0;
        int ny = fabs(sites_locations->at(i).normal_y) > 0.5 ? 1*sign(sites_locations->at(i).normal_y) : 0;
        if(nx == 0 && ny == 0)
            to_erase.push_back(i);
        sites_locations->points[i].normal_x = nx;
        sites_locations->points[i].normal_y = ny;
    }
    for(int i : to_erase)
        sites_locations->erase(sites_locations->begin()+i);
    std::cout << "number of sites " << sites_locations->size() << std::endl;



    //uncomment to visulize the sites for organs placement
    // pcl::visualization::PCLVisualizer::Ptr viewer (new pcl::visualization::PCLVisualizer ("3D Viewer"));
    // viewer->setBackgroundColor (0, 0, 0);
    // viewer->addPointCloud<pcl::PointNormal>(sites_locations,"site locations");
    // viewer->addPointCloud<pcl::PointNormal>(surface_cloud,"surface locations");
    // viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE,5.0, "surface locations");
    // viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE,10.0, "site locations");
    // viewer->addPointCloudNormals<pcl::PointNormal>(sites_locations,1,1,"site normals");
    // viewer->addCoordinateSystem(1,0,0,0);
    // while (!viewer->wasStopped ())
    // {
    //     viewer->spinOnce (100);
    // }
    // surface_cloud.reset();
}

using namespace are::sq_genome;

void QuadricsLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        if(!openOLogFile(logFileStream, logFile))
            return;
        logFileStream << ea->get_population()[i]->get_morph_genome()->id() << "-"
                      << std::dynamic_pointer_cast<sq_genome_t>(
                             ea->get_population()[i]->get_morph_genome()
                             )->get_quadric().to_string() << std::endl;
        logFileStream.close();
    }
}

void CompGenLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        if(!openOLogFile(logFileStream, logFile))
            return;
        logFileStream << ea->get_population()[i]->get_morph_genome()->id() << "-"
                      << std::dynamic_pointer_cast<sq_genome_t>(
                             ea->get_population()[i]->get_morph_genome()
                             )->get_components_genome().to_string() << std::endl;
        logFileStream.close();
    }
}


void SQGenomeLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::stringstream filename;
        filename << "morph_genome_" << ea->get_population()[i]->get_morph_genome()->id();
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        logFileStream << std::dynamic_pointer_cast<sq_genome_t>(
                             ea->get_population()[i]->get_morph_genome()
                             )->to_string();
        logFileStream.close();
    }
}



void ParentingLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream,logFile))
        return;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        logFileStream << ea->get_population()[i]->get_morph_genome()->id() << ","
                      << std::dynamic_pointer_cast<sq_genome_t>(ea->get_population()[i]->get_morph_genome())->get_parents_ids()[0]
                      << ","
                      << std::dynamic_pointer_cast<sq_genome_t>(ea->get_population()[i]->get_morph_genome())->get_parents_ids()[1]
                      << std::endl;
    }
    logFileStream.close();
}


