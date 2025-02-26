#include "simulatedER/nn2/NN2CPPNGenome.hpp"

using namespace are;
using namespace nn2_cppn;
using namespace sim::organ;

std::mt19937 nn2::rgen_t::gen;

int cppn_params::cppn::_mutation_type = 0; //uniform
bool cppn_params::cppn::_mutate_connections = true;
bool cppn_params::cppn::_mutate_neurons = true;
float cppn_params::cppn::_mutation_rate = 0.5f;
float cppn_params::cppn::_rate_mutate_conn = 0.1f;
float cppn_params::cppn::_rate_mutate_neur = 0.1f;
float cppn_params::cppn::_rate_add_neuron = 0.1f;
float cppn_params::cppn::_rate_del_neuron = 0.1f;
float cppn_params::cppn::_rate_add_conn = 0.1f;
float cppn_params::cppn::_rate_del_conn = 0.1f;
float cppn_params::cppn::_rate_change_conn = 0.1f;
float cppn_params::cppn::_rate_crossover = 0.5f;

size_t cppn_params::cppn::_min_nb_neurons = 0;
size_t cppn_params::cppn::_max_nb_neurons = 5;
size_t cppn_params::cppn::_min_nb_conns = 10;
size_t cppn_params::cppn::_max_nb_conns = 100;

double cppn_params::cppn::_expressiveness = 10.f;

int cppn_params::cppn::nb_inputs = 4;
int cppn_params::cppn::nb_outputs = 6;

float cppn_params::evo_float::mutation_rate = 0.1f;

void nn2_cppn_decoder::decode(nn2_cppn_t &cppn,
                              skeleton::type& skeleton,
                              organ_list_t &organ_list,
                              int &number_voxels,
                              bool growing)
{
    if(growing)
        generate_skeleton_growing(cppn,skeleton);
    else
        generate_skeleton(cppn,skeleton);
    skeleton::create_base(skeleton);
    skeleton::empty_space_for_head(skeleton);
    skeleton::remove_skeleton_regions(skeleton);
    skeleton::remove_hoverhangs(skeleton);
    skeleton::count_number_voxels(skeleton,number_voxels);
    skeleton::coord_t surface_coords;
    skeleton::find_skeleton_surface(skeleton,surface_coords);
    generate_organ_list(cppn,surface_coords,organ_list);
}

void nn2_cppn_decoder::generate_skeleton(nn2_cppn_t &cppn,skeleton::type &skeleton){
    PolyVox::Region region = skeleton.getEnclosingRegion();
    skeleton::type tmp_skeleton(region);
    std::vector<double> input{0,0,0,0}; // Vector used as input of the Neural Network (NN).
    std::vector<double> outputs;
    uint8_t voxel;
    // Generate voxel matrix
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                input[0] = static_cast<double>(x);
                input[1] = static_cast<double>(y);
                input[2] = static_cast<double>(z);
                input[3] = static_cast<double>(sqrt(pow(x,2)+pow(y,2)+pow(z,2)));

                // Activate NN
                cppn.step(input);
                outputs = cppn.outf();
                for(auto& o: outputs)
                    if(std::isnan(o))
                        o = 0;

                // Take output from NN and store it.
                voxel = morph_const::empty_voxel;
                if(outputs[1] > 0.00001) { // Sometimes values are very close to zero and these causes problems.
                    voxel = morph_const::filled_voxel;
                }
                tmp_skeleton.setVoxel(x, y, z, voxel);
            }
        }
    }

    uint8_t areVoxel;
    uint8_t uVoxelValue;
    bool isSkeletonConnected = false;
    for(int32_t z = region.getLowerZ()+1; z < region.getLowerZ() + morph_const::skeletonBaseHeight; z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                if(x <= morph_const::xHeadUpperLimit + morph_const::skeletonBaseThickness && x >= morph_const::xHeadLowerLimit - morph_const::skeletonBaseThickness &&
                    y <= morph_const::yHeadUpperLimit + morph_const::skeletonBaseThickness && y >= morph_const::yHeadLowerLimit - morph_const::skeletonBaseThickness){
                    if(x <= morph_const::xHeadUpperLimit && x >= morph_const::xHeadLowerLimit &&
                        y <= morph_const::yHeadUpperLimit && y >= morph_const::yHeadLowerLimit){
                        //isSkeletonConnected = false;
                    }
                    else{
                        areVoxel = tmp_skeleton.getVoxel(x, y, z);
                        // If output greater than threshold write voxel.
                        // NOTE: Hard boundaries seem to work better with convex decomposition
                        voxel = areVoxel;

                        if(voxel > 0.5){
                            isSkeletonConnected = true;
                        }
                    }
                }
            }
        }
    }
    if(isSkeletonConnected){
        for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
            for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
                for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                    areVoxel = tmp_skeleton.getVoxel(x, y, z);
                    // If output greater than threshold write voxel.
                    // NOTE: Hard boundaries seem to work better with convex decomposition
                    voxel = areVoxel;

                    if(voxel > 0.5)
                        uVoxelValue = morph_const::filled_voxel;
                    else
                        uVoxelValue = morph_const::empty_voxel;

                    skeleton.setVoxel(x, y, z, uVoxelValue);
                }
            }
        }
    }
}

