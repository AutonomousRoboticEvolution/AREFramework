#include "ARE/EA_SteadyState.h"
#include <algorithm>
#include <memory>



void EA_SteadyState::split_line(std::string &line, std::string delim, std::list<std::string> &values)
{
    size_t pos = 0;
    while ((pos = line.find(delim, (pos + 0))) != std::string::npos) {
        std::string p = line.substr(0, pos);
        values.push_back(p);
        line = line.substr(pos + 1);
    }
    while ((pos = line.find(delim, (pos + 1))) != std::string::npos) {
       std::string p = line.substr(0, pos);
        values.push_back(p);
        line = line.substr(pos + 1);
    }

    if (!line.empty()) {
        values.push_back(line);
    }
}

void EA_SteadyState::init()
{
    initializePopulation();
}

void EA_SteadyState::selection()
{
    createNewGenRandomSelect();
}

void EA_SteadyState::replacement()
{
    if (not populationGenomes.empty()) {
        // number of attempts means how many times the new individuals should be checked against the existing population
        //replaceNewPopRandom(2); // int is amount trials comparing offspring to existing population
        replaceNewRank();
    } else { // Should only happen in generation 0
        for (auto & nextGenGenome : nextGenGenomes) {
            populationGenomes.push_back(nextGenGenome->clone());
        }
    }
}

void EA_SteadyState::mutation()
{
    for (auto & nextGenGenome : nextGenGenomes) {
        nextGenGenome->mutate();
    }
}

void EA_SteadyState::initializePopulation()
{
    if (settings->verbose) {
        std::cout << "Creating initial population" << std::endl;
    }
    for (int i = 0; i < settings->populationSize; i++) {
        nextGenGenomes.push_back(createGenome(1, randomNum, settings));
        nextGenGenomes[i]->fitness = 0;
        nextGenGenomes[i]->individualNumber = i;
    }
}

void EA_SteadyState::loadPopulationGenomes(int scenenum)
{
    const std::vector<int> &popIndNumbers = settings->indNumbers;
    std::cout << "Loading population" << std::endl;
    for (unsigned long i = 0; i < popIndNumbers.size(); i++) {
        std::cout << "loading individual " << popIndNumbers[i] << std::endl;
        populationGenomes[i]->loadGenome(popIndNumbers[i], scenenum);
        populationGenomes[i]->fitness = settings->indFits[i]; // indFits has to be saved now.
    }
}

void EA_SteadyState::saveGenome(const std::shared_ptr<Genome> &g)
{
    g->individualNumber = settings->indCounter;
    if (settings->savePhenotype) {
        //g->savePhenotype(settings->indCounter, settings->sceneNum);
    }
    g->morph->saveGenome(settings->indCounter, g->fitness);
}

void EA_SteadyState::setFitness(int individual, float fitness)
{
    nextGenGenomes[individual]->fitness = fitness;
    //	nextGenFitness[individual] = fitness;
}

void EA_SteadyState::createNewGenRandomSelect()
{
    nextGenGenomes.clear();
    std::vector<int> additionalInds;
    // start
    if (settings->loadFromQueue) {
        // check if file exists
        std::ifstream file(settings->repository + "/queue" + std::to_string(settings->sceneNum) + ".csv");
        if (file.good()) {
            std::cout << "found queue, loading individuals" << std::endl;
            // load csv
            std::string value;
            std::list<std::string> values;
            // read the settings file and store the comma seperated values
            while (file.good()) {
                getline(file, value,
                        ','); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
                if (value.find('\n') != std::string::npos) {
                    split_line(value, "\n", values);
                } else {
                    values.push_back(value);
                }
            }
            file.close();
            additionalInds.reserve(values.size());
            for (const std::string &text: values) {
                additionalInds.push_back(atoi(text.c_str()));
            }

        }

    }
    std::vector<std::shared_ptr<Genome>> populationGenomesBuffer;

    for (int i = 0; i < populationGenomes.size() - additionalInds.size(); i++) {
        int parent = randomNum->randInt(populationGenomes.size(), 0);
        if (settings->verbose) {
            std::cout << "Selecting parent " << parent << " with fitness " << populationGenomes[i]->fitness
                      << " individual number is " << populationGenomes[i]->individualNumber << std::endl;
            std::cout << "^ will get ind number " << i + settings->indCounter << std::endl;
        }
        //nextGenFitness.push_back(-100.0);
        populationGenomesBuffer.push_back(populationGenomes[parent]->clone());
        if (settings->verbose) {
            std::cout << "About to deep copy genome" << std::endl;

        }

        if (settings->verbose) {
            std::cout << "Done with deep copy genome" << std::endl;
        }
        populationGenomesBuffer[i]->individualNumber = i + settings->indCounter;
        populationGenomesBuffer[i]->fitness = 0; // Ensure the fitness is set to zero.
        populationGenomesBuffer[i]->isEvaluated = false; // This should also be set, just to be sure.
    }
    if (settings->verbose) {
        std::cout << "Mutating next gen genomes of size: " << nextGenGenomes.size() << std::endl;
    }
    // load queue individuals
    for (unsigned long i = 0; i < additionalInds.size(); i++) {
        populationGenomesBuffer.push_back(std::make_unique<DefaultGenome>());
        unsigned long indNum = populationGenomesBuffer.size() - 1;
        populationGenomesBuffer[indNum]->loadGenome(additionalInds[i], settings->sceneNum);
        populationGenomesBuffer[indNum]->individualNumber = i + settings->indCounter;
        populationGenomesBuffer[indNum]->fitness = 0; // Ensure the fitness is set to zero.
        populationGenomesBuffer[indNum]->isEvaluated = false; // This should also be set, just to be sure.
    }


    // saving genomes

    nextGenGenomes.clear();
    for (auto & genome : populationGenomesBuffer) {
        nextGenGenomes.push_back(genome->clone());
    }
    mutation();
    for (int i = 0; i < nextGenGenomes.size(); i++) {
        nextGenGenomes[i]->saveGenome(populationGenomesBuffer[i]->individualNumber);

        // Used to debug
        // populationGenomes[i]->saveGenome(-populationGenomes[i]->individualNumber);
    }
    populationGenomesBuffer.clear();
}

