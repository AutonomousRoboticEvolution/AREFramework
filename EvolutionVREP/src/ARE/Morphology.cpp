#include "ARE/Morphology.h"
#include <iostream>

using namespace are;

Morphology::Morphology(const Settings& st){
    settings.reset(new Settings(st));
}

//std::vector<std::shared_ptr<ER_Module>> Morphology::getCreatedModules()
//{
//	std::vector<std::shared_ptr<ER_Module>> nullVec;
//	return nullVec;
//}


void Morphology::setPhenValue()
{
    phenValue = 1;
}




void Morphology::saveGenome(int indNum, float fitness) {
    std::cout << "saving Fixed Genome " << std::endl << "-------------------------------- " << std::endl;
    std::ofstream genomeFile;
    std::ostringstream genomeFileName;
    genomeFileName << settings->repository << "/morphologies" << settings->sceneNum << "/genome" << indNum << ".csv";
    //	genomeFileName << indNum << ".csv";
    genomeFile.open(genomeFileName.str());
    if (!genomeFile) {
        std::cerr << "Error opening file \"" << genomeFileName.str() << "\" to save genome." << std::endl;
    }
    if (settings->morphologyType == settings->CAT_MORPHOLOGY) {
        genomeFile << ",#GenomeType,CatGenome," << std::endl;
    }
    else {
        genomeFile << ",#GenomeType,unknown," << std::endl;
    }
    genomeFile << "#Individual:" << indNum << std::endl;
    genomeFile << "#Fitness:," << fitness << std::endl;
    genomeFile << "#ControlParams:," << std::endl;
    genomeFile << "	#ControlType,0," << std::endl;
    genomeFile << "#EndControlParams" << std::endl;
    genomeFile << "end of fixed Morphology" << std::endl;
    std::cout << "saved cat" << std::endl;
    genomeFile.close();
}
