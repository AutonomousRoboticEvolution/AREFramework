#include "simulatedER/nn2/sq_cppn_genome.hpp"

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

float sq_cppn::params::evo_float::mutation_rate = 0.1f;


void sq_cppn_decoder::decode(const sq_t &quadric, cppn_t &cppn, int nbr_organs,
                        skeleton::type &skeleton, organ_list_t &organ_list, int &number_voxels){
    generate_skeleton(quadric,skeleton);
    skeleton::create_base(skeleton);
    skeleton::empty_space_for_head(skeleton);
    skeleton::remove_skeleton_regions(skeleton);
    skeleton::count_number_voxels(skeleton,number_voxels);
    skeleton::coord_t surface_coords;
    skeleton::find_skeleton_surface(skeleton,surface_coords);
    generate_organ_list(cppn,surface_coords,nbr_organs,organ_list);
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

void sq_cppn_decoder::generate_organ_list(cppn_t &cppn, const skeleton::coord_t &surface_coords, int nbr_organs, organ_list_t &organ_list){
    std::vector<double> input{0,0,0}; // Vector used as input of the Neural Network (NN).
    int organ_type;
    organ_list_t  full_list;
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

            organ_type = cppn_to_organ_type(cppn,input);


            // Create organ if any
            if(organ_type > 0){
                std::vector<float> position(3);
                position.at(0) = static_cast<float>(surface_coords[m][n].at(0) * morph_const::voxel_real_size);
                position.at(1) = static_cast<float>(surface_coords[m][n].at(1) * morph_const::voxel_real_size);
                position.at(2) = static_cast<float>(surface_coords[m][n].at(2) * morph_const::voxel_real_size);
                position.at(2) += morph_const::matrix_size/2 * morph_const::voxel_real_size;


                // Gives the direction of the organ given the direction of the surface
                std::vector<float> orientation(3);

                int nx = surface_coords[m][n].at(3),ny = surface_coords[m][n].at(4),nz = surface_coords[m][n].at(5);
                sim::organ::generate_orientation(nx,ny,nz,orientation);
                organ_info organ(organ_type, position, orientation);
                full_list.push_back(organ);
            }
        }
    }

    //Select a maximum of 8 location with the maximum distance between them.
    std::function<float(std::vector<float>,std::vector<float>)> distance =
        [](std::vector<float> v1, std::vector<float> v2) -> float{
        return sqrt((v1[0]-v2[0])*(v1[0]-v2[0])
                    +(v1[1]-v2[1])*(v1[1]-v2[1])
                    +(v1[2]-v2[2])*(v1[2]-v2[2]));
    };
    std::vector<float> distances;
    for(const organ_info &o1: full_list){
        std::vector<float> p1 = o1.position;
        float max_dist = 0;
        for (const organ_info &o2: full_list){
            std::vector<float> p2 = o2.position;
            float dist = distance(p1,p2);
            if(max_dist < dist)
                max_dist = dist;
        }
        distances.push_back(max_dist);
    }
    std::vector<int> indexes(distances.size());
    std::iota(indexes.begin(),indexes.end(),0);
    std::sort(indexes.begin(),indexes.end(),[&](int i, int j){return distances[i]>distances[j];});
    for(int i = 0; i < nbr_organs; i++)
        organ_list.push_back(full_list[indexes[i]]);

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
        nn2_cppn_t cppn = std::dynamic_pointer_cast<SQCPPNGenome>(
                              ea->get_population()[i]->get_morph_genome()
                              )->get_cppn();
        cppn.write_dot(logFileStream);
        logFileStream.close();
    }
}
