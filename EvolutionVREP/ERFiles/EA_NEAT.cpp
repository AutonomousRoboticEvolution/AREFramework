#include "EA_NEAT.h"
#include <algorithm>

using namespace std;

EA_NEAT::EA_NEAT()
{}

EA_NEAT::~EA_NEAT()
{}

void EA_NEAT::split_line(string& line, string delim, list<string>& values)
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

void EA_NEAT::init()
{
	NEAT::Parameters params; 
	params.PopulationSize = settings->populationSize;
	params.DynamicCompatibility = true;
	params.WeightDiffCoeff = 0.0;
	params.CompatTreshold = 3.0;
	params.YoungAgeTreshold = 15;

	params.SpeciesMaxStagnation = 15;
	params.OldAgeTreshold = 35;
	params.OldAgePenalty = 0.2;
	params.MinSpecies = 2;
	params.MaxSpecies = 4;
	params.RouletteWheelSelection = false;
	params.RecurrentProb = 0.1;
	params.OverallMutationRate = 0.9;

	params.MutateWeightsProb = 0.4;

	params.WeightMutationMaxPower = 2.5;
	params.WeightReplacementMaxPower = 5.0;
	params.MutateWeightsSevereProb = 0.5;
	params.WeightMutationRate = 0.25;

	params.MaxWeight = 8;

	params.MutateAddNeuronProb = 0.3;
	params.MutateAddLinkProb = 0.25;
	params.MutateRemLinkProb = 0.01;

	params.MinActivationA = 4.9;
	params.MaxActivationA = 4.9;

	params.ActivationFunction_SignedSigmoid_Prob = 0.5;
	params.ActivationFunction_UnsignedSigmoid_Prob = 0.5;
	params.ActivationFunction_Tanh_Prob = 0.0;
	params.ActivationFunction_SignedStep_Prob = 0.0;

	params.CrossoverRate = 0.75;
	params.MultipointCrossoverRate = 0.4;
	params.SurvivalRate = 0.2;

	params.AllowClones = false;
	params.AllowLoops = false;
	params.DontUseBiasNeuron = true;

	params.MutateNeuronTraitsProb = 0.6;
	params.MutateLinkTraitsProb = 0.2;

	params.ArchiveEnforcement = true;

	//params.CustomConstraints = NEAT::constraints;

	NEAT::TraitParameters tp1;
	tp1.m_ImportanceCoeff = 1.0;
	tp1.m_MutationProb = 0.9;
	tp1.type = "int";
	tp1.dep_key = "y";
	tp1.dep_values.push_back(std::string("c"));
	NEAT::IntTraitParameters itp1;
	itp1.min = -5;
	itp1.max = 5;
	itp1.mut_power = 1;
	itp1.mut_replace_prob = 0.5;
	tp1.m_Details = itp1;

	NEAT::TraitParameters tp2;
	tp2.m_ImportanceCoeff = 0.2;
	tp2.m_MutationProb = 0.9;
	tp2.type = "float";
	NEAT::FloatTraitParameters itp2;
	itp2.min = -1;
	itp2.max = 1;
	itp2.mut_power = 0.2;
	itp2.mut_replace_prob = 0.1;
	tp2.m_Details = itp2;

	NEAT::TraitParameters tp3;
	tp3.m_ImportanceCoeff = 0.02;
	tp3.m_MutationProb = 0.9;
	tp3.type = "intset";
	NEAT::IntSetTraitParameters itp3;
	NEAT::intsetelement kkk;
	kkk.value = 4;
	itp3.set.push_back(kkk);
	kkk.value = 8;
	itp3.set.push_back(kkk);
	kkk.value = 16;
	itp3.set.push_back(kkk);
	kkk.value = 32;
	itp3.set.push_back(kkk);
	kkk.value = 64;
	itp3.set.push_back(kkk);
	itp3.probs.push_back(1);
	itp3.probs.push_back(1);
	itp3.probs.push_back(1);
	itp3.probs.push_back(1);
	itp3.probs.push_back(1);
	tp3.m_Details = itp3;

	NEAT::TraitParameters tps;
	tps.m_ImportanceCoeff = 0.02;
	tps.m_MutationProb = 0.9;
	tps.type = "str";
	NEAT::StringTraitParameters itps;
	itps.set.push_back("a");
	itps.set.push_back("b");
	itps.set.push_back("c");
	itps.set.push_back("d");
	itps.set.push_back("e");
	itps.probs.push_back(1);
	itps.probs.push_back(1);
	itps.probs.push_back(1);
	itps.probs.push_back(1);
	itps.probs.push_back(1);
	tps.m_Details = itps;

	/*TraitParameters tp3;
	tp3.m_ImportanceCoeff = 0.0;
	tp3.m_MutationProb = 0.9;
	tp3.type = "str";
	StringTraitParameters itp3;
	itp3.set.push_back("true");
	itp3.set.push_back("false");
	itp3.probs.push_back(1);
	itp3.probs.push_back(1);
	tp3.m_Details = itp3;*/

	params.GenomeTraits["v"] = tp1;
	params.GenomeTraits["x"] = tp2;
	params.GenomeTraits["y"] = tps;
	params.NeuronTraits["z"] = tp3;

	NEAT::Genome s(0, 8,
		8,
		8,
		false,
		NEAT::UNSIGNED_SIGMOID,
		NEAT::UNSIGNED_SIGMOID,
		1,
		params,
		2);

	if (settings->startingCondition == settings->COND_LOAD_BEST) {// load population if specified
		population = shared_ptr<NEAT::Population>(new NEAT::Population((settings->repository + "/testNEAT").c_str()));
	}
	else {
		population = shared_ptr<NEAT::Population>(new NEAT::Population(s, params, true, 1.0, randomNum->getSeed()));
		population->Epoch();
	}
	// Initialize the genome placeholder with all the information of the robot
	// unique_ptr<GenomeFactory> gf = unique_ptr<GenomeFactory>(new GenomeFactory);
	// currentGenome = gf->createGenome(settings->morphologyType, randomNum, settings);
	// gf.reset();
	std::cout << "Initialized NEAT" << std::endl;	
}