void nn2_cppn_decoder::generate_skeleton_growing(nn2_cppn_t &cppn,skeleton::type &skeleton){

    std::function<void(int32_t,int32_t,int32_t,skeleton::type&,nn2_cppn_t&)> assign_skeleton_voxel =
        [&](int32_t x,int32_t y,int32_t z,skeleton::type& skeleton,nn2_cppn_t &cppn){
            //- Test if the coordinate are out of bound, if yes exit
            PolyVox::Region region = skeleton.getEnclosingRegion();
            if(x < region.getLowerX() || x > region.getUpperX() ||
                y < region.getLowerY() || y > region.getUpperX() ||
                z < region.getLowerZ() || z > region.getUpperX())
                return;
            //- check if this voxel is aready assigned
            if(skeleton.getVoxel(x,y,z) != 128)
                return;

            double ix = static_cast<double>(x)/static_cast<double>(region.getUpperX());
            double iy = static_cast<double>(y)/static_cast<double>(region.getUpperY());
            double iz = static_cast<double>(z)/static_cast<double>(region.getUpperZ());
            std::vector<double> input{ix, iy, iz, sqrt(ix*ix+iy*iy+iz*iz)}; // Vector used as input of the Neural Network (NN)
            cppn.step(input);
            uint8_t voxel;
            if(cppn.outf()[1] > 0.000001){ //test if value is positive
                voxel= morph_const::filled_voxel;
                skeleton.setVoxel(x,y,z,voxel);
                assign_skeleton_voxel(x+1,y,z,skeleton,cppn);
                assign_skeleton_voxel(x-1,y,z,skeleton,cppn);
                assign_skeleton_voxel(x,y+1,z,skeleton,cppn);
                assign_skeleton_voxel(x,y-1,z,skeleton,cppn);
                assign_skeleton_voxel(x,y,z+1,skeleton,cppn);
                assign_skeleton_voxel(x,y,z-1,skeleton,cppn);
            }else{
                voxel = morph_const::empty_voxel;
                skeleton.setVoxel(x,y,z,voxel);
            }
        };

    std::cout << "Growth of skeleton" << std::endl;
    PolyVox::Region region = skeleton.getEnclosingRegion();
    // int32_t centre_x = region.getCentreX();
    // int32_t centre_y = region.getCentreY();
    int32_t centre_z = region.getCentreZ();
    // start from the base skeleton.
    for(int x = morph_const::xHeadLowerLimit - morph_const::skeletonBaseThickness; x < morph_const::xHeadUpperLimit + morph_const::skeletonBaseThickness; x++){
        for(int y = morph_const::yHeadLowerLimit - morph_const::skeletonBaseThickness; y < morph_const::yHeadUpperLimit + morph_const::skeletonBaseThickness; y++){
            if(x <= morph_const::xHeadUpperLimit && x >= morph_const::xHeadLowerLimit &&
                y <= morph_const::yHeadUpperLimit && y >= morph_const::yHeadLowerLimit)
                continue;
            assign_skeleton_voxel(x,y,centre_z,skeleton,cppn);
        }
    }

    std::vector<std::tuple<int32_t,int32_t,int32_t>> voxels_to_be_filled;

    //- Fill up holes in the skeleton and assigne empty voxel value to non assigned voxels
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                uint8_t voxel = skeleton.getVoxel(x,y,z);
                if(voxel == 128){
                    voxel = morph_const::empty_voxel;
                    skeleton.setVoxel(x,y,z,voxel);
                }else if(voxel == morph_const::empty_voxel){
                    int counter = 0;
                    if(x+1 < region.getUpperX() && skeleton.getVoxel(x+1,y,z) == morph_const::filled_voxel)
                        counter++;
                    if(x-1 > region.getLowerX() && skeleton.getVoxel(x-1,y,z) == morph_const::filled_voxel)
                        counter++;
                    if(y+1 < region.getUpperY() && skeleton.getVoxel(x,y+1,z) == morph_const::filled_voxel)
                        counter++;
                    if(y-1 > region.getLowerY() && skeleton.getVoxel(x,y-1,z) == morph_const::filled_voxel)
                        counter++;
                    if(z+1 < region.getUpperZ() && skeleton.getVoxel(x,y,z+1) == morph_const::filled_voxel)
                        counter++;
                    if(z-1 > region.getLowerZ() && skeleton.getVoxel(x,y,z-1) == morph_const::filled_voxel)
                        counter++;
                    if(counter >= 3)
                        voxels_to_be_filled.push_back({x,y,z});

                }
            }
        }
    }


    for(const std::tuple<int32_t,int32_t,int32_t> &coord: voxels_to_be_filled){
        uint8_t voxel = morph_const::filled_voxel;;
        skeleton.setVoxel(get<0>(coord),get<1>(coord),get<2>(coord),voxel);
    }

}

