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
    int motorOrgan[organsNumber];
    int brainOrgan = simLoadModel("models/brainOrgan.ttm");
//    int brainConnector = simLoadModel("models/brainConnector.ttm");
    int organClips[organsNumber];

    // Create force sensors to place organs
    int pamsArg1[] = { 0,0,0,0,0 };
    float pamsArg2[] = { 0,0,0,0,0 };
    int forceSensor[organsNumber];
    int forceSensor3 = simCreateForceSensor(0, pamsArg1, pamsArg2, NULL); // Brain
    int clipsSensor[organsNumber];
//    int forceBC = simCreateForceSensor(0, pamsArg1, pamsArg2, NULL); // Brain connector
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
    float columnHeight = 0.010; // Wheel not touching floor decrease height
    float magnitude;
    float angle;
    float voxel_size[3];
    float voxel_pos[3];
    float voxel_ori[3];
    float voxel_color[3];

    for(int i = 0; i < organsNumber; i++){
        // motorOrgan[i] = simLoadModel("models/motorOrgan2_bigWheel.ttm");
//        motorOrgan[i] = simLoadModel("models/motorOrgan3_massFixed.ttm");
        if(i % 2 == 0 ){
            motorOrgan[i] = simLoadModel("models/motorOrgan3_massFixed.ttm");
        }
        else{
            motorOrgan[i] = simLoadModel("models/sensorOrgan.ttm");
        }
        forceSensor[i] = simCreateForceSensor(0, pamsArg1, pamsArg2, NULL);

        // Connectors
        organClips[i] = simLoadModel("models/clipConnector.ttm");
        clipsSensor[i] = simCreateForceSensor(0, pamsArg1, pamsArg2, NULL);

        // Horizontal bar
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

        // Vertical bar
        voxel_size[0] = columnWidth;
        voxel_size[1] = columnHeight;
        voxel_size[2] = abs(organs[i].coordinates[2] / 100 - brainPos[2]);
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

        // Chamfer
        voxel_size[0] = columnWidth;
        voxel_size[1] = columnWidth;
        voxel_size[2] = columnWidth;
        temp_voxel_handle = simCreatePureShape(1, 8, voxel_size, 1, NULL);
        RobotMorphology.cubeHandles.push_back(temp_voxel_handle);

        voxel_pos[0] = organs[i].coordinates[0] / 100 - cos(angle) * 0.005;
        voxel_pos[1] = organs[i].coordinates[1] / 100 - sin(angle) * 0.005;
        voxel_pos[2] = columnWidth/2.0;
        simSetObjectPosition(temp_voxel_handle, -1, voxel_pos);

        voxel_ori[0] = 0.0;
        voxel_ori[1] = 0.0; //0.785398
        voxel_ori[2] = angle;
        simSetObjectOrientation(temp_voxel_handle, -1, voxel_ori);

        voxel_color[0] = 0;
        voxel_color[1] = 1;
        voxel_color[2] = 0;
        simSetShapeColor(temp_voxel_handle, NULL, sim_colorcomponent_ambient_diffuse, voxel_color);

        // Place organs
        // TODO: Try to remove these temporal variables.
        float tempPos[] = {organs[i].coordinates[0]/100, organs[i].coordinates[1]/100, organs[i].coordinates[2]/100 + 0.05}; // 0.0225 Wheels barely touching floor but more room to rotate;
        simSetObjectPosition(motorOrgan[i], -1, tempPos);
        simSetObjectPosition(forceSensor[i], -1, tempPos);
        float tempOri[] = {organs[i].orientations[0],organs[i].orientations[1],organs[i].orientations[2]};
        //float tempOri[] = {0.0,0.0,0.0};
        simSetObjectOrientation(motorOrgan[i], -1, tempOri);
        simSetObjectOrientation(forceSensor[i], -1, tempOri);

        //float tempPosClips[] = {organs[i].coordinates[0]/100, organs[i].coordinates[1]/100, organs[i].coordinates[2]/100}; // 0.0225 Wheels barely touching floor but more room to rotate;
        float tempPosClips[] = {organs[i].coordinates[0]/100, organs[i].coordinates[1]/100, organs[i].coordinates[2]/100 + 0.003}; // 0.0225 Wheels barely touching floor but more room to rotate;
        simSetObjectPosition(organClips[i], -1, tempPosClips);
        simSetObjectPosition(clipsSensor[i], -1, tempPosClips);
        //float tempOriClips[] = {organs[i].orientations[0],organs[i].orientations[1],organs[i].orientations[2]};
        float tempOriClips[] = {0.0, 0.0, 0.0};
        simSetObjectOrientation(organClips[i], -1, tempOriClips);
        simSetObjectOrientation(clipsSensor[i], -1, tempOriClips);
    }

    // Set brain position
    brainPos[2] = 0.03;
    simSetObjectPosition(brainOrgan, -1, brainPos);
    simSetObjectPosition(forceSensor3, -1, brainPos);

    // Set brain orientation
    simSetObjectOrientation(brainOrgan, -1, brainOri);
    simSetObjectOrientation(forceSensor3, -1, brainOri);

    // Set brain connector
    float tempBrainConnectorPos[] = {0.0, 0.0, 0.0};
    float tempBrainConnectorOri[] = {0.0, 0.0, 0.0};
