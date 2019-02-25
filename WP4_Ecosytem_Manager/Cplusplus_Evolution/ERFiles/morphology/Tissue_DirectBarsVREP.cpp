#include "Tissue_DirectBarsVREP.h"



Tissue_DirectBarsVREP::Tissue_DirectBarsVREP()
{
}


Tissue_DirectBarsVREP::~Tissue_DirectBarsVREP()
{
}

shared_ptr<Morphology> Tissue_DirectBarsVREP::clone() const {
    // shared_ptr<Morphology>()
    shared_ptr<Tissue_DirectBarsVREP> tissue_directbars = make_unique<Tissue_DirectBarsVREP>(*this);
	tissue_directbars->organs = this->organs;
	//tissue_directbars->control = control->clone();
    return tissue_directbars;
}

vector<int> Tissue_DirectBarsVREP::getObjectHandles(int parentHandle)
{
	return vector<int>();
}

vector<int> Tissue_DirectBarsVREP::getJointHandles(int parentHandle)
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

void Tissue_DirectBarsVREP::update() {
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

int Tissue_DirectBarsVREP::getMainHandle()
{
	return mainHandle;
}

struct sRobotMorphology
{
	int handle;
	int sensorHandles[2];
	std::vector<int> cubeHandles;
};


int Tissue_DirectBarsVREP::createRobot() {

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
    float organPos[2];
    float organOri[2];
	float organ1_pos[] = { organs[0].coordinates[0] / 100, organs[0].coordinates[1] / 100, organs[0].coordinates[2] / 100 };
	float organ2_pos[] = { organs[1].coordinates[0] / 100, organs[1].coordinates[1] / 100, organs[1].coordinates[2] / 100 };
	float brainPos[] = { 0.05, 0.05, 0.0 };
//	float organ1_angle[] = { 1.57080, 0, -1.57080 };
//	float organ2_angle[] = { 1.57080, 0, 1.57080 }; // 3.151516 for second orientation
    float organ1_angle[] = { 1.57080, organs[0].orientations[1], -1.57080 };
    float organ2_angle[] = { 1.57080, organs[1].orientations[1], 1.57080 }; // 3.151516 for second orientation
	float brainOri[] = { 0, 1.57080, 0 };

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


	for(int counter = 0; counter < 2; counter++){
        magnitude = sqrt(pow(organs[counter].coordinates[0] / 100 - brainPos[0], 2) + pow(organs[counter].coordinates[1] / 100 - brainPos[1], 2)) + columnWidth;
        angle = atan2(organs[counter].coordinates[1] / 100 - brainPos[1], organs[counter].coordinates[0] / 100 - brainPos[0]);

        voxel_size[0] = magnitude;
        voxel_size[1] = columnWidth;
        voxel_size[2] = columnHeight;
        temp_voxel_handle = simCreatePureShape(0, 8, voxel_size, 1, NULL);
        RobotMorphology.cubeHandles.push_back(temp_voxel_handle);

        voxel_pos[0] = brainPos[0] + (organs[counter].coordinates[0] / 100 - brainPos[0]) / 2;
        voxel_pos[1] = brainPos[1] + (organs[counter].coordinates[1] / 100 - brainPos[1]) / 2;
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
        voxel_size[2] = abs(organs[counter].coordinates[2] / 100 - brainPos[2] - columnWidth);
        temp_voxel_handle = simCreatePureShape(2, 8, voxel_size, 1, NULL);
        RobotMorphology.cubeHandles.push_back(temp_voxel_handle);

        voxel_pos[0] = organ1_pos[0];
        voxel_pos[1] = organ1_pos[1];
        voxel_pos[2] = brainPos[2] + (organs[counter].coordinates[2] / 100 - brainPos[2]) / 2;
        simSetObjectPosition(temp_voxel_handle, -1, voxel_pos);

        voxel_ori[0] = 0;
        voxel_ori[1] = 0;
        voxel_ori[2] = angle;
        simSetObjectOrientation(temp_voxel_handle, -1, voxel_ori);

        voxel_color[0] = 0;
        voxel_color[1] = 1;
        voxel_color[2] = 0;
        simSetShapeColor(temp_voxel_handle, NULL, sim_colorcomponent_ambient_diffuse, voxel_color);
	}
	// Organ 1

//	magnitude = sqrt(pow(organs[0].coordinates[0] / 100 - brainPos[0], 2) + pow(organs[0].coordinates[1] / 100 - brainPos[1], 2)) + columnWidth;
//	angle = atan2(organs[0].coordinates[1] / 100 - brainPos[1], organs[0].coordinates[0] / 100 - brainPos[0]);
//
//	voxel_size[0] = magnitude;
//	voxel_size[1] = columnWidth;
//	voxel_size[2] = columnHeight;
//	temp_voxel_handle = simCreatePureShape(0, 8, voxel_size, 1, NULL);
//
//	RobotMorphology.cubeHandles.push_back(temp_voxel_handle);
//
//	voxel_pos[0] = brainPos[0] + (organs[0].coordinates[0] / 100 - brainPos[0]) / 2;
//	voxel_pos[1] = brainPos[1] + (organs[0].coordinates[1] / 100 - brainPos[1]) / 2;
//	voxel_pos[2] = 0;
//	simSetObjectPosition(temp_voxel_handle, -1, voxel_pos);
//
//
//	voxel_ori[0] = 0;
//	voxel_ori[1] = 0;
//	voxel_ori[2] = angle;
//	simSetObjectOrientation(temp_voxel_handle, -1, voxel_ori);
//
//	voxel_color[0] = 0;
//	voxel_color[1] = 1;
//	voxel_color[2] = 0;
//	simSetShapeColor(temp_voxel_handle, NULL, sim_colorcomponent_ambient_diffuse, voxel_color);
//
//	// Second column
//
//	voxel_size[0] = columnWidth;
//	voxel_size[1] = columnHeight;
//	voxel_size[2] = abs(organs[0].coordinates[2] / 100 - brainPos[2] - columnWidth);
//	temp_voxel_handle = simCreatePureShape(2, 8, voxel_size, 1, NULL);
//	RobotMorphology.cubeHandles.push_back(temp_voxel_handle);
//
//	voxel_pos[0] = organ1_pos[0];
//	voxel_pos[1] = organ1_pos[1];
//	voxel_pos[2] = brainPos[2] + (organs[0].coordinates[2] / 100 - brainPos[2]) / 2;
//	simSetObjectPosition(temp_voxel_handle, -1, voxel_pos);
//
//	voxel_ori[0] = 0;
//	voxel_ori[1] = 0;
//	voxel_ori[2] = angle;
//	simSetObjectOrientation(temp_voxel_handle, -1, voxel_ori);
//
//	voxel_color[0] = 0;
//	voxel_color[1] = 1;
//	voxel_color[2] = 0;
//	simSetShapeColor(temp_voxel_handle, NULL, sim_colorcomponent_ambient_diffuse, voxel_color);
//
//	// Organ 2
//
//	magnitude = sqrt(pow(organs[1].coordinates[0] / 100 - brainPos[0], 2) + pow(organ2_pos[1] - organs[1].coordinates[1] / 100, 2)) + columnWidth;
//	angle = atan2(organs[1].coordinates[1] / 100 - brainPos[1], organs[1].coordinates[0] / 100 - brainPos[0]);
//
//	voxel_size[0] = magnitude;
//	voxel_size[1] = columnWidth;
//	voxel_size[2] = columnHeight;
//	temp_voxel_handle = simCreatePureShape(0, 8, voxel_size, 1, NULL);
//
//	RobotMorphology.cubeHandles.push_back(temp_voxel_handle);
//
//	voxel_pos[0] = brainPos[0] + (organ2_pos[0] - brainPos[0]) / 2;
//	voxel_pos[1] = brainPos[1] + (organ2_pos[1] - brainPos[1]) / 2;
//	voxel_pos[2] = 0;
//	simSetObjectPosition(temp_voxel_handle, -1, voxel_pos);
//
//
//	voxel_ori[0] = 0;
//	voxel_ori[1] = 0;
//	voxel_ori[2] = angle;
//	simSetObjectOrientation(temp_voxel_handle, -1, voxel_ori);
//
//	voxel_color[0] = 0;
//	voxel_color[1] = 1;
//	voxel_color[2] = 0;
//	simSetShapeColor(temp_voxel_handle, NULL, sim_colorcomponent_ambient_diffuse, voxel_color);
//
//	// Second column
//
//	voxel_size[0] = columnWidth;
//	voxel_size[1] = columnHeight;
//	voxel_size[2] = abs(organs[1].coordinates[2] / 100 - brainPos[2] - columnWidth);
//	temp_voxel_handle = simCreatePureShape(2, 8, voxel_size, 1, NULL);
//	RobotMorphology.cubeHandles.push_back(temp_voxel_handle);
//
//	voxel_pos[0] = organ2_pos[0];
//	voxel_pos[1] = organ2_pos[1];
//	voxel_pos[2] = brainPos[2] + (organs[1].coordinates[2] / 100 - brainPos[2]) / 2;
//	simSetObjectPosition(temp_voxel_handle, -1, voxel_pos);
//
//	voxel_ori[0] = 0;
//	voxel_ori[1] = 0;
//	voxel_ori[2] = angle;
//	simSetObjectOrientation(temp_voxel_handle, -1, voxel_ori);
//
//	voxel_color[0] = 0;
//	voxel_color[1] = 1;
//	voxel_color[2] = 0;
//	simSetShapeColor(temp_voxel_handle, NULL, sim_colorcomponent_ambient_diffuse, voxel_color);

	//	// Set organs position
	organ1_pos[2] = organ1_pos[2] + 0.01;
	simSetObjectPosition(motorOrgan1, -1, organ1_pos);
	simSetObjectPosition(forceSensor1, -1, organ1_pos);
	organ2_pos[2] = organ2_pos[2] + 0.01;
	simSetObjectPosition(motorOrgan2, -1, organ2_pos);
	simSetObjectPosition(forceSensor2, -1, organ2_pos);
	brainPos[2] = 0.03;
	simSetObjectPosition(brainOrgan, -1, brainPos);
	simSetObjectPosition(forceSensor3, -1, brainPos);
	//
	//	// Set organs orientation
	simSetObjectOrientation(motorOrgan1, -1, organ1_angle);
	simSetObjectOrientation(motorOrgan2, -1, organ2_angle);
	simSetObjectOrientation(brainOrgan, -1, brainOri);
	simSetObjectOrientation(forceSensor1, -1, organ1_angle);
	simSetObjectOrientation(forceSensor2, -1, organ2_angle);
	simSetObjectOrientation(forceSensor3, -1, brainOri);

	int* a = RobotMorphology.cubeHandles.data();
	int body = simGroupShapes(a, RobotMorphology.cubeHandles.size());
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
	coordinatesFile << 5.0 << ',' << 5.0 << ',' << 0.0 << std::endl;
	// Close tissue file
	coordinatesFile.close();


	return simGetObjectHandle("robotShape");
}

void Tissue_DirectBarsVREP::create()
{
	std::cout << "State: CREATE!" << std::endl;
    //initMorphology();
	mutate();
	mainHandle = createRobot();
}

void Tissue_DirectBarsVREP::init() {
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
void Tissue_DirectBarsVREP::mutate()
{
	std::cout << "State: MUTATE!" << std::endl;
	//    if (control) {
	//        control->mutate(settings->mutationRate);
	//    }
	mutateMorphology(settings->morphMutRate);
}

void Tissue_DirectBarsVREP::saveGenome(int indNum, float fitness)
{
	std::cout << "State: SAVE GENOME!" << std::endl;
	std::cout << "saving direct genome " << std::endl << "-------------------------------- " << std::endl;
	//	int evolutionType = 0; // regular evolution, will be changed in the future.
	std::cout << settings->repository << std::endl;
	ofstream genomeFile;
	ostringstream genomeFileName;
    //genomeFileName << "files/morphologies0" << "/genome" << indNum << ".csv";
	genomeFileName << settings->repository + "/morphologies0" << "/genome" << indNum << ".csv";
	// genomeFileName << settings->repository + "/morphologies" << sceneNum << "/genome" << indNum << ".csv"; // Scenenum = -1
	//	genomeFileName << indNum << ".csv";
	std::cout << settings->repository << std::endl;
	genomeFile.open(genomeFileName.str());
	genomeFile << "#Individual:" << indNum << endl;
	genomeFile << "#Fitness:," << fitness << endl;
	//	cout << "#AmountStates:," << amountStates << "," << endl << endl;

	genomeFile << "#MorphologyParams:," << endl;

	for (int i = 0; i < organs.size(); i++) {
		//genomeFile << "\t" << "#Organ" << i << "," << std::endl;
		for (int j = 0; j < organs[i].coordinates.size(); j++) {
			//genomeFile << "\t\t" << "#Coordinate" << j << ',' << organs[i].coordinates[j] << "," << std::endl;
			genomeFile << organs[i].coordinates[j] << ",\n";
		}
        for (int j = 0; j < organs[i].orientations.size(); j++) {
            //genomeFile << "\t\t" << "#Orientation" << j << ',' << organs[i].coordinates[j] << "," << std::endl;
            genomeFile << organs[i].orientations[j] << ",\n";
        }
	}
	genomeFile << "#EndOfMorphology," << endl;
	//	genomeFile << "#ControlParams:," << endl;
	//	genomeFile << control->getControlParams().str();
	//	genomeFile << "#EndOfControl," << endl << endl;
	genomeFile << "#EndGenome," << endl << endl;
	genomeFile.close();
}
bool Tissue_DirectBarsVREP::loadGenome(int individualNumber, int sceneNum) {
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
    int genomeCounter = 1;
	//TODO: Remove unnecessary organs
    for (int i = 0; i < organs.size(); i++) {
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
void Tissue_DirectBarsVREP::initMorphology() {

    std::cout << "Init Morph!" << std::endl;
	//TODO: Remove unnecessary organs

	// Number of organs
	organs.resize(5);
	// Initialize coordinates and orientations
    for (int i = 0; i < organs.size(); i++) {
        organs[i].coordinates.resize(3);
        organs[i].orientations.resize(3);
    }
}
// Mutate parameters for Moorphology
void Tissue_DirectBarsVREP::mutateMorphology(float mutationRate) {
	for (int i = 0; i < organs.size(); i++) {
		bool isOrganColliding;
		do {
			isOrganColliding = false;
			for (int j = 0; j < organs[i].coordinates.size(); j++) {
				// Generate random position for each coordinate
				if (0.0 < randomNum->randFloat(0, 1)) {
					organs[i].coordinates[j] = randomNum->randFloat(0.0, 20);
					//std::cout << organs[i].coordinates[j] << std::endl;
				}

			}
            for (int j = 0; j < organs[i].orientations.size(); j++) {
                // Generate random position for each coordinate
                if (0.0 < randomNum->randFloat(0, 1)) {
                    organs[i].orientations[j] = randomNum->randFloat(0.0, 6.28319);
                    //std::cout << organs[i].coordinates[j] << std::endl;
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