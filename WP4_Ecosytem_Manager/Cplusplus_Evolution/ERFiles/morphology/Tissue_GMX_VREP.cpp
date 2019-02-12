#include "Tissue_GMX_VREP.h"



Tissue_GMX_VREP::Tissue_GMX_VREP()
{
}


Tissue_GMX_VREP::~Tissue_GMX_VREP()
{
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



	// Create robot
	// TODO: Check if this necessary with WL
	int nextRobotMorphologyHandle = 0;
	int handle = -1;
	sRobotMorphology RobotMorphology;
	handle = nextRobotMorphologyHandle++;
	RobotMorphology.handle = handle;

	// Importing motor organs
	int motorOrgan1 = simLoadModel("models/motorOrgan2.ttm");
	int motorOrgan2 = simLoadModel("models/motorOrgan2.ttm");
	int brainOrgan = simLoadModel("models/brainOrgan.ttm");

	// Create force sensors to place organs
	int pamsArg1[] = { 0,0,0,0,0 };
	float pamsArg2[] = { 0,0,0,0,0 };
	int forceSensor1 = simCreateForceSensor(0, pamsArg1, pamsArg2, NULL);
	int forceSensor2 = simCreateForceSensor(0, pamsArg1, pamsArg2, NULL);
	int forceSensor3 = simCreateForceSensor(0, pamsArg1, pamsArg2, NULL);

	// TODO: EB to Create dummies

	// Create voxels
	std::vector <int> cubeHandles;

	float organ1_pos[] = { organs[0].coordinates[0] / 100, organs[0].coordinates[1] / 100, organs[0].coordinates[2] / 100 };
	float organ2_pos[] = { organs[1].coordinates[0] / 100, organs[1].coordinates[1] / 100, organs[1].coordinates[2] / 100 };
	float organ3_pos[] = { 0.1, 0.1, 0.0 };

	float organ1_angle[] = { 1.57080, 0, -1.57080 };
	float organ2_angle[] = { 1.57080, 0, 1.57080 }; // 3.151516 for second orientation
	float organ3_angle[] = { 0, 1.57080, 0 };

	// Create voxel
	int temp_voxel_handle;
	float columnWidth = 0.015;
	float columnHeight = 0.015;
	float magnitude;
	float angle;
	float voxel_size[3];
	float voxel_pos[3];
	float voxel_ori[3];
	float voxel_color[3];

	// Organ 1

	magnitude = sqrt(pow(organ1_pos[0] - organ3_pos[0], 2) + pow(organ1_pos[1] - organ3_pos[1], 2)) + columnWidth;
	angle = atan2(organ1_pos[1] - organ3_pos[1], organ1_pos[0] - organ3_pos[0]);

	voxel_size[0] = magnitude;
	voxel_size[1] = columnWidth;
	voxel_size[2] = columnHeight;
	temp_voxel_handle = simCreatePureShape(0, 8, voxel_size, 1, NULL);

	RobotMorphology.cubeHandles.push_back(temp_voxel_handle);

	voxel_pos[0] = organ3_pos[0] + (organ1_pos[0] - organ3_pos[0]) / 2;
	voxel_pos[1] = organ3_pos[1] + (organ1_pos[1] - organ3_pos[1]) / 2;
	voxel_pos[2] = 0;
	simSetObjectPosition(temp_voxel_handle, -1, voxel_pos);


	voxel_ori[0] = 0;
	voxel_ori[1] = 0;
	voxel_ori[2] = angle;
	simSetObjectOrientation(temp_voxel_handle, -1, voxel_ori);

	voxel_color[0] = 0;
	voxel_color[1] = 1;
	voxel_color[2] = 0;
	simSetShapeColor(temp_voxel_handle, NULL, sim_colorcomponent_ambient_diffuse, voxel_color);

	// Second column

	voxel_size[0] = columnWidth;
	voxel_size[1] = columnHeight;
	voxel_size[2] = abs(organ1_pos[2] - organ3_pos[2] - columnWidth);
	temp_voxel_handle = simCreatePureShape(2, 8, voxel_size, 1, NULL);
	RobotMorphology.cubeHandles.push_back(temp_voxel_handle);

	voxel_pos[0] = organ1_pos[0];
	voxel_pos[1] = organ1_pos[1];
	voxel_pos[2] = organ3_pos[2] + (organ1_pos[2] - organ3_pos[2]) / 2;
	simSetObjectPosition(temp_voxel_handle, -1, voxel_pos);

	voxel_ori[0] = 0;
	voxel_ori[1] = 0;
	voxel_ori[2] = angle;
	simSetObjectOrientation(temp_voxel_handle, -1, voxel_ori);

	voxel_color[0] = 0;
	voxel_color[1] = 1;
	voxel_color[2] = 0;
	simSetShapeColor(temp_voxel_handle, NULL, sim_colorcomponent_ambient_diffuse, voxel_color);

	// Organ 2

	magnitude = sqrt(pow(organ2_pos[0] - organ3_pos[0], 2) + pow(organ2_pos[1] - organ2_pos[1], 2)) + columnWidth;
	angle = atan2(organ2_pos[1] - organ3_pos[1], organ2_pos[0] - organ3_pos[0]);

	voxel_size[0] = magnitude;
	voxel_size[1] = columnWidth;
	voxel_size[2] = columnHeight;
	temp_voxel_handle = simCreatePureShape(0, 8, voxel_size, 1, NULL);

	RobotMorphology.cubeHandles.push_back(temp_voxel_handle);

	voxel_pos[0] = organ3_pos[0] + (organ2_pos[0] - organ3_pos[0]) / 2;
	voxel_pos[1] = organ3_pos[1] + (organ2_pos[1] - organ3_pos[1]) / 2;
	voxel_pos[2] = 0;
	simSetObjectPosition(temp_voxel_handle, -1, voxel_pos);


	voxel_ori[0] = 0;
	voxel_ori[1] = 0;
	voxel_ori[2] = angle;
	simSetObjectOrientation(temp_voxel_handle, -1, voxel_ori);

	voxel_color[0] = 0;
	voxel_color[1] = 1;
	voxel_color[2] = 0;
	simSetShapeColor(temp_voxel_handle, NULL, sim_colorcomponent_ambient_diffuse, voxel_color);

	// Second column

	voxel_size[0] = columnWidth;
	voxel_size[1] = columnHeight;
	voxel_size[2] = abs(organ2_pos[2] - organ3_pos[2] - columnWidth);
	temp_voxel_handle = simCreatePureShape(2, 8, voxel_size, 1, NULL);
	RobotMorphology.cubeHandles.push_back(temp_voxel_handle);

	voxel_pos[0] = organ2_pos[0];
	voxel_pos[1] = organ2_pos[1];
	voxel_pos[2] = organ3_pos[2] + (organ2_pos[2] - organ3_pos[2]) / 2;
	simSetObjectPosition(temp_voxel_handle, -1, voxel_pos);

	voxel_ori[0] = 0;
	voxel_ori[1] = 0;
	voxel_ori[2] = angle;
	simSetObjectOrientation(temp_voxel_handle, -1, voxel_ori);

	voxel_color[0] = 0;
	voxel_color[1] = 1;
	voxel_color[2] = 0;
	simSetShapeColor(temp_voxel_handle, NULL, sim_colorcomponent_ambient_diffuse, voxel_color);

	//	// Set organs position
	organ1_pos[2] = organ1_pos[2] + 0.01;
	simSetObjectPosition(motorOrgan1, -1, organ1_pos);
	simSetObjectPosition(forceSensor1, -1, organ1_pos);
	organ2_pos[2] = organ2_pos[2] + 0.01;
	simSetObjectPosition(motorOrgan2, -1, organ2_pos);
	simSetObjectPosition(forceSensor2, -1, organ2_pos);
	organ3_pos[2] = 0.03;
	simSetObjectPosition(brainOrgan, -1, organ3_pos);
	simSetObjectPosition(forceSensor3, -1, organ3_pos);
	//
	//	// Set organs orientation
	simSetObjectOrientation(motorOrgan1, -1, organ1_angle);
	simSetObjectOrientation(motorOrgan2, -1, organ2_angle);
	simSetObjectOrientation(brainOrgan, -1, organ3_angle);
	simSetObjectOrientation(forceSensor1, -1, organ1_angle);
	simSetObjectOrientation(forceSensor2, -1, organ2_angle);
	simSetObjectOrientation(forceSensor3, -1, organ3_angle);

	int* a = RobotMorphology.cubeHandles.data();
	int body = simGroupShapes(a, RobotMorphology.cubeHandles.size());
	printf("size: %d\n", RobotMorphology.cubeHandles.size());
	simSetObjectName(body, "robotShape");

	//    // Set parents
	simSetObjectParent(forceSensor1, body, 1);
	simSetObjectParent(forceSensor2, body, 1);
	simSetObjectParent(forceSensor3, body, 1);
	simSetObjectParent(motorOrgan1, forceSensor1, 1);
	simSetObjectParent(motorOrgan2, forceSensor2, 1);
	simSetObjectParent(brainOrgan, forceSensor3, 1);

	// Create collection
	int collection_handle = simCreateCollection("robotShape", 1); // This has to be before simAddObjectToCollection
	simAddObjectToCollection(collection_handle, body, sim_handle_single, 0);

	ofstream coordinatesFile;
	ostringstream coordinatesFileName;
	coordinatesFileName << settings->repository + "/stl0" << "/stl" << ".csv";
	coordinatesFile.open(coordinatesFileName.str());
	// First coordinates reserved for organs.
	coordinatesFile << organs[0].coordinates[0] << ',' << organs[0].coordinates[1] << ',' << organs[0].coordinates[2] << std::endl;
	coordinatesFile << organs[1].coordinates[0] << ',' << organs[1].coordinates[1] << ',' << organs[1].coordinates[2] << std::endl;
	coordinatesFile << 10.0 << ',' << 10.0 << ',' << 0.0 << std::endl;
	// Close tissue file
	coordinatesFile.close();


	return simGetObjectHandle("robotShape");
}

