#include "Tissue_DirectBarsVREP.h"

Tissue_DirectBarsVREP::Tissue_DirectBarsVREP()
{
	viability = shared_ptr<Viability>(new Viability);
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
//	if (control) {
//		vector<float> input; // get sensor parameters, not set in this case
//		vector<float> output = control->update(input);
//		//		cout << output[0] << endl;
//		for (int i = 0; i < output.size(); i++) {
//			simSetJointTargetVelocity(outputHandles[i], output[i] * 1000);// output[i]);
//			//simSetJointTargetPosition(outputHandles[i], output[i]);
//		}
//	}
    // If robot fails viability test stop simulation
    // TODO change fitness to zero
    //if(viabilityResult == false){
    //    std::cout << "Robot failed viability test. Stop simulation" << std::endl;
    //    simStopSimulation();
    //}
}

int Tissue_DirectBarsVREP::getMainHandle()
{
    return mainHandle;
}

int Tissue_DirectBarsVREP::createMorphology() {

    std::cout << "STATE: Creating morphology!" << std::endl;
    int robotHandle = -1;
    // Before robot generation checks
    bool viabilityResult = false;
    viabilityResult = viability->checkOrgansType(organs);
    if(viabilityResult == true){
        // Importing motor organs
        vector<int> organHandle(organsNumber,-1);
        int skeletonHandle;
		vector<int> forceSensor(organsNumber,-1);
        vector<int> componentHandles;

        // Create skeleton
        skeletonHandle = createSkeleton();
        componentHandles.push_back(skeletonHandle);
        //  Create components and set parents
        for(int i = 0; i < organsNumber; i++){
            organs[i].createOrgan(organs[i].organType, organs[i].coordinates, organs[i].orientations, &organHandle[i], &forceSensor[i]);
            componentHandles.push_back(organHandle[i]);
            simSetObjectParent(forceSensor[i], skeletonHandle, 1);
            // During robot generation checks
            viabilityResult = viability->printVolume(organs[i].coordinates);
            if(viabilityResult == false) break;

            viabilityResult = viability->collisionDetector(componentHandles, organHandle[i]);
            if(viabilityResult == false) break;

            int gripperHandle;
            gripperHandle = viability->createTemporalGripper(organs[i]);
            //viabilityResult = viability.collisionDetector(componentHandles, gripperHandle);
            simRemoveModel(gripperHandle);
            //if(viabilityResult == false) break;

        }
        robotHandle = simCreateCollection("robot", 1); // This has to be before simAddObjectToCollection
        simAddObjectToCollection(robotHandle, skeletonHandle, sim_handle_single, 0);
    }
    return robotHandle;
}

