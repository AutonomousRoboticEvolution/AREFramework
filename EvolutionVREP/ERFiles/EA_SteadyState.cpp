#include "EA_SteadyState.h"
#include <algorithm>
#include <memory>

using namespace std;


EA_SteadyState::EA_SteadyState()
{}

EA_SteadyState::~EA_SteadyState()
{}

void EA_SteadyState::split_line(string &line, string delim, list<string> &values)
{
    size_t pos = 0;
    while ((pos = line.find(delim, (pos + 0))) != string::npos) {
        string p = line.substr(0, pos);
        values.push_back(p);
        line = line.substr(pos + 1);
    }
    while ((pos = line.find(delim, (pos + 1))) != string::npos) {
        string p = line.substr(0, pos);
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
    std::unique_ptr<GenomeFactory> gf = std::make_unique<GenomeFactory>();
    for (int i = 0; i < settings->populationSize; i++) {
        nextGenGenomes.push_back(gf->createGenome(1, randomNum, settings));
        nextGenGenomes[i]->fitness = 0;
        nextGenGenomes[i]->individualNumber = i;
    }
    gf.reset();
}

void EA_SteadyState::loadPopulationGenomes(int scenenum)
{
    const vector<int> &popIndNumbers = settings->indNumbers;
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
    vector<int> additionalInds;
    // start
    if (settings->loadFromQueue) {
        // check if file exists
        ifstream file(settings->repository + "/queue" + to_string(settings->sceneNum) + ".csv");
        if (file.good()) {
            std::cout << "found queue, loading individuals" << endl;
            // load csv
            string value;
            list<string> values;
            // read the settings file and store the comma seperated values
            while (file.good()) {
                getline(file, value,
                        ','); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
                if (value.find('\n') != string::npos) {
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
    vector<shared_ptr<Genome>> populationGenomesBuffer;

    shared_ptr<MorphologyFactory> mfact(new MorphologyFactory);
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
            std::cout << "About to deep copy genome" << endl;

        }

        if (settings->verbose) {
            std::cout << "Done with deep copy genome" << endl;
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
    mfact.reset();
}

void EA_SteadyState::replaceNewPopRandom(int numAttempts)
{
    cout << "REPLACINGPOP::::::::::::::" << endl;
    for (int p = 0; p < populationGenomes.size(); p++) {
        for (int n = 0; n < numAttempts; n++) {
            int currentInd = randomNum->randInt(populationGenomes.size(), 0);
            // Within this loop a new individual is evaluated. When the
            // number of attempts is reached, and the new individual did not
            // replace an individual in the existing population, it's genome is deleted.
            if (nextGenGenomes[p]->fitness >= populationGenomes[currentInd]->fitness) {
                cout << "replacement: " << nextGenGenomes[p]->individualNumber << " replaces "
                     << populationGenomes[currentInd]->individualNumber << endl;
                cout << "replacement: " << nextGenGenomes[p]->fitness << " replaces "
                     << populationGenomes[currentInd]->fitness << endl;
                populationGenomes[currentInd].reset();
                populationGenomes[currentInd] = nextGenGenomes[p]->clone(); // new DefaultGenome();
                break;
            } else if (n == (numAttempts - 1)) {
                // delete genome file
                stringstream ss;
                ss << settings->repository + "/morphologies" << settings->sceneNum << "/genome"
                   << nextGenGenomes[p]->individualNumber << ".csv";
                string genomeFileName = ss.str();
                stringstream ssp;
                ssp << settings->repository + "/morphologies" << settings->sceneNum << "/phenotype"
                    << nextGenGenomes[p]->individualNumber << ".csv";
                string phenotypeFileName = ssp.str();
                //	genomeFileName << indNum << ".csv";
                cout << "Removing " << nextGenGenomes[p]->individualNumber << endl;
                remove(genomeFileName.c_str());
                remove(phenotypeFileName.c_str());
            }
        }
    }
    cout << "REPLACED POP" << endl;
}

bool compareByFitness(const shared_ptr<Genome> a, const shared_ptr<Genome> b)
{
    return a->fitness > b->fitness;
}

void EA_SteadyState::replaceNewRank()
{
    vector<shared_ptr<Genome>> populationGenomesBuffer;
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
    cout << "populationGenomes.size() = " << populationGenomes.size() << endl;
    cout << "nextGenGenomes.size() = " << nextGenGenomes.size() << endl;
    cout << "REPLACED POP RANKED" << endl;
}