//    simSetObjectPosition(brainConnector, -1, tempBrainConnectorPos);
//    simSetObjectPosition(forceBC, -1, tempBrainConnectorPos);
//    simSetObjectOrientation(brainConnector, -1, tempBrainConnectorOri);
//    simSetObjectOrientation(forceBC, -1, tempBrainConnectorOri);

    int* a = RobotMorphology.cubeHandles.data();
    int body = simGroupShapes(a, RobotMorphology.cubeHandles.size());
    simSetObjectName(body, "robotShape");

    //  Set parents
    for(int i = 0; i < organsNumber; i++){
        simSetObjectParent(forceSensor[i], body, 1);
        simSetObjectParent(motorOrgan[i], forceSensor[i], 1);
        simSetObjectParent(clipsSensor[i], body, 1);
        simSetObjectParent(organClips[i], clipsSensor[i], 1);
    }
    simSetObjectParent(forceSensor3, body, 1);
    simSetObjectParent(brainOrgan, forceSensor3, 1);

//    simSetObjectParent(forceBC, body, 1);
//    simSetObjectParent(brainConnector, forceBC, 1);

    int objectsNumber;
    int *objectHandles;
    objectHandles = simGetObjectsInTree(body, sim_handle_all, 0, &objectsNumber);
    std::cout << "Objects Number: " << objectsNumber << std::endl;
    std::cout << "Object 0: " << objectHandles[0] << std::endl;

    // Create collection
    int collection_handle = simCreateCollection("robotShape", 1); // This has to be before simAddObjectToCollection
    simAddObjectToCollection(collection_handle, body, sim_handle_single, 0);

    // Export stl
    simInt shapeCount = 0;
    int skeletonsNumber = 0;
    shapeCount = skeletonsNumber + 1;
    int tempHandles[shapeCount];
    tempHandles[0] = body;
    simFloat** vertices=new simFloat*[shapeCount];
    simInt* verticesSizes=new simInt[shapeCount];
    simInt** indices=new simInt*[shapeCount];
    simInt* indicesSizes=new simInt[shapeCount];
    simInt index=0;
    for(int j = 0; j < shapeCount; j++){
        simFloat* vert;
        simInt vertS;
        simInt* ind;
        simInt indS;
        int getShapeMeshResult = -1;
        getShapeMeshResult = simGetShapeMesh(tempHandles[j],&vert,&vertS,&ind,&indS,NULL);
        std::cout << getShapeMeshResult << std::endl;
        vertices[j]=vert;
        verticesSizes[j]=vertS;
        indices[j]=ind;
        indicesSizes[j]=indS;
        simFloat m[12];
        simGetObjectMatrix(tempHandles[j],-1,m);
        for (simInt i=0;i<vertS/3;i++)
        {
            simFloat v[3]={vert[3*i+0],vert[3*i+1],vert[3*i+2]};
            simTransformVector(m,v);
            vert[3*i+0]=v[0];
            vert[3*i+1]=v[1];
            vert[3*i+2]=v[2];
            //std::cout << vert[3*i+0] << "," << vert[3*i+1] << "," << vert[3*i+2] << std::endl;
        }
    }
    std::cout << "Before export mesh" << std::endl;
    //
    std::cout << shapeCount << std::endl;
    int success;
    success = simExportMesh(3,"models/example.stl",0,1,shapeCount,vertices,
                  verticesSizes,indices,indicesSizes,NULL,NULL);
    std::cout << "After export mesh" << std::endl;
    for (simInt i=0;i<shapeCount;i++)
    {
        simReleaseBuffer((simChar*)vertices[i]);
        simReleaseBuffer((simChar*)indices[i]);
    }
    delete[] vertices;
    delete[] verticesSizes;
    delete[] indices;
    delete[] indicesSizes;


    return simGetObjectHandle("robotShape");
}

