#include "physicalMorphoEvo.hpp"

using namespace are;

void PMEIndividual::createMorphology(){
    NEAT::Genome gen =
            std::dynamic_pointer_cast<CPPNGenome>(morphGenome)->get_neat_genome();
    morphology.reset(new sim::Morphology_CPPNMatrix(parameters));
    NEAT::NeuralNetwork nn;
    gen.BuildPhenotype(nn);
    std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->setNEATCPPN(nn);
    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(init_x,init_y,0.15);

    morphDesc = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getMorphDesc();
    testRes = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getRobotManRes();
    listOrganTypes = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getOrganTypes();
    listOrganPos = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getOrganPosList();
    listOrganOri = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getOrganOriList();
    skeletonListIndices = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getSkeletonListIndices();
    skeletonListVertices = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getSkeletonListVertices();
}


void PhysicalMorphoEvo::init(){

    std::string folder_to_load = settings::getParameter<settings::String>(parameters,"#folderToLoad").value;
    int generation = settings::getParameter<settings::Integer>(parameters,"#genToLoad").value;
    int indIdx = settings::getParameter<settings::Integer>(parameters,"#indToLoad").value;

    if(indIdx < 0){
        int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
        for(int i = 0; i < pop_size; i++){
            std::stringstream sstr2;
            sstr2 << folder_to_load << "/morphGenome_" << generation << "_" << i;
            morph_gen_files.push_back(sstr2.str());
            ids.push_back(std::make_pair(generation,i));
        }
    }else
    {
        std::stringstream sstr2;
        sstr2 << folder_to_load << "/morphGenome_" << generation << "_" << indIdx;
        morph_gen_files.push_back(sstr2.str());
        ids.push_back(std::make_pair(generation,indIdx));
    }


    CPPNGenome::Ptr morph_gen;
    EmptyGenome::Ptr ctrl_gen;
    //load morphology genome
    for(size_t i = 0; i < morph_gen_files.size(); i++){
        NEAT::Genome neat_gen(morph_gen_files[i].c_str());
        morph_gen.reset(new CPPNGenome(neat_gen));
        morph_gen->set_randNum(randomNum);
        morph_gen->set_parameters(parameters);

        ctrl_gen.reset(new EmptyGenome);

        Individual::Ptr ind(new PMEIndividual(morph_gen,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
    ctrl_gen.reset();
    morph_gen.reset();
}

void PhysicalMorphoEvo::init_next_pop(){

}

void PhysicalMorphoEvo::write_data_for_generate(){

    const auto& ind = population[currentIndIndex];
    const auto& id = ids[currentIndIndex];

    // Export blueprint

    std::stringstream sst_blueprint;
    sst_blueprint << "blueprint_" << id.first << "_" << id.second << ".csv";
    std::ofstream ofs_blueprint(Logging::log_folder + std::string("/waiting_to_be_built/")  + sst_blueprint.str());
    if(!ofs_blueprint)
    {
        std::cerr << "unable to open : " << Logging::log_folder + std::string("/waiting_to_be_built/")  + sst_blueprint.str() << std::endl;
        return;
    }
    std::vector<int> tempOrganTypes = std::dynamic_pointer_cast<PMEIndividual>(ind)->getListOrganTypes();
    std::vector<std::vector<float>> tempOrganPos = std::dynamic_pointer_cast<PMEIndividual>(ind)->getListOrganPos();
    std::vector<std::vector<float>> tempOrganOri = std::dynamic_pointer_cast<PMEIndividual>(ind)->getListOrganOri();
    for (int i = 0; i < tempOrganTypes.size(); i++) {
        ofs_blueprint << "0" << "," << tempOrganTypes.at(i) << ","
                      << tempOrganPos.at(i).at(0) << "," << tempOrganPos.at(i).at(1) << ","
                      << tempOrganPos.at(i).at(2) << ","
                      << tempOrganOri.at(i).at(0) << "," << tempOrganOri.at(i).at(1) << ","
                      << tempOrganOri.at(i).at(2) << ","
                      << std::endl;
    }

    // Export mesh file

    const auto **verticesMesh = new const simFloat *[2];
    const auto **indicesMesh = new const simInt *[2];
    auto *verticesSizesMesh = new simInt[2];
    auto *indicesSizesMesh = new simInt[2];
    verticesMesh[0] = std::dynamic_pointer_cast<PMEIndividual>(ind)->getSkeletonListVertices().data();
    verticesSizesMesh[0] = std::dynamic_pointer_cast<PMEIndividual>(ind)->getSkeletonListVertices().size();
    indicesMesh[0] = std::dynamic_pointer_cast<PMEIndividual>(ind)->getSkeletonListIndices().data();
    indicesSizesMesh[0] = std::dynamic_pointer_cast<PMEIndividual>(ind)->getSkeletonListIndices().size();

    std::stringstream filepath;
    filepath << Logging::log_folder << "/waiting_to_be_built/mesh_" << id.first << "_" << id.second << ".stl";

    //fileformat: the fileformat to export to:
    //  0: OBJ format, 3: TEXT STL format, 4: BINARY STL format, 5: COLLADA format, 6: TEXT PLY format, 7: BINARY PLY format
    simExportMesh(3, filepath.str().c_str(), 0, 1.0f, 1, verticesMesh, verticesSizesMesh, indicesMesh, indicesSizesMesh, nullptr, nullptr);

    delete[] verticesMesh;
    delete[] verticesSizesMesh;
    delete[] indicesMesh;
    delete[] indicesSizesMesh;
}
