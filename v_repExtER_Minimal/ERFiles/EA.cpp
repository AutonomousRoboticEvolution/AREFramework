#include "EA.h"



EA::EA()
{
}


EA::~EA()
{
}

void EA::split_line(string& line, string delim, list<string>& values)
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

void EA::setSettings(shared_ptr<Settings> st, shared_ptr<RandNum> rn)
{
	settings = st;
	randomNum = rn;
}

void EA::init()
{
	gf = unique_ptr<GenomeFactory>(new GenomeFactory);
}


void EA::createIndividual(int indNum)
{
	populationGenomes[indNum]->create();
}

void EA::loadIndividual(int individualNum) {

}

void EA::savePopFitness(int generation, vector<float> popfit) {
	cout << "SAVING GENERATION" << endl << endl;
	ofstream savePopFile;
	string saveFileName;
	saveFileName = settings->repository + "/SavedGenerations" + to_string(settings->sceneNum) + ".csv";
	savePopFile.open(saveFileName.c_str(), ios::out | ios::ate | ios::app);
	savePopFile << "generation " << generation << ": ,";
	for (int i = 0; i < populationGenomes.size(); i++) {
		savePopFile /*<< " ind " << i << ": " */ << popfit[i] << ",";
	}
	float avgFitness = 0;
	for (int i = 0; i < populationGenomes.size(); i++) {
		avgFitness += popfit[i];
	}
	avgFitness = avgFitness / populationGenomes.size();
	savePopFile << "avg: ," << avgFitness << ",";
	int bestInd = 0;
	float bestFitness = 0;
	for (int i = 0; i < populationGenomes.size(); i++) {
		if (bestFitness < popfit[i]) {
			bestFitness = popfit[i];
			bestInd = i;
		}
	}
	savePopFile << "ind: ," << popIndNumbers[bestInd] << ",";
	savePopFile << "fitness: ," << bestFitness << ",";
	savePopFile << "individuals: ,";
	for (int i = 0; i < popIndNumbers.size(); i++) {
		savePopFile << popIndNumbers[i] << ",";
	}
	savePopFile << endl;
	savePopFile.close();
	cout << "GENERATION SAVED" << endl;
}


void EA::savePopFitness(int generation) {
	cout << "SAVING GENERATION" << endl << endl;
	ofstream savePopFile;
	string saveFileName;
	saveFileName = settings->repository + "/SavedGenerations" + to_string(settings->sceneNum) + ".csv";
	savePopFile.open(saveFileName.c_str(), ios::out | ios::ate | ios::app);
	savePopFile << "generation " << generation << ": ,";
	for (int i = 0; i < populationGenomes.size(); i++) {
		savePopFile /*<< " ind " << i << ": " */ << populationGenomes[i]->fitness << ",";
	}
	float avgFitness = 0;
	for (int i = 0; i < populationGenomes.size(); i++) {
		avgFitness += populationGenomes[i]->fitness;
	}
	avgFitness = avgFitness / populationGenomes.size();
	savePopFile << "avg: ," << avgFitness << ",";
	int bestInd = 0;
	float bestFitness = 0;
	for (int i = 0; i < populationGenomes.size(); i++) {
		if (bestFitness < populationGenomes[i]->fitness) {
			bestFitness = populationGenomes[i]->fitness;
			bestInd = i;
		}
	}
	savePopFile << "ind: ," << popIndNumbers[bestInd] << ",";
	savePopFile << "fitness: ," << bestFitness << ",";
	savePopFile << "individuals: ,";
	for (int i = 0; i < popIndNumbers.size(); i++) {
		savePopFile << popIndNumbers[i] << ",";
	}
	savePopFile << endl;
	savePopFile.close();
	cout << "GENERATION SAVED" << endl;
}

void EA::loadPopulationGenomes()
{
	popIndNumbers = settings->indNumbers;
	popFitness = settings->indFits;

	for (int i = 0; i < popIndNumbers.size(); i++) {
		cout << "loading individual " << popIndNumbers[i] << endl;
		//populationGenomes[i]->init_noMorph();
		// bug??
		populationGenomes[i]->loadMorphologyGenome(popIndNumbers[i], settings->sceneNum);
		//	populationFitness[i] = populationGenomes[i]->morph->getFitness();
		cout << "called fitness = " << popFitness[i] << endl;
	}
}

void EA::loadBestIndividualGenome()
{
	vector<int> individuals;
	vector<float> fitnessValues;

	ifstream file(settings->repository + "/SavedGenerations" + to_string(settings->sceneNum) + ".csv");
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

	cout << "loading individual " << bestInd << ", sceneNum " << settings->sceneNum << endl;
	//	newGenome->morph->loadBaseMorphology(indNum, sceneNum);
	newGenome.reset();
	//unique_ptr<GenomeFactory> gf = unique_ptr<GenomeFactory>(new GenomeFactory);
	newGenome = gf->createGenome(1, randomNum, settings); 
	//gf.reset();
	//unique_ptr<DefaultGenomeVREP>(new DefaultGenomeVREP(randomNum, settings));
	//newGenome->morphologyType = populationGenomes[0]->morphologyType;
	//newGenome->init_noMorph(); // init_noMorph is old
	newGenome->init();	//	cout << "loading" << endl;
	newGenome->loadMorphologyGenome(bestInd, settings->sceneNum);
	//	cout << "creating??" << endl;
	// newGenome->create();
}