void nn2_cppn_decoder::generate_organ_list(nn2_cppn_t &cppn,
                         skeleton::coord_t &surface_coords,
                         organ_list_t &organ_list){
    std::vector<double> input{0,0,0,0}; // Vector used as input of the Neural Network (NN).
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
            input[3] = sqrt(x*x+y*y+z*z);
            organ_type = cppn_to_organ_type(cppn,input);
            // Create organ if any
            if(organ_type > 0){
                std::vector<float> tempPosVector(3);
                tempPosVector.at(0) = static_cast<float>(surface_coords[m][n].at(0) * morph_const::voxel_real_size);
                tempPosVector.at(1) = static_cast<float>(surface_coords[m][n].at(1) * morph_const::voxel_real_size);
                tempPosVector.at(2) = static_cast<float>(surface_coords[m][n].at(2) * morph_const::voxel_real_size);
                tempPosVector.at(2) += morph_const::matrix_size/2 * morph_const::voxel_real_size;
                std::vector<float> tempOriVector(3);
                generate_orientation(surface_coords[m][n].at(3), surface_coords[m][n].at(4), surface_coords[m][n].at(5), tempOriVector);
                organ_info organ(organ_type, tempPosVector, tempOriVector);
                organ_list.push_back(organ);
            }
        }
    }
}

int nn2_cppn_decoder::cppn_to_organ_type(nn2_cppn_t &cppn,const std::vector<double> &input){
    int organ_type = -1;
    std::vector<double> output;
    cppn.step(input);
    output = cppn.outf();
    for(auto& o: output)
        if(std::isnan(o))
            o = 0;
    // Is there an organ?
    organ_type = -1;
    int max_element = -1;
    max_element = std::max_element(output.begin()+2, output.end()) - output.begin();
    if(max_element == 2){ // Wheel
        // These if statements should be always true but they are here for debugging.
        //if(settings::getParameter<settings::Boolean>(parameters,"#isWheel").value) // For debugging only
        organ_type = 1;
    }
    else if(max_element == 3) { // Sensor
        // if(settings::getParameter<settings::Boolean>(parameters,"#isSensor").value) // For debugging only
        organ_type = 2;
    }
    else if(max_element == 4) { // Joint
        //  if(settings::getParameter<settings::Boolean>(parameters,"#isJoint").value) // For debugging only
        organ_type = 3;
    }
    else if(max_element == 5) { // Caster
        //        if(settings::getParameter<settings::Boolean>(parameters,"#isCaster").value) // For debugging only
        organ_type = 4;
    } else{
        std::cerr << "We shouldn't be here: " << __func__ << " max_element: " << max_element << std::endl;
        assert(false);
    }
    return organ_type;
}


void NN2CPPNGenomeLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::stringstream filename;
        filename << "morph_genome_" << ea->get_population()[i]->get_morph_genome()->id();
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        logFileStream << std::dynamic_pointer_cast<NN2CPPNGenome>(
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
                      << std::dynamic_pointer_cast<NN2CPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_nb_neurons()
                      << ","
                      << std::dynamic_pointer_cast<NN2CPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_nb_connections()
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
                      << std::dynamic_pointer_cast<NN2CPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_parents_ids()[0]
                      << ","
                      << std::dynamic_pointer_cast<NN2CPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_parents_ids()[1]
                      << std::endl;
    }
    logFileStream.close();
}

void GraphVizLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::stringstream filename;
        filename << "cppn_" << std::dynamic_pointer_cast<NN2CPPNGenome>(
                        ea->get_population()[i]->get_morph_genome()
                        )->id() << ".dot";
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        nn2_cppn_t cppn = std::dynamic_pointer_cast<NN2CPPNGenome>(
                             ea->get_population()[i]->get_morph_genome()
                             )->get_cppn();
        cppn.write_dot(logFileStream);
        logFileStream.close();
    }
}