void Tissue_GMX_VREP::create()
{
	std::cout << "State: CREATE!" << std::endl;
	mainHandle = createRobot();
}

void Tissue_GMX_VREP::init() {
	std::cout << "State: INIT!" << std::endl;
	initMorphology();
	mutate();
	//	unique_ptr<ControlFactory> controlFactory(new ControlFactory);
	//	control = controlFactory->createNewControlGenome(0, randomNum, settings); // ann
	//	controlFactory.reset();
	//	control->init(2, 50, 2);
	//	control->mutate(0.5);
	create();
}
void Tissue_GMX_VREP::mutate()
{
	std::cout << "State: MUTATE!" << std::endl;
	//    if (control) {
	//        control->mutate(settings->mutationRate);
	//    }
	//Tissue_GMX_VREP(settings->morphMutRate); (Frank has no clue what's going on here ???)
}

void Tissue_GMX_VREP::saveGenome(int indNum, int sceneNum, float fitness)
{
	std::cout << "State: SAVE GENOME!" << std::endl;
	std::cout << "saving direct genome " << std::endl << "-------------------------------- " << std::endl;
	//	int evolutionType = 0; // regular evolution, will be changed in the future.
	std::cout << settings->repository << std::endl;
	std::cout << sceneNum << std::endl;
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


	gaussians[0].medians[0] = stof(morphologyValues[1]);
	gaussians[0].medians[1] = stof(morphologyValues[3]);
	gaussians[0].medians[2] = stof(morphologyValues[5]);
	gaussians[0].sigmas[0] = stof(morphologyValues[7]);
	gaussians[0].sigmas[1] = stof(morphologyValues[9]);
	gaussians[0].sigmas[2] = stof(morphologyValues[11]);
	gaussians[1].medians[0] = stof(morphologyValues[13]);
	gaussians[1].medians[1] = stof(morphologyValues[15]);
	gaussians[1].medians[2] = stof(morphologyValues[17]);
	gaussians[1].sigmas[0] = stof(morphologyValues[19]);
	gaussians[1].sigmas[1] = stof(morphologyValues[21]);
	gaussians[1].sigmas[2] = stof(morphologyValues[23]);
	gaussians[2].medians[0] = stof(morphologyValues[25]);
	gaussians[2].medians[1] = stof(morphologyValues[27]);
	gaussians[2].medians[2] = stof(morphologyValues[29]);
	gaussians[2].sigmas[0] = stof(morphologyValues[31]);
	gaussians[2].sigmas[1] = stof(morphologyValues[33]);
	gaussians[2].sigmas[2] = stof(morphologyValues[35]);
	//TODO: Remove unnecessary organs
	organs[0].coordinates[0] = stof(morphologyValues[37]);
	organs[0].coordinates[1] = stof(morphologyValues[39]);
	organs[0].coordinates[2] = stof(morphologyValues[41]);
	organs[1].coordinates[0] = stof(morphologyValues[43]);
	organs[1].coordinates[1] = stof(morphologyValues[45]);
	organs[1].coordinates[2] = stof(morphologyValues[47]);
	organs[2].coordinates[0] = stof(morphologyValues[49]);
	organs[2].coordinates[1] = stof(morphologyValues[51]);
	organs[2].coordinates[2] = stof(morphologyValues[53]);
	organs[3].coordinates[0] = stof(morphologyValues[55]);
	organs[3].coordinates[1] = stof(morphologyValues[57]);
	organs[3].coordinates[2] = stof(morphologyValues[59]);
	organs[4].coordinates[0] = stof(morphologyValues[61]);
	organs[4].coordinates[1] = stof(morphologyValues[63]);
	organs[4].coordinates[2] = stof(morphologyValues[65]);
	return true; // TODO: Change this to only return true when genome is correctly loaded
}
// Initialize variables for morphology
void Tissue_GMX_VREP::initMorphology() {
	// Number of gaussians
	gaussians.resize(3);
	gaussians[0].medians.resize(3);
	gaussians[0].sigmas.resize(3);
	gaussians[1].medians.resize(3);
	gaussians[1].sigmas.resize(3);
	gaussians[2].medians.resize(3);
	gaussians[2].sigmas.resize(3);
	//TODO: Remove unnecessary organs
	// Number of organs
	organs.resize(5);
	organs[0].coordinates.resize(3);
	organs[1].coordinates.resize(3);
	organs[2].coordinates.resize(3);
	organs[3].coordinates.resize(3);
	organs[4].coordinates.resize(3);
}
// Mutate parameters for Moorphology
void Tissue_GMX_VREP::mutateMorphology(float mutationRate) {
	for (int i = 0; i < gaussians.size(); i++) {
		for (int j = 0; j < gaussians[i].medians.size(); j++) {
			if (mutationRate < randomNum->randFloat(0, 1)) {
				gaussians[i].medians[j] = randomNum->randFloat(0.0, 5);
			}
		}
		for (int j = 0; j < gaussians[i].sigmas.size(); j++) {
			if (mutationRate < randomNum->randFloat(0, 1)) {
				gaussians[i].sigmas[j] = randomNum->randFloat(0.0, 10.0);
			}
		}
	}
	for (int i = 0; i < organs.size(); i++) {
		bool isOrganColliding;
		do {
			isOrganColliding = false;
			for (int j = 0; j < organs[i].coordinates.size(); j++) {
				//if (mutationRate < randomNum->randFloat(0, 1)) {
				if (0.0 < randomNum->randFloat(0, 1)) {
					organs[i].coordinates[j] = randomNum->randFloat(0.0, 20);
					std::cout << organs[i].coordinates[j] << std::endl;
				}
			}
			if (i > 0) {
				// Wheel 1
				if (organs[i].coordinates[0] / 100 > organs[1].coordinates[0] / 100 - (0.07 / 2) // Original 0.05
					&& organs[i].coordinates[0] / 100 < organs[1].coordinates[0] / 100 + (0.07 / 2)
					&& organs[i].coordinates[1] / 100 > organs[1].coordinates[1] / 100 - 0.035 - (0.02 / 2)  // Original 0.05
					&& organs[i].coordinates[1] / 100 < organs[1].coordinates[1] / 100 + 05574 + (0.02 / 2)
					&& organs[i].coordinates[2] / 100 > organs[1].coordinates[2] / 100 - (0.07 / 2) // Original 0.01
					&& organs[i].coordinates[2] / 100 < organs[1].coordinates[2] / 100 + (0.07 / 2)) {
					true;
				}
				if (organs[i].coordinates[0] / 100 > organs[2].coordinates[0] / 100 - (0.07 / 2) // Original 0.05
					&& organs[i].coordinates[0] / 100 < organs[2].coordinates[0] / 100 + (0.07 / 2)
					&& organs[i].coordinates[1] / 100 > organs[2].coordinates[1] / 100 - 05574 - (0.03 / 2)  // Original 0.05
					&& organs[i].coordinates[1] / 100 < organs[2].coordinates[1] / 100 + 0.035 + (0.03 / 2)
					&& organs[i].coordinates[2] / 100 > organs[2].coordinates[2] / 100 - (0.07 / 2) // Original 0.01
					&& organs[i].coordinates[2] / 100 < organs[2].coordinates[2] / 100 + (0.07 / 2)) {
					true;
				}
			}
		} while (isOrganColliding);
		// DEBUG
//        organs[0].coordinates[0] = 8;
//        organs[0].coordinates[1] = 8;
//        organs[0].coordinates[2] = 3;
//        organs[1].coordinates[0] = 0;
//        organs[1].coordinates[1] = 8;
//        organs[1].coordinates[2] = 3;
//        organs[2].coordinates[0] = 8;
//        organs[2].coordinates[1] = 0;
//        organs[2].coordinates[2] = 3;
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

