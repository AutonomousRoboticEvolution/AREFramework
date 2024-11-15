#include "simulatedER/nn2/sq_cppn_genome.hpp"

using namespace are;
using namespace are::sq_cppn;

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

float sq_cppn::params::evo_float::mutation_rate = 0.1f;


void sq_cppn_decoder::decode(const sq_t &quadric, const cppn_t &cppn,
                        skeleton::type &skeleton, organ_list_t &organ_list, int &number_voxels){
    generate_skeleton(quadric,skeleton);
    skeleton::create_base(skeleton);
    skeleton::empty_space_for_head(skeleton);
    skeleton::remove_skeleton_regions(skeleton);
    skeleton::count_number_voxels(skeleton,number_voxels);
    skeleton::coord_t surface_coords;
    skeleton::find_skeleton_surface(skeleton,surface_coords);

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

void sq_cppn_decoder::generate_organ_list(const cppn_t &cppn, const skeleton::coord_t &surface_coords, organ_list_t &organ_list){
    std::vector<double> input{0,0,0}; // Vector used as input of the Neural Network (NN).
    std::vector<double> output;
    int organ_type;
    for(int m = 0; m < surface_coords.size(); m++) {
        // This sorts the vectors along the z-axis. This allows to generate organs from the bottom of the robot to the top hence reducing the number of useless organs.
        // Solution taken from https://stackoverflow.com/questions/45494567/c-how-to-sort-the-rows-of-a-2d-vector-by-the-values-in-each-rows-column
        std::sort(surface_coords[m].begin(),
                  surface_coords[m].end(),
                  [] (const std::vector<int> &a, const std::vector<int> &b)
                  {
                      return a[2] < b[2];
                  });
        for (int n = 0; n < surface_coords[m].size(); n+=1) { /// \todo EB: Define this constant elsewhere!
            double x = static_cast<double>(surface_coords[m][n].at(0))/static_cast<double>(morph_const::real_matrix_size/2);
            double y = static_cast<double>(surface_coords[m][n].at(1))/static_cast<double>(morph_const::real_matrix_size/2);
            double z = static_cast<double>(surface_coords[m][n].at(2))/static_cast<double>(morph_const::real_matrix_size/2);
            input[0] = x;
            input[1] = y;
            input[2] = z;
            organ_type = get_organ_from_cppn(input); // TO DO decoding of cppn
            // Create organ if any
            if(organ_type > 0){
                std::vector<double> position(3);
                position.at(0) = static_cast<double>(surface_coords[m][n].at(0) * morph_const::voxel_real_size);
                position.at(1) = static_cast<double>(surface_coords[m][n].at(1) * morph_const::voxel_real_size);
                position.at(2) = static_cast<double>(surface_coords[m][n].at(2) * morph_const::voxel_real_size);
                position.at(2) += morph_const::matrix_size/2 * morph_const::voxel_real_size;


                // Gives the direction of the organ given the direction of the surface
                std::vector<double> orientation(3);
                int nx = surface_coords[m][n].at(3),ny = surface_coords[m][n].at(4),nz = surface_coords[m][n].at(5);
                if ((nx < 0) && (ny == 0) && (nz == 0)){
                    orientation.at(0) = +0.0; orientation.at(1) = +M_PI_2; orientation.at(2) = +0.0;
                } else if ((nx == 0) && (ny < 0) && (nz == 0)){
                    orientation.at(0) = -M_PI_2; orientation.at(1) = +0.0; orientation.at(2) = +M_PI_2;
                } else if ((nx == 0) && (ny > 0) && (nz == 0)){
                    orientation.at(0) = +M_PI_2; orientation.at(1) = +0.0; orientation.at(2) = -M_PI_2;
                } else if ((nx > 0) && (ny == 0) && (nz == 0)) {
                    orientation.at(0) = +0.0; orientation.at(1) = -M_PI_2; orientation.at(2) = +M_PI;
                } else {
                    orientation.at(0) = +0.6154; orientation.at(1) = -0.5236; orientation.at(2) = -2.1862;
                    std::cerr << "We shouldn't be here: " << __func__ << " " << x << " "
                              << y << " " << z << std::endl;
                }
                organ_info organ(organ_type, position, orientation);
                organ_list.push_back(organ);
            }
        }
    }

    //Select a maximum of 8 location with the maximum distance between them.

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
        nn2_cppn_t cppn = std::dynamic_pointer_cast<SQCPPNGenome>(
                              ea->get_population()[i]->get_morph_genome()
                              )->get_cppn();
        cppn.write_dot(logFileStream);
        logFileStream.close();
    }
}
