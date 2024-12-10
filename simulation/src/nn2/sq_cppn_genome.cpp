#include "simulatedER/nn2/sq_cppn_genome.hpp"
#include <PolyVox/MarchingCubesSurfaceExtractor.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/features/normal_3d.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/filters/voxel_grid.h>

using namespace are;
using namespace are::sq_cppn;
using namespace sim::organ;

quadric_param_t::quadric_param_t(double _a, double _b, double _c,
                                 double _u, double _v,
                                 double _r, double _s, double _t):
        a(_a), b(_b), c(_c), u(_u), v(_v), r(_r), s(_s), t(_t){
}
void quadric_param_t::random(const misc::RandNum::Ptr& rn){
    a += rn->randDouble(-0.5,0.5);
    if(a == 0)
        a = 0.0001;
    b += rn->randDouble(-0.5,0.5);
    if(b == 0)
        b = 0.0001;
    c += rn->randDouble(-0.5,0.5);
    if(c == 0)
        c = 0.0001;
    u += rn->randDouble(-0.5,0.5);
    if(u == 0)
        u = 0.0001;
    v += rn->randDouble(-0.5,0.5);
    if(v == 0)
        v = 0.0001;
    r += rn->randDouble(-0.5,0.5);
    if(r == 0)
        r = 0.0001;
    s += rn->randDouble(-0.5,0.5);
    if(s == 0)
        s = 0.0001;
    t += rn->randDouble(-0.5,0.5);
    if(t == 0)
        t = 0.0001;
}

std::string quadric_param_t::to_string() const{
    std::stringstream sstr;
    sstr << a << ","
         << b << ","
         << c << ","
         << u << ","
         << v << ","
         << r << ","
         << s << ","
         << t;
    return sstr.str();
}

void quadric_param_t::from_string(const std::string& str){
    std::vector<std::string> splitted_str;
    misc::split_line(str,",",splitted_str);
    a = std::stod(splitted_str[0]);
    b = std::stod(splitted_str[1]);
    c = std::stod(splitted_str[2]);
    u = std::stod(splitted_str[3]);
    v = std::stod(splitted_str[4]);
    r = std::stod(splitted_str[5]);
    s = std::stod(splitted_str[6]);
    t = std::stod(splitted_str[7]);
}

int sq_cppn::params::cppn::_mutation_type = 0; //uniform
bool sq_cppn::params::cppn::_mutate_connections = true;
bool sq_cppn::params::cppn::_mutate_neurons = true;
float sq_cppn::params::cppn::_mutation_rate = 0.5f;
float sq_cppn::params::cppn::_rate_mutate_conn = 0.1f;
float sq_cppn::params::cppn::_rate_mutate_neur = 0.1f;
float sq_cppn::params::cppn::_rate_add_neuron = 0.1f;
float sq_cppn::params::cppn::_rate_del_neuron = 0.1f;
float sq_cppn::params::cppn::_rate_add_conn = 0.1f;
float sq_cppn::params::cppn::_rate_del_conn = 0.1f;
float sq_cppn::params::cppn::_rate_change_conn = 0.1f;
float sq_cppn::params::cppn::_rate_crossover = 0.5f;

size_t sq_cppn::params::cppn::_min_nb_neurons = 0;
size_t sq_cppn::params::cppn::_max_nb_neurons = 2;
size_t sq_cppn::params::cppn::_min_nb_conns = 10;
size_t sq_cppn::params::cppn::_max_nb_conns = 100;

double sq_cppn::params::cppn::_expressiveness = 10.f;

float sq_cppn::params::evo_float::mutation_rate = 0.1f;


void sq_cppn_decoder::decode(const sq_t &quadric, cppn_t &cppn, int nbr_organs,
                        skeleton::type &skeleton, organ_list_t &organ_list, int &number_voxels){
    generate_skeleton(quadric,skeleton);
    skeleton::create_base(skeleton);
    skeleton::empty_space_for_head(skeleton);
    skeleton::remove_skeleton_regions(skeleton);
    skeleton::count_number_voxels(skeleton,number_voxels);
    pcl::PointCloud<pcl::PointNormal>::Ptr sites_locations(new pcl::PointCloud<pcl::PointNormal>());
    generate_organs_sites(skeleton,sites_locations);
    generate_organ_list(cppn,sites_locations,nbr_organs,organ_list);
}

