#include "Tissue_GMX_VREP.h"



Tissue_GMX_VREP::Tissue_GMX_VREP()
{
}


Tissue_GMX_VREP::~Tissue_GMX_VREP()
{
}

shared_ptr<Morphology> Tissue_GMX_VREP::clone() const {
	// shared_ptr<Morphology>()
	shared_ptr<Tissue_GMX_VREP> tissue_gmx = make_unique<Tissue_GMX_VREP>(*this);
	tissue_gmx->organs = this->organs;
	tissue_gmx->gaussians = this->gaussians;
	//tissue_directbars->control = control->clone();
	return tissue_gmx;
}

vector<int> Tissue_GMX_VREP::getObjectHandles(int parentHandle)
{
	return vector<int>();
}

vector<int> Tissue_GMX_VREP::getJointHandles(int parentHandle)
{
	simAddObjectToSelection(sim_handle_tree, parentHandle);
	int selectionSize = simGetObjectSelectionSize();
	int tempObjectHandles[1024]; // temporarily stores the object Handles as they cannot be inserted directly in a vector for some reason
	vector<int> jointHandles;
	//s_objectAmount = selectionSize;
	simGetObjectSelection(tempObjectHandles);
	int objectCounter = 0;
	for (size_t i = 0; i < selectionSize; i++)
	{
		if (simGetObjectType(tempObjectHandles[i]) == sim_object_joint_type) {
			jointHandles.push_back(tempObjectHandles[i]);
		}
	}
	simRemoveObjectFromSelection(sim_handle_all, -1);
	//cout << "jointHandles size = " << jointHandles.size() << endl;
	for (int i = 0; i < jointHandles.size(); i++) {
		outputHandles.push_back(jointHandles[i]);
		simSetObjectInt32Parameter(jointHandles[i], 2000, 1);
		simSetObjectInt32Parameter(jointHandles[i], 2001, 1);
		simSetObjectFloatParameter(jointHandles[i], 2002, 0.5);
		simSetObjectFloatParameter(jointHandles[i], 2003, 0.1);
		simSetObjectFloatParameter(jointHandles[i], 2004, 0.0);
		//	sim_jointintparam_ctrl_enabled(2001) : int32 parameter : dynamic motor control loop enable state(0 or != 0)
		//	simSetJointTargetVelocity(joint, 0.2);
		//	simSetJointForce(joint, 100);
		simSetJointTargetPosition(jointHandles[i], 1.0);
		simSetJointForce(jointHandles[i], 1.0);
		simSetJointTargetVelocity(jointHandles[i], 0.05);
	}
	return jointHandles;
}

void Tissue_GMX_VREP::update() {
	if (control) {
		vector<float> input; // get sensor parameters, not set in this case
		vector<float> output = control->update(input);
		//		cout << output[0] << endl;
		for (int i = 0; i < output.size(); i++) {
			simSetJointTargetVelocity(outputHandles[i], output[i] * 1000);// output[i]);
			//simSetJointTargetPosition(outputHandles[i], output[i]);
		}
	}
}

int Tissue_GMX_VREP::getMainHandle()
{
	return mainHandle;
}

struct sRobotMorphology
{
	int handle;
	int sensorHandles[2];
	std::vector<int> cubeHandles;
};


