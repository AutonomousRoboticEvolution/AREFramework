#include "EA_NEAT.h"
#include <algorithm>


EA_NEAT::EA_NEAT()
{
}


EA_NEAT::~EA_NEAT()
{
}

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
	params.OldAgePenalty = 0.1;
	params.MinSpecies = 2;
	params.MaxSpecies = 4;
	params.RouletteWheelSelection = false;
	params.RecurrentProb = 0.0;
	params.OverallMutationRate = 0.4;

	params.MutateWeightsProb = 0.0;

	params.WeightMutationMaxPower = 2.5;
	params.WeightReplacementMaxPower = 5.0;
	params.MutateWeightsSevereProb = 0.5;
	params.WeightMutationRate = 0.25;

	params.MaxWeight = 8;

	params.MutateAddNeuronProb = 0.003;
	params.MutateAddLinkProb = 0.05;
	params.MutateRemLinkProb = 0.0;

	params.MinActivationA = 4.9;
	params.MaxActivationA = 4.9;

	params.ActivationFunction_SignedSigmoid_Prob = 0.0;
	params.ActivationFunction_UnsignedSigmoid_Prob = 1.0;
	params.ActivationFunction_Tanh_Prob = 0.0;
	params.ActivationFunction_SignedStep_Prob = 0.0;

	params.CrossoverRate = 0.75;
	params.MultipointCrossoverRate = 0.4;
	params.SurvivalRate = 0.2;

	params.AllowClones = false;
	params.AllowLoops = false;
	params.DontUseBiasNeuron = true;

	params.MutateNeuronTraitsProb = 0.2;
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
	itp1.mut_replace_prob = 0.1;
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

	NEAT::Genome s(0, 1,
		1,
		1,
		false,
		NEAT::UNSIGNED_SIGMOID,
		NEAT::UNSIGNED_SIGMOID,
		1,
		params,
		2);

	population = shared_ptr<NEAT::Population>(new NEAT::Population(s, params, true, 1.0, randomNum->getSeed()));

	// Initialize the genome placeholder with all the information of the robot
	unique_ptr<GenomeFactory> gf = unique_ptr<GenomeFactory>(new GenomeFactory);
	currentGenome = gf->createGenome(settings->morphologyType, randomNum, settings);
	gf.reset();
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
	cout << "updating EA_NEAT" << endl;
}

void EA_NEAT::end()
{
	// get position of the phenotype

}

void EA_NEAT::selection() // This will be used instead of the Epoch used in NEAT (To keep EAs consistent)
{
	genomeBuffer.clear();
	// epoch used in NEAT. This should replace the previous existing population. 
	population->Epoch();
	// Note that this function accepts that all the genomes have been evaluated
	std::cout << "Generation:" << settings->generation << endl;
	// Add individuals to the population buffer. Should this be a deep copy?
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

}


bool EA_NEAT::compareByFitness(const shared_ptr<Genome> a, const shared_ptr<Genome> b)
{
	return a->fitness > b->fitness;
}

void EA_NEAT::createIndividual(int ind) {
	for (int i = 0; i < population->m_Species.size(); i++) {
		for (int j = 0; j < population->m_Species[i].m_Individuals.size(); j++) {
			// check if the genomes have been evaluated.
			currentNeatIndividual = &population->m_Species[i].m_Individuals[j]; // shallow copy
			if (currentNeatIndividual->IsEvaluated() == false) {
				// Not sure if I need to make the phenotype of the network
				// currentNet = unique_ptr<NEAT::NeuralNetwork>(new NEAT::NeuralNetwork());
				currentNeatIndividual->PrintAllTraits();
				break; // break from for loop to evaluate this genome
			}
		}
	}
}