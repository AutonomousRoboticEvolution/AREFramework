#include "simulatedER/sq_genome.hpp"
#include <PolyVox/MarchingCubesSurfaceExtractor.h>
#include <pcl/features/normal_3d.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/filters/voxel_grid.h>
using namespace are;

double comp_mut_params::_position_mutation_rate = 0.1;
double comp_mut_params::_type_mutation_rate = 0.1;
double comp_mut_params::_modify_comp_list_mutation_rate = 0.05;
double comp_mut_params::_add_remove_comp_prob = 0.5;

std::string cg::components_genome_from_file(std::string &filename, int id){
        std::ifstream ifs(filename);
    std::string line, res;
    std::vector<std::string> split_line;
    while(std::getline(ifs,line)){
        misc::split_line(line,"-",split_line); 
        if(id == std::stoi(split_line[0])){
            res = split_line[1];
            return res;
        }
    }
    std::cerr << "Loading components error --- id " << id << " not found." << std::endl;
    return "";
}

void SQGenome::random(){
    components_genome.random(randomNum);
    quadric.random(randomNum);
}

void SQGenome::mutate(){
    components_genome.mutate(randomNum);
    quadric.mutate(randomNum);
}

void SQGenome::crossover(const Genome::Ptr &partner,Genome::Ptr child) {
    cg_t cg_child;
    cg_t cg_partner = std::dynamic_pointer_cast<SQGenome>(partner)->get_components_genome();
    components_genome.crossover(cg_partner,cg_child,randomNum);

    sq_t quadric_child;
    sq_t quadric_partner = std::dynamic_pointer_cast<SQGenome>(partner)->get_quadric();
    quadric.crossover(quadric_partner,quadric_child,randomNum);
    *std::dynamic_pointer_cast<SQGenome>(child) = SQGenome(cg_child,quadric_child);
    child->set_parameters(parameters);
    child->set_randNum(randomNum);
    std::dynamic_pointer_cast<SQGenome>(child)->set_parents_ids({_id,partner->id()});
}

void SQGenome::symmetrical_crossover(const Genome::Ptr &partner,Genome::Ptr child1,Genome::Ptr child2){
    cg_t cg_child1, cg_child2;
    cg_t cg_partner = std::dynamic_pointer_cast<SQGenome>(partner)->get_components_genome();
    components_genome.crossover(cg_partner,cg_child1,randomNum);
    cg_partner.crossover(components_genome,cg_child2,randomNum);

    sq_t quadric_child1, quadric_child2;
    sq_t quadric_partner = std::dynamic_pointer_cast<SQGenome>(partner)->get_quadric();
    quadric.crossover(quadric_partner,quadric_child1,randomNum);
    quadric_partner.crossover(quadric,quadric_child2,randomNum);

    *std::dynamic_pointer_cast<SQGenome>(child1) = SQGenome(cg_child1,quadric_child1);
    *std::dynamic_pointer_cast<SQGenome>(child2) = SQGenome(cg_child2,quadric_child2);
    child1->set_parameters(parameters);
    child2->set_parameters(parameters);
    child1->set_randNum(randomNum);
    child2->set_randNum(randomNum);
    std::dynamic_pointer_cast<SQGenome>(child1)->set_parents_ids({_id,partner->id()});
    std::dynamic_pointer_cast<SQGenome>(child2)->set_parents_ids({partner->id(),_id});
}

void SQGenome::from_string(const std::string &str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<SQGenome>();
    iarch >> *this;
}

std::string SQGenome::to_string() const{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<SQGenome>();
    oarch << *this;
    return sstream.str();
}

void sq_decoder::decode(const sq_t &quadric,
                        cg_t &comp_gen,
                        skeleton::type& skeleton,
                        organ_list_t &organ_list,
                        int &number_voxels){
    generate_skeleton(quadric,skeleton);
    skeleton::create_base(skeleton);
    skeleton::empty_space_for_head(skeleton);
    skeleton::remove_skeleton_regions(skeleton);
    skeleton::count_number_voxels(skeleton,number_voxels);
    pcl::PointCloud<pcl::PointNormal>::Ptr sites_locations = std::make_shared<pcl::PointCloud<pcl::PointNormal>>();
    generate_organs_sites(skeleton,sites_locations);
    generate_organ_list(comp_gen,sites_locations,organ_list);
}

void sq_decoder::generate_skeleton(const sq_t &quadric,
                                   skeleton::type& skeleton){
    PolyVox::Region region = skeleton.getEnclosingRegion();

    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                double ix = static_cast<double>(x)/static_cast<double>(region.getUpperX());
                double iy = static_cast<double>(y)/static_cast<double>(region.getUpperY());
                double iz = static_cast<double>(z)/static_cast<double>(region.getUpperZ());
                double output = quadric(ix,iy,iz);
                uint8_t voxel;

                if(output <= 1)
                    voxel = morph_const::filled_voxel;
                else
                    voxel = morph_const::empty_voxel;
                skeleton.setVoxel(x,y,z,voxel);
            }
        }
    }
}