int Tissue_GMX_VREP::createRobot() {
    std::cout << "State: CREATE ROBOT!" << std::endl;
    int iNumberOfSamples = 333;

    std::vector< std::vector<float> > mean(3);
    std::vector< std::vector<float> > sigma(3);

    // Get this information from genome
    for(int i=0; i < mean.size(); i++){
		mean.at(i) = {gaussians[i].medians[0], gaussians[i].medians[1], gaussians[i].medians[2]};
		sigma.at(i) = {gaussians[i].sigmas[0], gaussians[i].sigmas[1], gaussians[i].sigmas[2]};
    }

    // Generate samples for each distribution
    Eigen::MatrixXd samplesA = getSamples(iNumberOfSamples, mean.at(0), sigma.at(0));
    Eigen::MatrixXd samplesB = getSamples(iNumberOfSamples, mean.at(1), sigma.at(1));
    Eigen::MatrixXd samplesC = getSamples(iNumberOfSamples, mean.at(2), sigma.at(2));

    // Collect the samples from all the distributions into a single matrix
    Eigen::MatrixXd allSamples(samplesA.rows(), samplesA.cols()+samplesB.cols()+samplesC.cols());
    allSamples << samplesA, samplesB, samplesC;
    // std::cout << "DEBUGG Samples: " << allSamples << std::endl;

    // Get list of coordinates of each voxel
    Eigen::MatrixXd listOfCoordinates = createVoxelsFromPoints(allSamples);

    // Create robot
    // TODO: Check if this necessary with WL
    int nextRobotMorphologyHandle = 0;
    int handle = -1;
    sRobotMorphology RobotMorphology;
    handle = nextRobotMorphologyHandle++;
    RobotMorphology.handle = handle;

    // Importing motor organs
    int brainOrgan = simLoadModel("models/brainOrgan.ttm");
    int motorOrgan[organsNumber];
    // Create force sensors to place organs
    int pamsArg1[] = {0,0,0,0,0};
    float pamsArg2[] = {0,0,0,0,0};
    int forceSensor[organsNumber];
    int forceSensor3 = simCreateForceSensor(0,pamsArg1,pamsArg2,NULL);

    // Set organs position and orientation
    for(int i = 0; i < organsNumber; i++){
        motorOrgan[i] = simLoadModel("models/motorOrgan2.ttm");
        forceSensor[i] = simCreateForceSensor(0,pamsArg1,pamsArg2,NULL);
        float tempPos[3] = {organs[i].coordinates[0] / 100, organs[i].coordinates[1] / 100, organs[i].coordinates[2] / 100};
        simSetObjectPosition(motorOrgan[i], -1, tempPos);
        simSetObjectPosition(forceSensor[i], -1, tempPos);
        float tempOri[3] = {organs[i].orientations[0] / 100, organs[i].orientations[1] / 100, organs[i].orientations[2] / 100};
        simSetObjectOrientation(motorOrgan[i], -1, tempOri);
        simSetObjectOrientation(forceSensor[i], -1, tempOri);
    }
    float organ3_pos[] = {organs[2].coordinates[0] / 100, organs[2].coordinates[1] / 100, organs[2].coordinates[2] / 100};
    float organ3_ori[] = {0, 1.57080, 0};
    simSetObjectPosition(brainOrgan, -1, organ3_pos);
    simSetObjectPosition(forceSensor3, -1, organ3_pos);
    simSetObjectOrientation(brainOrgan, -1, organ3_ori);
    simSetObjectOrientation(forceSensor3, -1, organ3_ori);

    // TODO: EB to Create dummies

    // Open tissue file
    // TODO: Move file generation to a different place
    ofstream tissueFile;
    ostringstream tissueFileName;
    tissueFileName << settings->repository + "/stl0"  << "/stl" << ".csv";
    tissueFile.open(tissueFileName.str());
    // First coordinates reserved for organs.
    tissueFile << organs[0].coordinates[0] << ',' << organs[0].coordinates[1] << ',' << organs[0].coordinates[2] << std::endl;
    tissueFile << organs[1].coordinates[0] << ',' << organs[1].coordinates[1] << ',' << organs[1].coordinates[2] << std::endl;
    tissueFile << organs[2].coordinates[0] << ',' << organs[2].coordinates[1] << ',' << organs[2].coordinates[2] << std::endl;


    // Create voxels
    std::vector <int> cubeHandles;
    for (int i = 0; i < listOfCoordinates.rows(); i++) {


        // 48 Static object
        // 00 Dynamic not respondable
        // 08 Dynamic respondable
        //std::cout << "Voxel: " << listOfCoordinates(i,0)/100 << " " << listOfCoordinates(i,1)/100 << " " << listOfCoordinates(i,2)/100 << std::endl;
        // TODO EB to write method for this keepVoxel(int organType, int list of coordinates);
        // Motor 1
        if(listOfCoordinates(i,0)/100 > organs[0].coordinates[0]/100 - (0.0451/2) // Original 0.0351
           && listOfCoordinates(i,0)/100 < organs[0].coordinates[0]/100 + (0.0451/2)
           && listOfCoordinates(i,1)/100 > organs[0].coordinates[1]/100 - (0.04574/2)  // Original 0.03574
           && listOfCoordinates(i,1)/100 < organs[0].coordinates[1]/100 + (0.04574/2)
           && listOfCoordinates(i,2)/100 > organs[0].coordinates[2]/100 - (0.0433/2) // Original 0.0283
           && listOfCoordinates(i,2)/100 < organs[0].coordinates[2]/100 + 0.1 + (0.0433/2)){
            continue;
        }

        if(listOfCoordinates(i,0)/100 > organs[1].coordinates[0]/100 - (0.0451/2)
           && listOfCoordinates(i,0)/100 < organs[1].coordinates[0]/100 + (0.0451/2)
           && listOfCoordinates(i,1)/100 > organs[1].coordinates[1]/100 - (0.04574/2)
           && listOfCoordinates(i,1)/100 < organs[1].coordinates[1]/100 + (0.04574/2)
           && listOfCoordinates(i,2)/100 > organs[1].coordinates[2]/100 - (0.0433/2)
           && listOfCoordinates(i,2)/100 < organs[1].coordinates[2]/100 + 0.1 + (0.0433/2)){
            continue;
        }

        // Wheel 1
        if(listOfCoordinates(i,0)/100 > organs[0].coordinates[0]/100 - (0.07/2) // Original 0.05
           && listOfCoordinates(i,0)/100 < organs[0].coordinates[0]/100 + (0.07/2)
           && listOfCoordinates(i,1)/100 > organs[0].coordinates[1]/100 + 0.03 - (0.03/2)  // Original 0.05
           && listOfCoordinates(i,1)/100 < organs[0].coordinates[1]/100 + 0.03 + (0.03/2)
           && listOfCoordinates(i,2)/100 > organs[0].coordinates[2]/100 - (0.07/2) // Original 0.01
           && listOfCoordinates(i,2)/100 < organs[0].coordinates[2]/100 + 0.1 + (0.07/2)){
            continue;
        }
        // Wheel 2
        if(listOfCoordinates(i,0)/100 > organs[1].coordinates[0]/100 - (0.07/2) // Original 0.05
           && listOfCoordinates(i,0)/100 < organs[1].coordinates[0]/100 + (0.07/2)
           && listOfCoordinates(i,1)/100 > organs[1].coordinates[1]/100 + 0.03 - (0.03/2)  // Original 0.05
           && listOfCoordinates(i,1)/100 < organs[1].coordinates[1]/100 + 0.03 + (0.03/2)
           && listOfCoordinates(i,2)/100 > organs[1].coordinates[2]/100 - (0.07/2) // Original 0.01
           && listOfCoordinates(i,2)/100 < organs[1].coordinates[2]/100 + 0.1 + (0.07/2)){
            continue;
        }
        // Brain
        if(listOfCoordinates(i,0)/100 > organs[2].coordinates[0]/100 - (0.044/2)  // Original 0.024
           && listOfCoordinates(i,0)/100 < organs[2].coordinates[0]/100 + (0.044/2)
           && listOfCoordinates(i,1)/100 > organs[2].coordinates[1]/100 - (0.092/2) // Original 0.072
           && listOfCoordinates(i,1)/100 < organs[2].coordinates[1]/100 + (0.092/2)
           && listOfCoordinates(i,2)/100 > organs[2].coordinates[2]/100 - (0.056/2) // Original 0.036
           && listOfCoordinates(i,2)/100 < organs[2].coordinates[2]/100 + 0.1 + (0.056/2)){
            continue;
        }
        // Create voxel

        int temp_voxel_handle;
        //float voxel_size[] = {0.005, 0.005, 0.005};
        float voxel_size[] = {0.01, 0.01, 0.01};
        temp_voxel_handle = simCreatePureShape(0, 8, voxel_size, 1, NULL);
        RobotMorphology.cubeHandles.push_back(temp_voxel_handle);
        float voxel_pos[] = {listOfCoordinates(i, 0) / 100, listOfCoordinates(i, 1) / 100,
                             listOfCoordinates(i, 2) / 100};
        simSetObjectPosition(temp_voxel_handle, -1, voxel_pos);
        float voxel_color[] = {0, 1, 0};
        simSetShapeColor(temp_voxel_handle, NULL, sim_colorcomponent_ambient_diffuse, voxel_color);
        tissueFile << listOfCoordinates(i,0) << ',' << listOfCoordinates(i,1) << ',' << listOfCoordinates(i,2) << std::endl;
    }
    // Close tissue file
    tissueFile.close();

    int* a = RobotMorphology.cubeHandles.data();
    int body = simGroupShapes(a, RobotMorphology.cubeHandles.size());
    simSetObjectName(body, "robotShape");

    // Set parents
    for(int i = 0; i < organsNumber; i++){
        simSetObjectParent(forceSensor[i], body, 1);
        simSetObjectParent(motorOrgan[i], forceSensor[i], 1);
    }
    simSetObjectParent(forceSensor3, body, 1);
    simSetObjectParent(brainOrgan, forceSensor3, 1);

    // Create collection
    int collection_handle = simCreateCollection("robotShape", 1); // This has to be before simAddObjectToCollection
    simAddObjectToCollection(collection_handle, body, sim_handle_single, 0);
    return simGetObjectHandle("robotShape");
}