void sq_cppn_decoder::generate_skeleton(const sq_t &quadric, skeleton::type& skeleton){
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

void sq_cppn_decoder::generate_organs_sites(skeleton::type &skeleton, pcl::PointCloud<pcl::PointNormal>::Ptr &sites_locations){
    auto sign = [](float x) -> int {return (x>0) - (x < 0);};
    PolyVox::Mesh<PolyVox::MarchingCubesVertex<uint8_t>> mesh =
        PolyVox::extractMarchingCubesMesh<skeleton::type>(&skeleton,skeleton.getEnclosingRegion());
    PolyVox::Mesh<PolyVox::Vertex<uint8_t>> surface_mesh= PolyVox::decodeMesh(mesh);
    pcl::PointCloud<pcl::PointNormal>::Ptr surface_cloud(new pcl::PointCloud<pcl::PointNormal>());
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

    pcl::VoxelGrid<pcl::PointNormal> voxel_grid;
    voxel_grid.setInputCloud(surface_cloud);
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

}

void sq_cppn_decoder::generate_organ_list(cppn_t &cppn, const pcl::PointCloud<pcl::PointNormal>::Ptr & site_locations, int nbr_organs, organ_list_t &organ_list){
    std::function<float(std::vector<float>,std::vector<float>)> distance =
        [](std::vector<float> v1, std::vector<float> v2) -> float{
        return sqrt((v1[0]-v2[0])*(v1[0]-v2[0])
                    +(v1[1]-v2[1])*(v1[1]-v2[1])
                    +(v1[2]-v2[2])*(v1[2]-v2[2]));
    };

    std::vector<double> input{0,0,0}; // Vector used as input of the Neural Network (NN).
    int organ_type;
    organ_list_t  full_list;
    for(const pcl::PointNormal &site : *site_locations){
        input[0] = static_cast<float>(site.x* morph_const::voxel_real_size);
        input[1] = static_cast<float>(site.y* morph_const::voxel_real_size);
        input[2] = static_cast<float>(site.z* morph_const::voxel_real_size);


        organ_type = cppn_to_organ_type(cppn,input);
        if(organ_type > 0){
            std::vector<float> position(3);
            position.at(0) = input[0];
            position.at(1) = input[1];
            position.at(2) = input[2];
            position.at(2) += morph_const::matrix_size/2 * morph_const::voxel_real_size;
            //float pi_2 = static_cast<float>(M_PI_2);
            std::vector<float> orientation(3);//{site.normal_x*pi_2,site.normal_y*pi_2,site.normal_z*pi_2};
            generate_orientation(site.normal_x,site.normal_y,site.normal_z,orientation);

            // Gives the direction of the organ given the direction of the surface
            organ_info organ(organ_type, position, orientation);
            organ_list.push_back(organ);
        }
    }
    //std::cout << full_list.size() << " organ placed" << std::endl;
    // organ_list = full_list;
 

    //Select a maximum of 8 location with the maximum distance between them.

    // std::vector<float> distances;
    // for(const organ_info &o1: full_list){
    //     std::vector<float> p1 = o1.position;
    //     float mean_dist = 0;
    //     for (const organ_info &o2: full_list){
    //         std::vector<float> p2 = o2.position;
    //         mean_dist += distance(p1,p2);
    //     }
    //     distances.push_back(mean_dist/static_cast<float>(full_list.size()-1));
    // }
    // std::vector<int> indexes(distances.size());
    // std::iota(indexes.begin(),indexes.end(),0);
    // std::sort(indexes.begin(),indexes.end(),[&](int i, int j){return distances[i]>distances[j];});
    // for(int i = 0; i < nbr_organs; i++)
    //     organ_list.push_back(full_list[indexes[i]]);

}

int sq_cppn_decoder::cppn_to_organ_type(cppn_t &cppn,const std::vector<double> &input){
    int organ_type = -1;
    cppn.step(input);
    double output = cppn.outf()[0] + 1;


    // Is there an organ?
    if(output < 0.4) //no organ
        organ_type = -1;
    else if(output >= 0.4 && output < 0.8) // Wheel
        organ_type = 1;
    else if(output >= 0.8 && output < 1.2) // Sensor
        organ_type = 2;
    else if(output >= 1.2 && output < 1.6)// Joint
        organ_type = 3;
    else if(output >= 1.6) // Caster
        organ_type = 4;
    else{
        std::cerr << "We shouldn't be here: " << __func__ << " max_element: " << output << std::endl;
        exit(-1);
    }
    return organ_type;
}

void QuadricsLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        if(!openOLogFile(logFileStream, logFile))
            return;
        logFileStream << ea->get_population()[i]->get_morph_genome()->id() << ";"
                      << std::dynamic_pointer_cast<SQCPPNGenome>(
                            ea->get_population()[i]->get_morph_genome()
                        )->get_quadric().to_string() << std::endl;
        logFileStream.close();
    }
}

void SQCPPNGenomeLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::stringstream filename;
        filename << "morph_genome_" << ea->get_population()[i]->get_morph_genome()->id();
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        logFileStream << std::dynamic_pointer_cast<SQCPPNGenome>(
                             ea->get_population()[i]->get_morph_genome()
                             )->to_string();
        logFileStream.close();
    }
}

void NbrConnNeurLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream,logFile))
        return;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        logFileStream << ea->get_population()[i]->get_morph_genome()->id() << ","
                      << std::dynamic_pointer_cast<SQCPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_nb_neurons()
                      << ","
                      << std::dynamic_pointer_cast<SQCPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_nb_connections()
                      << std::endl;
    }
    logFileStream.close();
}

void ParentingLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream,logFile))
        return;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        logFileStream << ea->get_population()[i]->get_morph_genome()->id() << ","
                      << std::dynamic_pointer_cast<SQCPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_parents_ids()[0]
                      << ","
                      << std::dynamic_pointer_cast<SQCPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_parents_ids()[1]
                      << std::endl;
    }
    logFileStream.close();
}

void GraphVizLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::stringstream filename;
        filename << "cppn_" << std::dynamic_pointer_cast<SQCPPNGenome>(
                                   ea->get_population()[i]->get_morph_genome()
                                   )->id() << ".dot";
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        sq_cppn::cppn_t cppn = std::dynamic_pointer_cast<SQCPPNGenome>(
                              ea->get_population()[i]->get_morph_genome()
                              )->get_cppn();
        cppn.write_dot(logFileStream);
        logFileStream.close();
    }
}
