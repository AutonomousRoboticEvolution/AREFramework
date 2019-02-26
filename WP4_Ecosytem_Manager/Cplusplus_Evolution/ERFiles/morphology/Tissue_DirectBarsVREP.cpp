#include "Tissue_DirectBarsVREP.h"

Tissue_DirectBarsVREP::Tissue_DirectBarsVREP()
{
}


Tissue_DirectBarsVREP::~Tissue_DirectBarsVREP()
{
}

shared_ptr<Morphology> Tissue_DirectBarsVREP::clone() const {
    std::cout << "State: Clone" << std::endl;
    shared_ptr<Tissue_DirectBarsVREP> tissue_directbars = make_unique<Tissue_DirectBarsVREP>(*this);
	tissue_directbars->organs = this->organs;
    tissue_directbars->organsNumber = this->organsNumber;
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
	int motorOrgan[organs.size()];
	int brainOrgan = simLoadModel("models/brainOrgan.ttm");

	// Create force sensors to place organs
	int pamsArg1[] = { 0,0,0,0,0 };
	float pamsArg2[] = { 0,0,0,0,0 };
    int forceSensor[organs.size()];
	int forceSensor3 = simCreateForceSensor(0, pamsArg1, pamsArg2, NULL);

	// TODO: EB to Create dummies

	// Create voxels
	std::vector <int> cubeHandles;
	float brainPos[] = { 0.0, 0.0, 0.0 };
//	float organ1_angle[] = { 1.57080, 0, -1.57080 };
//	float organ2_angle[] = { 1.57080, 0, 1.57080 }; // 3.151516 for second orientation
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

	for(int i = 0; i < organs.size(); i++){
        motorOrgan[i] = simLoadModel("models/motorOrgan2.ttm");
        forceSensor[i] = simCreateForceSensor(0, pamsArg1, pamsArg2, NULL);

        magnitude = sqrt(pow(organs[i].coordinates[0] / 100 - brainPos[0], 2) + pow(organs[i].coordinates[1] / 100 - brainPos[1], 2)) + columnWidth;
        angle = atan2(organs[i].coordinates[1] / 100 - brainPos[1], organs[i].coordinates[0] / 100 - brainPos[0]);

        voxel_size[0] = magnitude;
        voxel_size[1] = columnWidth;
        voxel_size[2] = columnHeight;
        temp_voxel_handle = simCreatePureShape(0, 8, voxel_size, 1, NULL);
        RobotMorphology.cubeHandles.push_back(temp_voxel_handle);

        voxel_pos[0] = brainPos[0] + (organs[i].coordinates[0] / 100 - brainPos[0]) / 2;
        voxel_pos[1] = brainPos[1] + (organs[i].coordinates[1] / 100 - brainPos[1]) / 2;
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
        voxel_size[2] = abs(organs[i].coordinates[2] / 100 - brainPos[2] - columnWidth);
        temp_voxel_handle = simCreatePureShape(2, 8, voxel_size, 1, NULL);
        RobotMorphology.cubeHandles.push_back(temp_voxel_handle);

        voxel_pos[0] = organs[i].coordinates[0] / 100;
        voxel_pos[1] = organs[i].coordinates[1] / 100;
        voxel_pos[2] = brainPos[2] + (organs[i].coordinates[2] / 100 - brainPos[2]) / 2;
        simSetObjectPosition(temp_voxel_handle, -1, voxel_pos);

        voxel_ori[0] = 0;
        voxel_ori[1] = 0;
        voxel_ori[2] = angle;
        simSetObjectOrientation(temp_voxel_handle, -1, voxel_ori);

        voxel_color[0] = 0;
        voxel_color[1] = 1;
        voxel_color[2] = 0;
        simSetShapeColor(temp_voxel_handle, NULL, sim_colorcomponent_ambient_diffuse, voxel_color);

        // Place organs
        // TODO: Try to remove these temporal variables.
        float tempPos[] = {organs[i].coordinates[0]/100, organs[i].coordinates[1]/100, organs[i].coordinates[2]/100 + 0.01}; // 0.02 Prevent organs colliding with tissue;
        simSetObjectPosition(motorOrgan[i], -1, tempPos);
        simSetObjectPosition(forceSensor[i], -1, tempPos);
        float tempOri[] = {organs[i].orientations[0],organs[i].orientations[1],organs[i].orientations[2]};
        simSetObjectOrientation(motorOrgan[i], -1, tempOri);
        simSetObjectOrientation(forceSensor[i], -1, tempOri);
	}

	// Set organs position
	brainPos[2] = 0.03;
	simSetObjectPosition(brainOrgan, -1, brainPos);
	simSetObjectPosition(forceSensor3, -1, brainPos);

	// Set organs orientation
	simSetObjectOrientation(brainOrgan, -1, brainOri);
	simSetObjectOrientation(forceSensor3, -1, brainOri);

	int* a = RobotMorphology.cubeHandles.data();
	int body = simGroupShapes(a, RobotMorphology.cubeHandles.size());
	simSetObjectName(body, "robotShape");

	//  Set parents
    for(int i = 0; i < organs.size(); i++){
        simSetObjectParent(forceSensor[i], body, 1);
        simSetObjectParent(motorOrgan[i], forceSensor[i], 1);
    }
    simSetObjectParent(forceSensor3, body, 1);
	simSetObjectParent(brainOrgan, forceSensor3, 1);

	int objectsNumber;
	int *objectHandles;
	objectHandles = simGetObjectsInTree(body, sim_handle_all, 0, &objectsNumber);
    std::cout << "Objects Number: " << objectsNumber << std::endl;
    std::cout << "Object 0: " << objectHandles[0] << std::endl;
	// Create collection
	int collection_handle = simCreateCollection("robotShape", 1); // This has to be before simAddObjectToCollection
	simAddObjectToCollection(collection_handle, body, sim_handle_single, 0);

	return simGetObjectHandle("robotShape");
}