void Tissue_DirectBarsVREP::create(){
    std::cout << "State: CREATE!" << std::endl;
    mutate();
    mainHandle = createRobot();
    viabilityResult;
    viabilityResult = viabilityTest(mainHandle); // True: passed, False: Failed
    std::cout << "Viability: " << viabilityResult << std::endl;
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
void Tissue_DirectBarsVREP::initMorphology() {

    std::cout << "Init Morph!" << std::endl;
    //TODO: Remove unnecessary organs
    organs.resize(10);
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
    if (settings->morphMutRate < randomNum->randFloat(0, 1)) { // Decrease number of organs
//        organsNumber = (int)randomNum->randFloat(1, 9);
        organsNumber = 4;
    }
//    if (settings->morphMutRate < randomNum->randFloat(0, 1)) { // Decrease number of organs
//        if(genome->organsNumber > 1) genome->organsNumber--;
//    }
//    if (settings->morphMutRate < randomNum->randFloat(0, 1)) { // Increment number of organs;
//        if(genome->organsNumber < 10) genome->organsNumber++;
//    }
    for (int i = 0; i < organsNumber; i++) { // Mutate organs
        for (int j = 0; j < organs[i].coordinates.size(); j++) { // Mutate coordinates
            if (settings->morphMutRate < randomNum->randFloat(0, 1)) {
                if(j!=2){ // Make sure to generate coordinates above the ground
                    organs[i].coordinates[j] = randomNum->randFloat(-10.0, 10.0); // 3D printer build volumen
                }
                else{
//                  organs[i].coordinates[j] = 0.0; // Motors close to ground
                    organs[i].coordinates[j] = randomNum->randFloat(0.0, 10.0);
                }
            }
        }
//        organs[i].coordinates[0] = 0.0;
//        organs[i].coordinates[1] = 8.0;
        for (int j = 0; j < organs[i].orientations.size(); j++) { // Mutate orientations
            if (settings->morphMutRate < randomNum->randFloat(0, 1)){
                organs[i].orientations[j] = randomNum->randFloat(0.0, 6.28319);
            }
        }
        if(i % 2 == 0 ){
            organs[i].orientations[0] = 1.57080;
//          organs[i].orientations[1] = 0.0;
            organs[i].orientations[2] = -1.57080;
        }
        else{
            organs[i].orientations[0] = 1.57;
//          organs[i].orientations[1] = 0.0;
            organs[i].orientations[2] = 0.0;
        }
//        organs[i].orientations[0] = 1.57080; // Y-axis
////        organs[i].orientations[1] = 0.0; // Z-axis
//        organs[i].orientations[2] = -1.57080; // X-axis along motor axis. No mutation required
    }
}

bool Tissue_DirectBarsVREP::viabilityTest(int robotHandle){
    simAddObjectToSelection(sim_handle_tree, robotHandle);
    int selectionSize = simGetObjectSelectionSize();
    int tempObjectHandles[1024]; // temporarily stores the object Handles as they cannot be inserted directly in a vector for some reason
    vector<int> jointHandles;
    vector<int> shapesHandles;
    simGetObjectSelection(tempObjectHandles);

    for (size_t i = 0; i < selectionSize; i++)
    {
        if (simGetObjectType(tempObjectHandles[i]) == sim_object_joint_type) {
            jointHandles.push_back(tempObjectHandles[i]);
            simSetJointTargetVelocity(jointHandles[i], 10.0);
        }
        if (simGetObjectType(tempObjectHandles[i]) == sim_object_shape_type) {
            shapesHandles.push_back(tempObjectHandles[i]);
            std::cout << simGetObjectName(tempObjectHandles[i]) << std::endl;
        }
    }

    bool testResult = true;
    // Test 1: Organs number
    // The number of organs cannot be greater than 5
    if(organsNumber > 4){
        return false;
    }
    // Test 2: Volume build size
    // Check if an organ is outside of the 3D printing volume
    float position[3];
    float euclDisOrigin; // Distance between origin and organ
    for(int i = 0; i < jointHandles.size(); i++){
        // Get position of each motor
        simGetObjectPosition(jointHandles[i],-1,position);
        //std::cout << position[0] << "," << position[1] << "," << position[2] << std::endl;
        euclDisOrigin = sqrt(pow(position[0],2)+pow(position[1],2)+pow(position[2],2));
        // std::cout << "Distance from origin: " << euclDisOrigin << std::endl;
        if(euclDisOrigin > 0.25){
            //std::cout << simGetObjectName(jointHandles[i]) << " outside of the build volume" << std::endl;
            // testResult = false;
            return false;
        }
    }
    // Test 3: No organs above brain (direct bars exclusive restriction)
    // No organs can be above the brain
    for(int i = 0; i < jointHandles.size(); i++){
        simGetObjectPosition(jointHandles[i],-1,position);
        euclDisOrigin = sqrt(pow(position[0],2)+pow(position[1],2));
        if(euclDisOrigin < 0.036){
            // testResult = false;
            return false;
        }
    }
    // Test 2: Prevent colliding organs
    // Check if any organs are colliding with the tissue
    for(int i = 0; i < shapesHandles.size(); i++){
        if(simCheckCollision(shapesHandles[1],shapesHandles[i])){
            std::cout << simGetObjectName(shapesHandles[1]) << " and " << simGetObjectName(shapesHandles[i]) << " are colliding. " << std::endl;
            //testResult = false;
            return false;
        }
    }

    simRemoveObjectFromSelection(sim_handle_all, -1);
    return testResult;
}