void sq_decoder::generate_organ_list(cg_t &comp_gen,
                                     const pcl::PointCloud<pcl::PointNormal>::Ptr site_locations,
                                     organ_list_t &organ_list){
    float number_sites = static_cast<float>(site_locations->size());
    float ratio = number_sites/100;
    std::vector<float> position(3);

    for(int i = 0; i < comp_gen.positions.size(); i++){
        int organ_type = comp_gen.types[i];
        int site_index = static_cast<int>(std::trunc(static_cast<float>(comp_gen.positions[i])*ratio));
        assert(site_index < site_locations->size());
        position[0] = site_locations->at(site_index).x*morph_const::voxel_real_size;
        position[1] = site_locations->at(site_index).y*morph_const::voxel_real_size;
        position[2] = site_locations->at(site_index).z*morph_const::voxel_real_size;
        position[2] += morph_const::matrix_size/2 * morph_const::voxel_real_size;
        std::vector<float> orientation(3);
        generate_orientation(site_locations->at(site_index).normal_x,
                             site_locations->at(site_index).normal_y,
                             site_locations->at(site_index).normal_z,orientation);
        organ_info organ(organ_type, position, orientation);
        organ_list.push_back(organ);
    }
}

void sq_decoder::generate_organs_sites(skeleton::type &skeleton, pcl::PointCloud<pcl::PointNormal>::Ptr sites_locations){
    auto sign = [](float x) -> int {return (x>0) - (x < 0);};
    PolyVox::Mesh<PolyVox::MarchingCubesVertex<uint8_t>> mesh =
        PolyVox::extractMarchingCubesMesh<skeleton::type>(&skeleton,skeleton.getEnclosingRegion());
    PolyVox::Mesh<PolyVox::Vertex<uint8_t>> surface_mesh = PolyVox::decodeMesh(mesh);
    pcl::PointCloud<pcl::PointNormal>::Ptr surface_cloud = std::make_shared<pcl::PointCloud<pcl::PointNormal>>();
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
        surface_cloud->push_back(pcl::PointNormal(x,y,z,nx,ny,0));
    }
    pcl::PointCloud<pcl::PointNormal>::Ptr filtered_cloud = std::make_shared<pcl::PointCloud<pcl::PointNormal>>();

    pcl::VoxelGrid<pcl::PointNormal> voxel_grid;
    voxel_grid.setInputCloud(surface_cloud);
    voxel_grid.setLeafSize(1.5,1.5,1.5);
    voxel_grid.filter(*filtered_cloud);
    for(auto point: *filtered_cloud){
        int nx = fabs(point.normal_x) > 0.5 ? 1*sign(point.normal_x) : 0;
        int ny = fabs(point.normal_y) > 0.5 ? 1*sign(point.normal_y) : 0;
        if(nx != 0 || ny != 0)
        {
            point.normal_x = nx;
            point.normal_y = ny;
            sites_locations->push_back(point);
        }
    }
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

void QuadricsLog::saveLog(EA::Ptr & ea){
    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        if(!openOLogFile(logFileStream, logFile))
            return;
        logFileStream << ea->get_population()[i]->get_morph_genome()->id() << "-"
                      << std::dynamic_pointer_cast<SQGenome>(
                             ea->get_population()[i]->get_morph_genome()
                             )->get_quadric().to_string() << std::endl;
        logFileStream.close();
    }
}

void CompGenLog::saveLog(EA::Ptr & ea){
    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        if(!openOLogFile(logFileStream, logFile))
            return;
        logFileStream << ea->get_population()[i]->get_morph_genome()->id() << "-"
                      << std::dynamic_pointer_cast<SQGenome>(
                             ea->get_population()[i]->get_morph_genome()
                             )->get_components_genome().to_string() << std::endl;
        logFileStream.close();
    }
}

void SQGenomeLog::saveLog(EA::Ptr & ea){
    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::stringstream filename;
        filename << "morph_genome_" << ea->get_population()[i]->get_morph_genome()->id();
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        logFileStream << std::dynamic_pointer_cast<SQGenome>(
                             ea->get_population()[i]->get_morph_genome()
                             )->to_string();
        logFileStream.close();
    }
}

void ParentingLog::saveLog(EA::Ptr & ea){
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream,logFile))
        return;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        logFileStream << ea->get_population()[i]->get_morph_genome()->id() << ","
                      << std::dynamic_pointer_cast<SQGenome>(ea->get_population()[i]->get_morph_genome())->get_parents_ids()[0]
                      << ","
                      << std::dynamic_pointer_cast<SQGenome>(ea->get_population()[i]->get_morph_genome())->get_parents_ids()[1]
                      << std::endl;
    }
    logFileStream.close();
}