void EA_SteadyState::replaceNewPopRandom(int numAttempts)
{
    std::cout << "REPLACINGPOP::::::::::::::" << std::endl;
    for (int p = 0; p < populationGenomes.size(); p++) {
        for (int n = 0; n < numAttempts; n++) {
            int currentInd = randomNum->randInt(populationGenomes.size(), 0);
            // Within this loop a new individual is evaluated. When the
            // number of attempts is reached, and the new individual did not
            // replace an individual in the existing population, it's genome is deleted.
            if (nextGenGenomes[p]->fitness >= populationGenomes[currentInd]->fitness) {
                std::cout << "replacement: " << nextGenGenomes[p]->individualNumber << " replaces "
                     << populationGenomes[currentInd]->individualNumber << std::endl;
                std::cout << "replacement: " << nextGenGenomes[p]->fitness << " replaces "
                     << populationGenomes[currentInd]->fitness << std::endl;
                populationGenomes[currentInd].reset();
                populationGenomes[currentInd] = nextGenGenomes[p]->clone(); // new DefaultGenome();
                break;
            } else if (n == (numAttempts - 1)) {
                // delete genome file
                std::stringstream ss;
                ss << settings->repository + "/morphologies" << settings->sceneNum << "/genome"
                   << nextGenGenomes[p]->individualNumber << ".csv";
                std::string genomeFileName = ss.str();
                std::stringstream ssp;
                ssp << settings->repository + "/morphologies" << settings->sceneNum << "/phenotype"
                    << nextGenGenomes[p]->individualNumber << ".csv";
                std::string phenotypeFileName = ssp.str();
                //	genomeFileName << indNum << ".csv";
                std::cout << "Removing " << nextGenGenomes[p]->individualNumber << std::endl;
                remove(genomeFileName.c_str());
                remove(phenotypeFileName.c_str());
            }
        }
    }
    std::cout << "REPLACED POP" << std::endl;
}

bool compareByFitness(const std::shared_ptr<Genome> a, const std::shared_ptr<Genome> b)
{
    return a->fitness > b->fitness;
}

void EA_SteadyState::replaceNewRank()
{
    std::vector<std::shared_ptr<Genome>> populationGenomesBuffer;
    // create one big population. Just store the pointers in the buffer.
    for (int i = 0; i < populationGenomes.size(); i++) {
        populationGenomesBuffer.push_back(populationGenomes[i]);
    }
    for (int i = 0; i < nextGenGenomes.size(); i++) {
        populationGenomesBuffer.push_back(nextGenGenomes[i]);
    }

    // There were some individuals that got an odd fitness value, this should not happen anymore, but just in case, added a cerr.
    for (int i = 0; i < populationGenomes.size(); i++) {
        if (populationGenomesBuffer[i]->fitness > 100) {
            std::cerr << "ERROR: Note that the fitness wasn't set correctly. Giving individual "
                      << populationGenomes[i]->individualNumber << " a fitness of 0" << std::endl;
            populationGenomesBuffer[i]->fitness = 0;
        }
    }

    // sort population on fitness
    std::sort(populationGenomesBuffer.begin(), populationGenomesBuffer.end(), compareByFitness);

    // remove all individuals on the bottom of the list.
    while (populationGenomesBuffer.size() > settings->populationSize) {
        populationGenomesBuffer.pop_back();
    }

    // The buffer can now replace the existing populationGenomes
    populationGenomes.clear();

    for (int i = 0; i < populationGenomesBuffer.size(); i++) {
        populationGenomes.push_back(populationGenomesBuffer[i]->clone());
    }
    //populationGenomes = populationGenomesBuffer;
    // ^ This swap should kill all objects no referenced to anymore. Without smart pointers this looks dangerous as hell.
    //populationGenomesBuffer.clear();
    std::cout << "populationGenomes.size() = " << populationGenomes.size() << std::endl;
    std::cout << "nextGenGenomes.size() = " << nextGenGenomes.size() << std::endl;
    std::cout << "REPLACED POP RANKED" << std::endl;
}
