#include "CustomMorphologyVREP.h"



CustomMorphologyVREP::CustomMorphologyVREP()
{
}


CustomMorphologyVREP::~CustomMorphologyVREP()
{
}

vector<int> CustomMorphologyVREP::getObjectHandles(int parentHandle)
{
	return vector<int>();
}

vector<int> CustomMorphologyVREP::getJointHandles(int parentHandle)
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

void CustomMorphologyVREP::update() {
	if (control) {
		vector<float> input; // get sensor parameters, not set in this case
		vector<float> output = control->update(input);
		cout << output[0] << endl;
		for (int i = 0; i < output.size(); i++) {
			simSetJointTargetVelocity(outputHandles[i], output[i] *1000);// output[i]);
			//simSetJointTargetPosition(outputHandles[i], output[i]);
		}
	}
}

int CustomMorphologyVREP::getMainHandle()
{
	return mainHandle;
}

struct sRobotMorphology
{
	int handle;
	int bodyHandle;
	int motorHandles[2];
	int sensorHandles[2];
	int wheelHandles[3];
	int organHandles[5];
	std::vector<int> cubeHandles;
};


int CustomMorphologyVREP::createWei() {
	//void parseCSV();
	std::vector<std::vector<float> > parsedCsv;

	std::vector<sRobotMorphology> allRobotMorphologys;
	int nextRobotMorphologyHandle = 0;
	//void parseMorphology();

	std::ifstream  data("Coordinates.csv");
	std::string line;
	while (std::getline(data, line))
	{
		std::stringstream lineStream(line);
		std::string cell;
		std::vector<float> parsedRow;
		while (std::getline(lineStream, cell, ','))
		{
			float temp_data;
			std::stringstream convertor(cell);
			convertor >> temp_data;
			parsedRow.push_back(temp_data);
			//printf("%.4f ", temp_data);
		}
		parsedCsv.push_back(parsedRow);
		//printf("\n");
	}
	//v.size() as it gives the total number of rows and v[i].size() gives you the total number of colums in ith row.
	printf("rows: %d, cols: %d\n", parsedCsv.size(), parsedCsv[0].size());

	int handle = -1;
	sRobotMorphology RobotMorphology;
	handle = nextRobotMorphologyHandle++;
	RobotMorphology.handle = handle;
	// Draw cubes
	float organ1_size[] = { 0.018,0.018,0.032 };
	RobotMorphology.cubeHandles.push_back(simCreatePureShape(0, 8, organ1_size, 1, NULL));
	float organ2_size[] = { 0.018,0.018,0.032 };
	RobotMorphology.cubeHandles.push_back(simCreatePureShape(0, 8, organ2_size, 1, NULL));
	float organ3_size[] = { 0.01,0.02,0.015 };
	RobotMorphology.cubeHandles.push_back(simCreatePureShape(0, 8, organ3_size, 1, NULL));
	float organ4_size[] = { 0.01,0.02,0.015 };
	RobotMorphology.cubeHandles.push_back(simCreatePureShape(0, 8, organ4_size, 1, NULL));
	float organ5_size[] = { 0.03,0.015,0.058 };
	RobotMorphology.cubeHandles.push_back(simCreatePureShape(0, 8, organ5_size, 1, NULL));
	// Voxels
	std::vector <int> cubeHandles;
	for (int i = 5; i < parsedCsv.size(); i++) {
		// 48 Static object
		// 00 Dynamic not respondable
		// 08 Dynamic respondable
		int temp_voxel_handle;
		float voxel_size[] = { 0.01,0.01,0.01 };
		temp_voxel_handle = simCreatePureShape(0, 8, voxel_size, 1, NULL);
		RobotMorphology.cubeHandles.push_back(temp_voxel_handle);
		float voxel_pos[] = { parsedCsv[i][0] / 100,parsedCsv[i][1] / 100,parsedCsv[i][2] / 100 };
		simSetObjectPosition(temp_voxel_handle, -1, voxel_pos);
		float voxel_color[] = { 0,1,0 };
		simSetShapeColor(temp_voxel_handle, NULL, sim_colorcomponent_ambient_diffuse, voxel_color);
	}
	//Organs properties
	float organ_color[] = { 0,0,1 };
	simSetShapeColor(RobotMorphology.cubeHandles[0], NULL, sim_colorcomponent_ambient_diffuse, organ_color);
	simSetShapeColor(RobotMorphology.cubeHandles[1], NULL, sim_colorcomponent_ambient_diffuse, organ_color);
	simSetObjectName(RobotMorphology.cubeHandles[0], "motorOrgan1");
	simSetObjectName(RobotMorphology.cubeHandles[1], "motorOrgan2");
	simSetObjectName(RobotMorphology.cubeHandles[2], "sensorOrgan1");
	simSetObjectName(RobotMorphology.cubeHandles[3], "sensorOrgan2");
	simSetObjectName(RobotMorphology.cubeHandles[4], "batteryOrgan");

	float organ1_pos[] = { parsedCsv[0][0] / 100.0f, parsedCsv[0][1] / 100.0f + 0.025f, parsedCsv[0][2] / 100.0f };
	simSetObjectPosition(RobotMorphology.cubeHandles[0], -1, organ1_pos);
	float organ2_pos[] = { parsedCsv[1][0] / 100.0f, parsedCsv[1][1] / 100.0f - 0.025f, parsedCsv[1][2] / 100.0f };
	simSetObjectPosition(RobotMorphology.cubeHandles[1], -1, organ2_pos);
	float organ3_pos[] = { parsedCsv[2][0] / 100.0f, parsedCsv[2][1] / 100.0f, parsedCsv[2][2] / 100.0f };
	simSetObjectPosition(RobotMorphology.cubeHandles[2], -1, organ3_pos);
	float organ4_pos[] = { parsedCsv[3][0] / 100.0f, parsedCsv[3][1] / 100.0f, parsedCsv[3][2] / 100.0f };
	simSetObjectPosition(RobotMorphology.cubeHandles[3], -1, organ4_pos);
	float organ5_pos[] = { parsedCsv[4][0] / 100.0f, parsedCsv[4][1] / 100.0f, parsedCsv[4][2] / 100.0f };
	simSetObjectPosition(RobotMorphology.cubeHandles[4], -1, organ5_pos);

	float organ_angle[] = { 1.57080, 0, 0 };
	simSetObjectOrientation(RobotMorphology.cubeHandles[0], -1, organ_angle);
	simSetObjectOrientation(RobotMorphology.cubeHandles[1], -1, organ_angle);
	simSetObjectOrientation(RobotMorphology.cubeHandles[2], -1, organ_angle);
	simSetObjectOrientation(RobotMorphology.cubeHandles[3], -1, organ_angle);
	simSetObjectOrientation(RobotMorphology.cubeHandles[4], -1, organ_angle);
	int* a = RobotMorphology.cubeHandles.data();
	int body = simGroupShapes(a, RobotMorphology.cubeHandles.size());
	printf("size: %d\n", RobotMorphology.cubeHandles.size());
	simSetObjectName(body, "robot_shape");

	// Motors
	float motor_size[] = { 0.005, 0.01 };
	std::string motorName = "motor";
	for (int i = 0; i < 2; i++) {
		RobotMorphology.motorHandles[i] = simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, motor_size, NULL, NULL);
		outputHandles.push_back(RobotMorphology.motorHandles[i]);
		simSetObjectInt32Parameter(RobotMorphology.motorHandles[i], 2000, 1);
		simSetObjectName(RobotMorphology.motorHandles[i], (motorName + std::to_string(i)).c_str());
		float motor_pos[] = { parsedCsv[i][0] / 100, parsedCsv[i][1] / 100, parsedCsv[i][2] / 100 };
		simSetObjectPosition(RobotMorphology.motorHandles[i], -1, motor_pos);
		float motor_angle[] = { 1.57080, 0, 0 };
		simSetObjectOrientation(RobotMorphology.motorHandles[i], -1, motor_angle);
		simSetObjectParent(RobotMorphology.motorHandles[i], body, 1);
		//simSetJointTargetVelocity(RobotMorphology.motorHandles[i], 0);  //velocity
	}

	// Wheels
	float wheel_size[] = { 0.03,0.03,0.005 };
	std::string wheelName = "wheel";
	for (int i = 0; i < 2; i++) {
		RobotMorphology.wheelHandles[i] = simCreatePureShape(2, 8, wheel_size, 1, NULL);
		simSetObjectName(RobotMorphology.wheelHandles[i], (wheelName + std::to_string(i)).c_str());
		float wheel_pos[] = { parsedCsv[i][0] / 100, parsedCsv[i][1] / 100, parsedCsv[i][2] / 100 };
		simSetObjectPosition(RobotMorphology.wheelHandles[i], -1, wheel_pos);
		float wheel_angle[] = { 1.57080, 0, 0 };
		simSetObjectOrientation(RobotMorphology.wheelHandles[i], -1, wheel_angle);
		simSetObjectParent(RobotMorphology.wheelHandles[i], RobotMorphology.motorHandles[i], 1);    // Define parents and children
	}

	// Sensors
	std::string sensorName = "proxSensor";
	for (int i = 0; i < 2; i++) {
		int prox_para1[] = { 16,16,2,2,0,0,0,0 };
		float prox_para2[] = { 0.01,0.15,0,0,0,0,0.01,0,0,0.35,0,0.01,0.005,0.0,0.0 };
		RobotMorphology.sensorHandles[i] = simCreateProximitySensor(sim_proximitysensor_cone_subtype, sim_objectspecialproperty_detectable_infrared, 288, prox_para1, prox_para2, NULL);
		simSetObjectName(RobotMorphology.sensorHandles[i], (sensorName + std::to_string(i)).c_str());
		float sensor_pos[] = { parsedCsv[i + 2][0] / 100, parsedCsv[i + 2][1] / 100, parsedCsv[i + 2][2] / 100 };
		printf("sensor pos: %.4f,%.4f,%.4f\n", sensor_pos[0], sensor_pos[1], sensor_pos[2]);
		simSetObjectPosition(RobotMorphology.sensorHandles[i], -1, sensor_pos);
		float sensor_angle[] = { 0, -1.5708, 0 };
		simSetObjectOrientation(RobotMorphology.sensorHandles[i], -1, sensor_angle);
		simSetObjectParent(RobotMorphology.sensorHandles[i], body, 1);    // Define parents and children
	}

	// Create collection
	//cout << handle << endl;
	int collection_handle = simCreateCollection("robot_shape", 1); // This has to be before simAddObjectToCollection
	simAddObjectToCollection(collection_handle, body, sim_handle_single, 0);
	// cout << handle << endl;
	return simGetObjectHandle("robot_shape");
}

void CustomMorphologyVREP::create()
{

	//simLoadModel("C:\\Program Files\\V-REP3\\V-REP_PRO_EDU\\models\\NAOwithoutscript.ttm");
	//mainHandle = simGetObjectHandle("NAO");
	mainHandle = createWei();
//	simLoadModel("models/robots/mobile/Snake.ttm");
//	mainHandle = simGetObjectHandle("snake");
//	simLoadModel("models/robots/mobile/Marty the Robot.ttm");
//	mainHandle = simGetObjectHandle("Marty");
	//vector<int> jointHandles = getJointHandles(mainHandle);
}

void CustomMorphologyVREP::init() {
	create();
	unique_ptr<ControlFactory> controlFactory(new ControlFactory);
	control = controlFactory->createNewControlGenome(0, randomNum, settings); // ann
	controlFactory.reset();
	control->init(2, 50, 2);
	control->mutate(0.5);
}