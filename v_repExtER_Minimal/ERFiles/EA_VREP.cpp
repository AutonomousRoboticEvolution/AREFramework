#include "EA_VREP.h"

EA_VREP::EA_VREP()
{
}


EA_VREP::~EA_VREP()
{
}

void EA_VREP::init()
{
	gf = unique_ptr<GenomeFactoryVREP>(new GenomeFactoryVREP);
}

void EA_VREP::selection()
{
}

void EA_VREP::replacement()
{
}

void EA_VREP::mutation()
{
}

void EA_VREP::initNewGenome(shared_ptr<Settings> st, bool client)
{
	newGenome = gf->createGenome(0, randomNum, st);
	newGenome->fitness = 0;
	
}

void EA_VREP::loadIndividual(int individualNum, int sceneNum) {
	cout << "loading individual " << individualNum << ", sceneNum " << sceneNum << endl;
	//	newGenome->morph->loadBaseMorphology(indNum, sceneNum);
	newGenome = gf->createGenome(0, randomNum,settings);
	//newGenome = unique_ptr<DefaultGenomeVREP>(new DefaultGenomeVREP(randomNum, settings));
	//newGenome->init();
	newGenome->loadGenome(individualNum, sceneNum);
	cout << "loaded" << endl;
}

void EA_VREP::initializePopulation(shared_ptr<Settings> st, bool client)
{
	settings = st;
	if (client) {
		for (int i = 0; i < st->populationSize; i++)
		{
			populationGenomes.push_back(gf->createGenome(1, randomNum, st));
			populationGenomes[i]->fitness = 0;
			// for easy access of fitness values (used by client-server)
			popFitness.push_back(0);
		}
	}
	else {
		for (int i = 0; i < st->populationSize; i++)
		{
			populationGenomes.push_back(gf->createGenome(0, randomNum, st));
			populationGenomes[i]->fitness = 0;
			// for easy access of fitness values (used by client-server)
			popFitness.push_back(0);
		}
	}
}

void EA_VREP::loadBestIndividualGenome(int sceneNum)
{
	vector<int> individuals;
	vector<float> fitnessValues;

	ifstream file(settings->repository + "/SavedGenerations" + to_string(sceneNum) + ".csv");
	if (file.good()) {
		cout << "saved generations file found" << endl;
		string value;
		list<string> values;
		// read the settings file and store the comma seperated values
		while (file.good()) {
			getline(file, value, ','); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
			if (value.find('\n') != string::npos) {
				split_line(value, "\n", values);
			}
			else {
				values.push_back(value);
			}
		}
		file.close();

		list<string>::const_iterator it = values.begin();
		for (it = values.begin(); it != values.end(); it++) {
			string tmp = *it;
			if (tmp == "ind: ") {
				it++;
				tmp = *it;
				individuals.push_back(atoi(tmp.c_str()));
			}
			else if (tmp == "fitness: ") {
				it++;
				tmp = *it;
				fitnessValues.push_back(atof(tmp.c_str()));
			}
		}
	}
	int bestInd = 0;
	float bestFit = 0.0;
	cout << "individuals size = " << individuals.size() << endl;
	for (int i = 0; i < individuals.size(); i++) {
		if (fitnessValues[i] >= bestFit) {
			bestFit = fitnessValues[i];
			bestInd = individuals[i];
		}
	}

	cout << "loading individual " << bestInd << ", sceneNum " << sceneNum << endl;
	newGenome.reset();
	newGenome = gf->createGenome(0, randomNum, settings);
	//newGenome->init();	//	cout << "loading" << endl;
	newGenome->loadGenome(bestInd, sceneNum);
}