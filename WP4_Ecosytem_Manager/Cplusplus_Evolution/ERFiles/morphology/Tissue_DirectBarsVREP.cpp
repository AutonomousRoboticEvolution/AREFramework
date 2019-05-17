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
    // Create skeleton - direct bars
    Skeleton skeleton;
    skeleton.organs = organs;
    skeleton.skeletonType = BARS;
    viabilityResult = viability.checkOrgansType(organs);
    skeleton.createSkeleton();
    if(viabilityResult == true){
        vector<int> componentHandles;
        componentHandles.push_back(skeleton.skeletonHandle);
        //  Create components and set parents
        for(int i = 0; i < organsNumber; i++){
            // Create organ
            organs[i].createOrgan();
            componentHandles.push_back(organs[i].organHandle);
            simSetObjectParent(organs[i].forceSensorHandle, skeleton.skeletonHandle, 1);
            // During robot generation checks
            viabilityResult = viability.printVolume(organs[i].coordinates);
            if(viabilityResult == false) break;

            viabilityResult = viability.collisionDetector(componentHandles, organs[i].organHandle);
            if(viabilityResult == false) break;

            int gripperHandle;
            gripperHandle = viability.createTemporalGripper(organs[i]);
            viabilityResult = viability.collisionDetector(componentHandles, gripperHandle);
            simRemoveModel(gripperHandle);
            if(viabilityResult == false)
                break;
        }
        robotHandle = simCreateCollection("robot", 1); // This has to be before simAddObjectToCollection
        simAddObjectToCollection(robotHandle, skeleton.skeletonHandle, sim_handle_single, 0);
    }
    viabilityResult = viability.obstructedSensor(robotHandle);
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
    organs[0].organType = BRAINORGAN; // Motor organ type
    for (int i = 0; i < organs[0].coordinates.size(); i++) { // Mutate coordinates
        organs[0].coordinates[i] = 0.0; // 3D printer build volumen
    }
    organs[0].coordinates[2] = 0.025;
    for (int i = 0; i < organs[0].orientations.size(); i++) { // Mutate orientations
        organs[0].orientations[i] = 0.0;
    }
    // Mutate non-organs
    for (int i = 1; i < organsNumber; i++) { // Mutate organs
        organs[i].organType = SENSORORGAN; // Motor organ type
        for (int j = 0; j < organs[i].coordinates.size(); j++) { // Mutate coordinates
            if (settings->morphMutRate < randomNum->randFloat(0, 1)) {
                if(j!=2){ // Make sure to generate coordinates above the ground
                    organs[i].coordinates[j] = randomNum->randFloat(-0.1, 0.1); // 3D printer build volume
                }
                else{
                    organs[i].coordinates[j] = randomNum->randFloat(0.0, 0.1);
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
    }
}