vector<float> EA_NEAT::Evaluate(NEAT::Genome genome)
{
	vector<float> fitness;
	// create the robot based on the neural network. 
	er_genome->init();

	//NEAT::NeuralNetwork &net = new NEAT::NeuralNetwork();
	//genome.BuildPhenotype(net);

	//delete &net;
	return fitness;
}



void EA_NEAT::update() {
	// cout << "updating EA_NEAT" << endl;
	neat_morph->update();
}

void EA_NEAT::end()
{
	// get position of the phenotype
}

shared_ptr<Morphology> EA_NEAT::getMorph()
{
	return neat_morph;
}

void EA_NEAT::selection() // This will be used instead of the Epoch used in NEAT (To keep EAs consistent)
{
	// cout << "selection operator is never called" << endl;
	genomeBuffer.clear();
	NEAT::Genome * bestGenome = population->GetBestGenomePointer();
	settings->bestIndividual = bestGenome->GetID();
	settings->indFits.clear();
	settings->indNumbers.clear();
	for (int i = 0; i < population->m_Species.size(); i++) {
		for (int j = 0; j < population->m_Species[i].m_Individuals.size(); j++) {
			settings->indFits.push_back(population->m_Species[i].m_Individuals[j].GetFitness());
			settings->indNumbers.push_back(population->m_Species[i].m_Individuals[j].GetID());
		}
	}
	// save the settings with the most recent individuals of the neat population
	settings->saveSettings();
	savePopFitness(population->GetGeneration(), settings->indFits, settings->indNumbers);

	// epoch used in NEAT. This should replace the previous existing population. 
	population->Epoch();
	// Note that this function accepts that all the genomes have been evaluated
	settings->generation += 1;
	std::cout << "Generation:" << settings->generation << endl;
	// Add individuals to the population buffer. Should this be a deep copy?
	// not used 
	for (int i = 0; i < population->m_Species.size(); i++) {
		for (int j = 0; j < population->m_Species[i].m_Individuals.size(); j++) {
			genomeBuffer.push_back(&population->m_Species[i].m_Individuals[j]);
			//genomeBuffer.push_back(pop->m_Species[i].m_Individuals);
		}
	}
}

void EA_NEAT::replacement()
{

}

void EA_NEAT::mutation() {

}

void EA_NEAT::initializePopulation()
{

}

void EA_NEAT::loadPopulationGenomes(int scenenum)
{
	cout << "Loading NEAT genomes" << endl;
	if (!population) {
		population->loadPopulation((settings->repository + neatSaveFile).c_str());
	}
	else {
		population->loadPopulation((settings->repository + neatSaveFile).c_str());
		// population = shared_ptr<NEAT::Population>(new NEAT::Population((settings->repository + neatSaveFile).c_str()));
	}
}


bool EA_NEAT::compareByFitness(const shared_ptr<Genome> a, const shared_ptr<Genome> b)
{
	return a->fitness > b->fitness;
}

