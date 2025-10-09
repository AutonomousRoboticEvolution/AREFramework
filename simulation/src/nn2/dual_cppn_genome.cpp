#include "simulatedER/nn2/dual_cppn_genome.hpp"

using namespace are;
using namespace dual_cppn;
using namespace sim::organ;

// std::mt19937 nn2::rgen_t::gen;


void dual_cppn_decoder::decode(skel_cppn_t &skel_cppn,
                               org_cppn_t &org_cppn,
                              skeleton::type& skeleton,
                               int nbr_organs,
                              organ_list_t &organ_list,
                              int &number_voxels,
                              bool growing)
{
    if(growing)
        generate_skeleton_growing(skel_cppn,skeleton);
    else
        generate_skeleton(skel_cppn,skeleton);
    skeleton::create_base(skeleton);
    skeleton::empty_space_for_head(skeleton);
    skeleton::remove_skeleton_regions(skeleton);
    skeleton::remove_hoverhangs(skeleton);
    skeleton::count_number_voxels(skeleton,number_voxels);
    skeleton::coord_t surface_coords;
    skeleton::find_skeleton_surface(skeleton,surface_coords);
    pcl::PointCloud<pcl::PointNormal>::Ptr sites_locations(new pcl::PointCloud<pcl::PointNormal>());
    generate_organs_sites(skeleton,sites_locations);
    generate_organ_list(org_cppn,sites_locations,nbr_organs,organ_list);
}


void dual_cppn_decoder::generate_skeleton(skel_cppn_t &cppn,skeleton::type &skeleton){
    PolyVox::Region region = skeleton.getEnclosingRegion();
    skeleton::type tmp_skeleton(region);
    std::vector<double> input{0,0,0}; // Vector used as input of the Neural Network (NN).
    std::vector<double> outputs;
    uint8_t voxel;
    // Generate voxel matrix
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                input[0] = static_cast<double>(x);
                input[1] = static_cast<double>(y);
                input[2] = static_cast<double>(z);

                // Activate NN
                cppn.step(input);
                outputs = cppn.outf();
                for(auto& o: outputs)
                    if(std::isnan(o))
                        o = 0;

                // Take output from NN and store it.
                voxel = morph_const::empty_voxel;
                if(outputs[0] > 0.00001) { // Sometimes values are very close to zero and these causes problems.
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

void dual_cppn_decoder::generate_skeleton_growing(skel_cppn_t &cppn,skeleton::type &skeleton){

    std::function<void(int32_t,int32_t,int32_t,skeleton::type&,skel_cppn_t&)> assign_skeleton_voxel =
        [&](int32_t x,int32_t y,int32_t z,skeleton::type& skeleton,skel_cppn_t &cppn){
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
            std::vector<double> input{ix, iy, iz}; // Vector used as input of the Neural Network (NN)
            cppn.step(input);
            uint8_t voxel;
            if(cppn.outf()[0] > 0.000001){ //test if value is positive
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

void dual_cppn_decoder::generate_organs_sites(skeleton::type &skeleton, pcl::PointCloud<pcl::PointNormal>::Ptr &sites_locations){
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

void dual_cppn_decoder::generate_organ_list(org_cppn_t &cppn, const pcl::PointCloud<pcl::PointNormal>::Ptr & site_locations, int nbr_organs, organ_list_t &organ_list){
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
}

int dual_cppn_decoder::cppn_to_organ_type(org_cppn_t &cppn,const std::vector<double> &input){
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

void GenomeLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::stringstream filename;
        filename << "morph_genome_" << ea->get_population()[i]->get_morph_genome()->id();
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        logFileStream << std::dynamic_pointer_cast<DualCPPNGenome>(
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
                      << std::dynamic_pointer_cast<DualCPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_nb_neurons()[0]
                      << ","
                      << std::dynamic_pointer_cast<DualCPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_nb_connections()[0]
                      << std::dynamic_pointer_cast<DualCPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_nb_neurons()[1]
                      << ","
                      << std::dynamic_pointer_cast<DualCPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_nb_connections()[1]
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
                      << std::dynamic_pointer_cast<DualCPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_parents_ids()[0]
                      << ","
                      << std::dynamic_pointer_cast<DualCPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_parents_ids()[1]
                      << std::endl;
    }
    logFileStream.close();
}

void GraphVizLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::pair<skel_cppn_t,org_cppn_t> cppns = std::dynamic_pointer_cast<DualCPPNGenome>(
                              ea->get_population()[i]->get_morph_genome()
                              )->get_cppns();
        std::stringstream filename1;
        filename1 << "skel_cppn_" << std::dynamic_pointer_cast<DualCPPNGenome>(
                                        ea->get_population()[i]->get_morph_genome()
                                        )->id() << ".dot";
        if(!openOLogFile(logFileStream, filename1.str()))
            return;
        cppns.first.write_dot(logFileStream);
        logFileStream.close();
        std::stringstream filename2;
        filename1 << "org_cppn_" << std::dynamic_pointer_cast<DualCPPNGenome>(
                                         ea->get_population()[i]->get_morph_genome()
                                         )->id() << ".dot";
        if(!openOLogFile(logFileStream, filename1.str()))
            return;
        cppns.second.write_dot(logFileStream);
        logFileStream.close();
    }
}