void Tissue_DirectBarsVREP::create(){
	std::cout << "State: CREATE!" << std::endl;
	mutate();
	mainHandle = createRobot();
	bool testResult;
    testResult = viabilityTest(mainHandle); // True: passed, False: Failed
    std::cout << "Viability: " << testResult << std::endl;
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

void Tissue_DirectBarsVREP::saveGenome(int indNum, float fitness){
	std::cout << "State: SAVE GENOME!" << std::endl;
	std::cout << "saving direct genome " << std::endl << "-------------------------------- " << std::endl;
	//	int evolutionType = 0; // regular evolution, will be changed in the future.
	ofstream genomeFile;
	ostringstream genomeFileName;
    //genomeFileName << "files/morphologies0" << "/genome" << indNum << ".csv";
	genomeFileName << settings->repository + "/morphologies0" << "/genome" << indNum << ".csv";
	// genomeFileName << settings->repository + "/morphologies" << sceneNum << "/genome" << indNum << ".csv"; // Scenenum = -1
	//	genomeFileName << indNum << ".csv";
	genomeFile.open(genomeFileName.str());
	genomeFile << "#Individual:" << indNum << endl;
	genomeFile << "#Fitness:," << fitness << endl;

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
    // Generate phenotype file
    ofstream phenotypeFile;
    ostringstream phenotypeFileName;
    phenotypeFileName << settings->repository + "/morphologies0" << "/phenotype" << indNum << ".csv";
    phenotypeFile.open(phenotypeFileName.str());
    phenotypeFile << 0.0 << ',' << 0.0 << ',' << 0.0 << ',' << 0.0 << ',' << 0.0 << ',' << 0.0 << std::endl;
    for(int i = 0; i < organs.size(); i++){
        for (int j = 0; j < organs[i].coordinates.size(); j++) {
            phenotypeFile << organs[i].coordinates[j] << ',';
        }
        for (int j = 0; j < organs[i].orientations.size(); j++) {
            phenotypeFile << organs[i].orientations[j] << ',';
        }
        phenotypeFile << std::endl;
    }
    // Close tissue file
    phenotypeFile.close();
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
    int genomeCounter = 13; // Ignore brain
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
    organsNumber = 4;
	organs.resize(organsNumber);
	// Initialize coordinates and orientations
    for (int i = 0; i < organs.size(); i++) {
        organs[i].coordinates.resize(3);
        organs[i].orientations.resize(3);
    }
}
// Mutate parameters for Moorphology
void Tissue_DirectBarsVREP::mutateMorphology(float mutationRate) {
    std::cout << "Mutate morphology!" << std::endl;
    // Mutate number of organs
//    if (settings->morphMutRate < randomNum->randFloat(0, 1)) { // Decrease number of organs
//        if(organsNumber > 1) organsNumber--;
//    }
//    if (settings->morphMutRate < randomNum->randFloat(0, 1)) { // Increment number of organs;
//        if(organsNumber < 10) organsNumber++;
//    }
	for (int i = 0; i < organs.size(); i++) { // Mutate organs
        for (int j = 0; j < organs[i].coordinates.size(); j++) { // Mutate coordinates
            if (settings->morphMutRate < randomNum->randFloat(0, 1)) {
                if(j!=2){ // Make sure to generate coordinates above the ground
                    organs[i].coordinates[j] = randomNum->randFloat(-25.0, 25.0); // 3D printer build volumen
                }
                else{
                    organs[i].coordinates[j] = randomNum->randFloat(0.0, 50.0); // 3D printer build volumen
                }
            }
        }
        for (int j = 0; j < organs[i].orientations.size(); j++) { // Mutate orientations
            if (settings->morphMutRate < randomNum->randFloat(0, 1)) {
                organs[i].orientations[j] = randomNum->randFloat(0.0, 6.28319);
            }
        }
	}
}

bool Tissue_DirectBarsVREP::viabilityTest(int robotHandle){
    simAddObjectToSelection(sim_handle_tree, robotHandle);
    int selectionSize = simGetObjectSelectionSize();
    int tempObjectHandles[1024]; // temporarily stores the object Handles as they cannot be inserted directly in a vector for some reason
    vector<int> jointHandles;
    vector<int> shapesHandles;
    //s_objectAmount = selectionSize;
    simGetObjectSelection(tempObjectHandles);
    int objectCounter = 0;

    for (size_t i = 0; i < selectionSize; i++)
    {
        if (simGetObjectType(tempObjectHandles[i]) == sim_object_joint_type) {
            jointHandles.push_back(tempObjectHandles[i]);
        }
        if (simGetObjectType(tempObjectHandles[i]) == sim_object_shape_type) {
            shapesHandles.push_back(tempObjectHandles[i]);
            std::cout << simGetObjectName(tempObjectHandles[i]) << std::endl;
        }
    }

    bool testResult = true;
    // Test volume build size
    float position[3];
    float euclDisOrigin; // Distance between origin and organ
    for(int i = 0; i < jointHandles.size(); i++){
        // Get position of each motor
        simGetObjectPosition(jointHandles[i],-1,position);
        //std::cout << position[0] << "," << position[1] << "," << position[2] << std::endl;
        euclDisOrigin = sqrt(pow(position[0],2)+pow(position[1],2)+pow(position[2],2));
        // std::cout << "Distance from origin: " << euclDisOrigin << std::endl;
        if(euclDisOrigin > 0.25){
           std::cout << simGetObjectName(jointHandles[i]) << " outside of the build volume" << std::endl;
           testResult = false;
        }
    }
    for(int i = 0; i < shapesHandles.size(); i++){
        if(simCheckCollision(shapesHandles[1],shapesHandles[i])){
            std::cout << simGetObjectName(shapesHandles[1]) << " and " << simGetObjectName(shapesHandles[i]) << " are colliding. " << std::endl;
            testResult = false;
        }

    }
    // Test colliding organs
//    vector<int> shapesHandles;
//    for (size_t i = 0; i < selectionSize; i++)    {
//        if (simGetObjectType(tempObjectHandles[i]) == sim_object_shape_type) {
//            shapesHandles.push_back(tempObjectHandles[i]);
//            std::cout << simGetObjectName(tempObjectHandles[i]) << std::endl;
//        }
//    }

    simRemoveObjectFromSelection(sim_handle_all, -1);
    return testResult;
}