void EA_NEAT::createIndividual(int ind) {
	cout << "Should create NEAT individual" << endl;
	if (ind < -1) { // load best
		unique_ptr<MorphologyFactoryVREP> mf = unique_ptr<MorphologyFactoryVREP>(new MorphologyFactoryVREP);
		neat_morph = mf->createMorphologyGenome(settings->morphologyType, randomNum, settings);
		mf.reset();
		currentNeatIndividual = population->GetBestGenomePointer(); // shallow copy
		// currentNet = &population->GetBestGenomePointer();
		// Not sure if I need to make the phenotype of the network
		currentNet.reset();
		currentNet = shared_ptr<NEAT::NeuralNetwork>(new NEAT::NeuralNetwork());
		// currentNet = make_shared<NEAT::NeuralNetwork>(new NEAT::NeuralNetwork(currentNeatIndividual));
		// currentNet = &currentNeatIndividual;
		currentNeatIndividual->BuildPhenotype(*currentNet);
		// currentNeatIndividual->BuildPhenotype(*currentNet);
		neat_morph->neat_net = currentNet; //  make_shared<NEAT::NeuralNetwork>(currentNeatIndividual);
		neat_morph->create();
		if (settings->verbose) {
			currentNeatIndividual->PrintAllTraits();
		}
		return;
	}
	else if (ind > -1) // load individual with the given ID
	{
		cout << "Creating ind " << ind << endl;
		unique_ptr<MorphologyFactoryVREP> mf = unique_ptr<MorphologyFactoryVREP>(new MorphologyFactoryVREP);
		neat_morph = mf->createMorphologyGenome(settings->morphologyType, randomNum, settings);
		mf.reset();
		for (int i = 0; i < population->m_Species.size(); i++) {
			for (int j = 0; j < population->m_Species[i].m_Individuals.size(); j++) {
				if (population->m_Species[i].m_Individuals[j].GetID() == ind) {
					// this is the individual that should be created
					currentNeatIndividual = &population->m_Species[i].m_Individuals[j]; // set the individual pointer
					cout << "Found the NEAT individual that needs to be loaded" << endl;
				}
			}
		}
		// set the NN 
		currentNet.reset();
		currentNet = shared_ptr<NEAT::NeuralNetwork>(new NEAT::NeuralNetwork());
		currentNeatIndividual->BuildPhenotype(*currentNet);
		neat_morph->neat_net = currentNet; //  make_shared<NEAT::NeuralNetwork>(currentNeatIndividual);
		neat_morph->create();
		if (settings->verbose) {
			currentNeatIndividual->PrintAllTraits();
		}
		return;
	}
	// Check if new generation of individuals should be created
	unique_ptr<MorphologyFactoryVREP> mf = unique_ptr<MorphologyFactoryVREP>(new MorphologyFactoryVREP);
	neat_morph = mf->createMorphologyGenome(settings->morphologyType, randomNum, settings);
	mf.reset();

	bool shouldEpoch = true;
	for (int i = 0; i < population->m_Species.size(); i++) {
		for (int j = 0; j < population->m_Species[i].m_Individuals.size(); j++) {
			if (population->m_Species[i].m_Individuals[j].IsEvaluated() == false) {
				shouldEpoch = false;
				break;
			}
		}
	}
	if (shouldEpoch == true && settings->instanceType != settings->INSTANCE_SERVER) {
		population->Save((settings->repository + "/testNEAT" + to_string(settings->generation)).c_str());
		selection();
		// population->Epoch();
	}
	bool foundIndividualToEvaluate = false;
	for (int i = 0; i < population->m_Species.size(); i++) {
		for (int j = 0; j < population->m_Species[i].m_Individuals.size(); j++) {
			// check if the genomes have been evaluated.
			currentNeatIndividual = &population->m_Species[i].m_Individuals[j]; // shallow copy
			if (currentNeatIndividual->IsEvaluated() == false) {
				// Not sure if I need to make the phenotype of the network
				currentNet.reset();
				currentNet = shared_ptr<NEAT::NeuralNetwork>(new NEAT::NeuralNetwork());
				currentNeatIndividual->BuildPhenotype(*currentNet);
				neat_morph->neat_net = currentNet;
				neat_morph->create();
				neat_morph->savePhenotype(currentNeatIndividual->GetID(), 0.0);
				if (settings->verbose) {
					currentNeatIndividual->PrintAllTraits();
				}
				foundIndividualToEvaluate = true; // not used
				return; // break from for loop to evaluate this genome
			}
		}
	}
}

void EA_NEAT::setFitness(int ind, float fit)
{
	currentNeatIndividual->SetFitness(fit);
	currentNeatIndividual->SetEvaluated();
}

void EA_NEAT::loadBestIndividualGenome(int sceneNum) {
	// assumes that the NEAT population has been loaded 
	// NEAT doesn't store fitness values in genome so after loading the genomes need there fitness values again
	int c = 0;
	for (int i = 0; i < population->m_Species.size(); i++) {
		for (int j = 0; j < population->m_Species[i].m_Individuals.size(); j++) {
			population->m_Species[i].m_Individuals[j].SetFitness(settings->indFits[c]);
			//population->m_Species[i].m_Individuals[j].SetID(settings->indNumbers[c]);
			c++;
		}
	}
	currentNeatIndividual = population->GetBestGenomePointer();
	cout << "loaded individual number " << currentNeatIndividual->GetID() << " that had a fitness of " << currentNeatIndividual->GetFitness() << endl;
	for (int i = 0; i < population->m_Species.size(); i++) {
		for (int j = 0; j < population->m_Species[i].m_Individuals.size(); j++) {
			cout << "fitness of individual " << population->m_Species[i].m_Individuals[j].GetID() << "," << j << " = " << population->m_Species[i].m_Individuals[j].GetFitness() << endl;
		}
	}
}