void Tissue_DirectBarsVREP::create(){
    std::cout << "State: CREATE!" << std::endl;
    mutate();
    mainHandle = createMorphology();
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
    genomeFile << organsNumber << ",\n";
    for (int i = 0; i < organsNumber; i++) {
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
    for(int i = 0; i < organsNumber; i++){
        for (int j = 0; j < organs[i].coordinates.size(); j++) {
            phenotypeFile << organs[i].coordinates[j] << ',';
        }
        for (int j = 0; j < organs[i].orientations.size(); j++) {
            phenotypeFile << organs[i].orientations[j]*180.0/3.1516 << ',';
        }
        phenotypeFile << std::endl;
    }
    // Close tissue file
    phenotypeFile.close();

}
//bool Tissue_DirectBarsVREP::loadGenome(std::istream &genomeInput, int individualNumber){
bool Tissue_DirectBarsVREP::loadGenome(int individualNumber, int sceneNum) {
    std::cout << "State: LOAD GENOME!" << std::endl;
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
    // Load values
    int genomeCounter = 1; //
    organsNumber = stof(morphologyValues[genomeCounter]);
    genomeCounter += 2;
    for (int i = 0; i < organsNumber; i++) {
        for (int j = 0; j < organs[i].coordinates.size(); j++) {
            organs[i].coordinates[j] = stof(morphologyValues[genomeCounter]);
            genomeCounter += 2;
        }
        for (int j = 0; j < organs[i].orientations.size(); j++) {
            organs[i].orientations[j] = stof(morphologyValues[genomeCounter])*3.1415/180.0;
            genomeCounter += 2;
        }
    }

    return true; // TODO: Change this to only return true when genome is correctly loaded
}
// Initialize variables for morphology
// Reserve some space
void Tissue_DirectBarsVREP::initMorphology() {

    std::cout << "Init Morph!" << std::endl;
    organsNumber = 5; //TODO move from here!
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
    organsNumber = 5;
    // Brain organ
    organs[0].organType = 0; // Motor organ type
    for (int j = 0; j < organs[0].coordinates.size(); j++) { // Mutate coordinates
        organs[0].coordinates[0] = 0.0; // 3D printer build volumen
    }
    organs[0].coordinates[2] = 0.0;
    for (int j = 0; j < organs[0].orientations.size(); j++) { // Mutate orientations
        organs[0].orientations[j] = 0.0;
    }
    organs[0].orientations[1] = 1.57080; //TODO change orientation in model!
    // Mutate non-organs
    for (int i = 1; i < organsNumber; i++) { // Mutate organs
        organs[i].organType = 1; // Motor organ type
        for (int j = 0; j < organs[i].coordinates.size(); j++) { // Mutate coordinates
            if (settings->morphMutRate < randomNum->randFloat(0, 1)) {
                if(j!=2){ // Make sure to generate coordinates above the ground
                    organs[i].coordinates[j] = randomNum->randFloat(-10.0, 10.0); // 3D printer build volumen
                }
                else{
                    organs[i].coordinates[j] = randomNum->randFloat(0.0, 10.0);
                }
            }
        }
        for (int j = 0; j < organs[i].orientations.size(); j++) { // Mutate orientations
            if (settings->morphMutRate < randomNum->randFloat(0, 1)){
                organs[i].orientations[j] = randomNum->randFloat(0.0, 6.28319);
            }
            // TODO Just for testing!!!
            organs[i].orientations[j] = 0.0;
        }
        organs[i].orientations[0] = 3.1415;
        //organs[i].orientations[1] = 1.0;
        //organs[i].orientations[2] = 1.5708;
    }
}

// Create body
int Tissue_DirectBarsVREP::createSkeleton() {
    std::vector<int> primitiveHandles;
    int skeletonHandle;
    int temp_primitive_handle;
    float columnWidth = 0.015;
    float columnHeight = 0.010; // Wheel not touching floor decrease height
    float magnitude;
    float angle;
    float primitiveSize[3];
    float primitivePosition[3];
    float primitiveOrientation[3];
    float primitiveColour[3];
    //TODO remove this variable from here
    float brainPos[] = { 0.0, 0.0, 0.0 };

    for(int i = 1; i < organsNumber; i++){
        // Horizontal bar
        magnitude = sqrt(pow(organs[i].coordinates[0] / 100 - brainPos[0], 2) + pow(organs[i].coordinates[1] / 100 - brainPos[1], 2)) + columnWidth;
        angle = atan2(organs[i].coordinates[1] / 100 - brainPos[1], organs[i].coordinates[0] / 100 - brainPos[0]);

        primitiveSize[0] = magnitude;
        primitiveSize[1] = columnWidth;
        primitiveSize[2] = columnHeight;
        temp_primitive_handle = simCreatePureShape(0, 8, primitiveSize, 1, NULL);
        primitiveHandles.push_back(temp_primitive_handle);

        primitivePosition[0] = brainPos[0] + (organs[i].coordinates[0] / 100 - brainPos[0]) / 2;
        primitivePosition[1] = brainPos[1] + (organs[i].coordinates[1] / 100 - brainPos[1]) / 2;
        primitivePosition[2] = 0;
        simSetObjectPosition(temp_primitive_handle, -1, primitivePosition);

        primitiveOrientation[0] = 0;
        primitiveOrientation[1] = 0;
        primitiveOrientation[2] = angle;
        simSetObjectOrientation(temp_primitive_handle, -1, primitiveOrientation);

        primitiveColour[0] = 0;
        primitiveColour[1] = 1;
        primitiveColour[2] = 0;
        simSetShapeColor(temp_primitive_handle, NULL, sim_colorcomponent_ambient_diffuse, primitiveColour);

        // Vertical bar
        primitiveSize[0] = columnWidth;
        primitiveSize[1] = columnHeight;
        primitiveSize[2] = abs(organs[i].coordinates[2] / 100 - brainPos[2]);
        temp_primitive_handle = simCreatePureShape(2, 8, primitiveSize, 1, NULL);
        primitiveHandles.push_back(temp_primitive_handle);

        primitivePosition[0] = organs[i].coordinates[0] / 100;
        primitivePosition[1] = organs[i].coordinates[1] / 100;
        primitivePosition[2] = brainPos[2] + (organs[i].coordinates[2] / 100 - brainPos[2]) / 2;
        simSetObjectPosition(temp_primitive_handle, -1, primitivePosition);

        primitiveOrientation[0] = 0;
        primitiveOrientation[1] = 0;
        primitiveOrientation[2] = angle;
        simSetObjectOrientation(temp_primitive_handle, -1, primitiveOrientation);

        primitiveColour[0] = 0;
        primitiveColour[1] = 1;
        primitiveColour[2] = 0;
        simSetShapeColor(temp_primitive_handle, NULL, sim_colorcomponent_ambient_diffuse, primitiveColour);

        // Chamfer
        primitiveSize[0] = columnWidth;
        primitiveSize[1] = columnWidth;
        primitiveSize[2] = columnWidth;
        temp_primitive_handle = simCreatePureShape(1, 8, primitiveSize, 1, NULL);
        primitiveHandles.push_back(temp_primitive_handle);

        primitivePosition[0] = organs[i].coordinates[0] / 100 - cos(angle) * 0.005;
        primitivePosition[1] = organs[i].coordinates[1] / 100 - sin(angle) * 0.005;
        primitivePosition[2] = columnWidth/2.0;
        simSetObjectPosition(temp_primitive_handle, -1, primitivePosition);

        primitiveOrientation[0] = 0.0;
        primitiveOrientation[1] = 0.0; //0.785398
        primitiveOrientation[2] = angle;
        simSetObjectOrientation(temp_primitive_handle, -1, primitiveOrientation);

        primitiveColour[0] = 0;
        primitiveColour[1] = 1;
        primitiveColour[2] = 0;
        simSetShapeColor(temp_primitive_handle, NULL, sim_colorcomponent_ambient_diffuse, primitiveColour);
    }
    int* array = primitiveHandles.data();
    skeletonHandle = simGroupShapes(array, primitiveHandles.size());
    simSetObjectName(skeletonHandle, "robotShape");
    return skeletonHandle;
}