void Tissue_GMX_VREP::create()
{
	std::cout << "State: CREATE!" << std::endl;
    mutate();
	mainHandle = createRobot();
}

void Tissue_GMX_VREP::init() {
	std::cout << "State: INIT!" << std::endl;
	initMorphology();
	//mutate();
	//	unique_ptr<ControlFactory> controlFactory(new ControlFactory);
	//	control = controlFactory->createNewControlGenome(0, randomNum, settings); // ann
	//	controlFactory.reset();
	//	control->init(2, 50, 2);
	//	control->mutate(0.5);
	//create();
}
void Tissue_GMX_VREP::mutate()
{
	std::cout << "State: MUTATE!" << std::endl;
	//    if (control) {
	//        control->mutate(settings->mutationRate);
	//    }
	//Tissue_GMX_VREP(settings->morphMutRate); (Frank has no clue what's going on here ???)
	mutateMorphology(settings->morphMutRate);
}

void Tissue_GMX_VREP::saveGenome(int indNum, float fitness)
{
	std::cout << "State: SAVE GENOME!" << std::endl;
	std::cout << "saving direct genome " << std::endl << "-------------------------------- " << std::endl;
	//	int evolutionType = 0; // regular evolution, will be changed in the future.
	std::cout << settings->repository << std::endl;
	ofstream genomeFile;
	ostringstream genomeFileName;
	genomeFileName << settings->repository + "/morphologies0" << "/genome" << indNum << ".csv";
	// genomeFileName << settings->repository + "/morphologies" << sceneNum << "/genome" << indNum << ".csv"; // Scenenum = -1
	//	genomeFileName << indNum << ".csv";
	genomeFile.open(genomeFileName.str());
	genomeFile << "#Individual:" << indNum << endl;
	genomeFile << "#Fitness:," << fitness << endl;
	//	cout << "#AmountStates:," << amountStates << "," << endl << endl;

	genomeFile << "#MorphologyParams:," << endl;
	for (int i = 0; i < gaussians.size(); i++) {
		//genomeFile << "\t" << "#Gaussian" << i << "," << std::endl;
		for (int j = 0; j < gaussians[i].medians.size(); j++) {
			//genomeFile << "\t\t" << "#Median" << j << ',' << gaussians[i].medians[j] << "," << std::endl;
			genomeFile << gaussians[i].medians[j] << ",\n";
		}
		for (int j = 0; j < gaussians[i].sigmas.size(); j++) {
			//genomeFile << "\t\t" << "#Sigma" << j << ',' << gaussians[i].sigmas[j] << "," << std::endl;
			genomeFile << gaussians[i].sigmas[j] << ",\n";
		}
	}
	for (int i = 0; i < organs.size(); i++) {
		//genomeFile << "\t" << "#Organ" << i << "," << std::endl;
		for (int j = 0; j < organs[i].coordinates.size(); j++) {
			//genomeFile << "\t\t" << "#Coordinate" << j << ',' << organs[i].coordinates[j] << "," << std::endl;
			genomeFile << organs[i].coordinates[j] << ",\n";
		}
	}
	genomeFile << "#EndOfMorphology," << endl;
	//	genomeFile << "#ControlParams:," << endl;
	//	genomeFile << control->getControlParams().str();
	//	genomeFile << "#EndOfControl," << endl << endl;
	genomeFile << "#EndGenome," << endl << endl;
	genomeFile.close();
}
bool Tissue_GMX_VREP::loadGenome(int individualNumber, int sceneNum) {
	std::cout << "State: LOAD GENOME!" << std::endl;
	cout << "loading cat genome " << individualNumber << endl;
	unique_ptr<ControlFactory> controlFactory(new ControlFactory);
	control = controlFactory->createNewControlGenome(0, randomNum, settings); // ann
	controlFactory.reset();
	ostringstream genomeFileName;
	genomeFileName << settings->repository + "/morphologies0" << "/genome" << individualNumber << ".csv";
	//genomeFileName << settings->repository + "/morphologies" << sceneNum << "/genome" << individualNumber << ".csv";
	ifstream genomeFile(genomeFileName.str());
	string value;
	list<string> values;
	while (genomeFile.good()) {
		getline(genomeFile, value, ',');
		//	cout << value << ",";
		if (value.find('\n') != string::npos) {
			split_line(value, "\n", values);
		}
		else {
			values.push_back(value);
		}
	}
	vector<string> controlValues;
	bool checkingControl = false;

	list<string>::const_iterator it = values.begin();
	for (it = values.begin(); it != values.end(); it++) {
		string tmp = *it;
		if (checkingControl == true) {
			controlValues.push_back(tmp);
		}
		if (tmp == "#Fitness:") {
			it++;
			tmp = *it;
			fitness = atof(tmp.c_str());
		}
		if (tmp == "#ControlParams:") {
			checkingControl = true;
		}
		if (tmp == "#EndControlParams") {
			//cout << "setting control params" << endl;
			control->setControlParams(controlValues);
			controlValues.clear();
			checkingControl = false;
		}
	}
	// Load genome for morphology
	initMorphology(); // Initialize morphology
	std::cout << "Init Morph!" << std::endl;
	vector<string> morphologyValues;
	bool checkingMorphology = false;


	for (it = values.begin(); it != values.end(); it++) {
		string tmp = *it;
		if (checkingMorphology == true) {
			morphologyValues.push_back(tmp);
		}
		//        if (tmp == "#Fitness:") {
		//            it++;
		//            tmp = *it;
		//            fitness = atof(tmp.c_str());
		//        }
		if (tmp == "#MorphologyParams:") {
			checkingMorphology = true;
		}
		if (tmp == "#EndOfMorphology") {
			//cout << "setting control params" << endl;
			//control->setControlParams(controlValues);
			//controlValues.clear();
			checkingMorphology = false;
		}
	}
	std::cout << "Print!" << std::endl;
	for (int i = 0; i < 30; i++) {
		std::cout << morphologyValues[i] << std::endl;
	}

	// Load values
	int genomeCounter = 1; //
	organsNumber = stof(morphologyValues[genomeCounter]);
	genomeCounter += 2;
	for (int i = 0; i < gaussians.size(); i++) {
		for (int j = 0; j < gaussians[i].medians.size(); j++) {
			gaussians[i].medians[j] = stof(morphologyValues[genomeCounter]);
			genomeCounter += 2;
		}
		for (int j = 0; j < gaussians[i].sigmas.size(); j++) {
			gaussians[i].sigmas[j] = stof(morphologyValues[genomeCounter]);
			genomeCounter += 2;
		}
	}
	for (int i = 0; i < organsNumber; i++) {
		for (int j = 0; j < organs[i].coordinates.size(); j++) {
			organs[i].coordinates[j] = stof(morphologyValues[genomeCounter]);
			genomeCounter += 2;
		}
		for (int j = 0; j < organs[i].orientations.size(); j++) {
			organs[i].orientations[j] = stof(morphologyValues[genomeCounter]);
			genomeCounter += 2;
		}
	}
	return true; // TODO: Change this to only return true when genome is correctly loaded
}
// Initialize variables for morphology
void Tissue_GMX_VREP::initMorphology() {
	// Number of gaussians

	gaussians.resize(3);
	for(int i = 0; i < gaussians.size(); i++){
		gaussians[i].medians.resize(3);
		gaussians[i].sigmas.resize(3);
	}
	//TODO: Remove unnecessary organs
	// Number of organs
	organs.resize(10);
	for (int i = 0; i < organs.size(); i++) {
		organs[i].coordinates.resize(3);
		organs[i].orientations.resize(3);
	}
}
// Mutate parameters for Moorphology
void Tissue_GMX_VREP::mutateMorphology(float mutationRate) {
    // Generate gaussians
    organsNumber = 2;
	for (int i = 0; i < gaussians.size(); i++) {
		for (int j = 0; j < gaussians[i].medians.size(); j++) {
			if (mutationRate < randomNum->randFloat(0, 1)) {
                if(j!=2){ // Make sure to generate coordinates above the ground
                    gaussians[i].medians[j] = randomNum->randFloat(-5.0, 5.0);
                }
                else{
                    gaussians[i].medians[j] = randomNum->randFloat(0.0, 10.0);
                }

			}
		}
		for (int j = 0; j < gaussians[i].sigmas.size(); j++) {
			if (mutationRate < randomNum->randFloat(0, 1)) {
				gaussians[i].sigmas[j] = randomNum->randFloat(0.0, 4.0);
			}
		}
	}
	for (int i = 0; i < organs.size(); i++) {
		for (int j = 0; j < organs[i].coordinates.size(); j++) { // Mutate coordinates
			if (settings->morphMutRate < randomNum->randFloat(0, 1)) {
				if(j!=2){ // Make sure to generate coordinates above the ground
					organs[i].coordinates[j] = randomNum->randFloat(-5.0, 5.0); // 3D printer build volumen
				}
				else{
					organs[i].coordinates[j] = randomNum->randFloat(0.0, 10.0); // 3D printer build volumen
				}
			}
		}
//		for (int j = 0; j < organs[i].orientations.size(); j++) { // Mutate orientations
//			if (settings->morphMutRate < randomNum->randFloat(0, 1)) {
//				organs[i].orientations[j] = randomNum->randFloat(0.0, 6.28319);
//			}
//		}
        organs[i].orientations[0] = 1.57080;
        organs[i].orientations[2] = 0.0;
        organs[i].orientations[2] = -1.57080;
	}
}
// Create list of voxels from points
// TODO: Move from here
template <typename Derived>
Eigen::MatrixXd createVoxelsFromPoints(const Eigen::MatrixBase<Derived>& allSamples) {
	// Generate voxels.
	Eigen::MatrixXd listOfCoordinates(1, 3);
	int listOfCoordiantesIndex = 0;
	//TODO: Remove this and adsjut when granularity has been defined.
	int threshold = 0;
	int granularity = 1;
	for (int i = -10 * granularity; i <= 10 * granularity; i++)
	{
		for (int j = -10 * granularity; j <= 10 * granularity; j++)
		{
			for (int k = -10 * granularity; k <= 10 * granularity; k++)
			{
				float i_float = (float)i / granularity;
				float j_float = (float)j / granularity;
				float k_float = (float)k / granularity;
				int numberOfPoints = 0;
				for (int m = 0; m < allSamples.cols(); m++)
				{
					if ((allSamples(0, m) < i_float + 1) && (allSamples(0, m) > i_float - 1) &&
						(allSamples(1, m) < j_float + 1) && (allSamples(1, m) > j_float - 1) &&
						(allSamples(2, m) < k_float + 1) && (allSamples(2, m) > k_float - 1))
					{
						numberOfPoints++;

					}
				}
				if (numberOfPoints > threshold)
				{
					listOfCoordinates.conservativeResize(listOfCoordiantesIndex + 1, 3);
					listOfCoordinates(listOfCoordiantesIndex, 0) = i_float;
					listOfCoordinates(listOfCoordiantesIndex, 1) = j_float;
					listOfCoordinates(listOfCoordiantesIndex, 2) = k_float;
					listOfCoordiantesIndex++;
				}

			}
		}
	}
	//std::cout << "List of coordinates for voxels:" << std::endl;
	//std::cout << listOfCoordinates << std::endl;

	std::ofstream myfile;
	myfile.open("example.csv");

	myfile << listOfCoordinates;
	myfile.close();
	return listOfCoordinates;
}

