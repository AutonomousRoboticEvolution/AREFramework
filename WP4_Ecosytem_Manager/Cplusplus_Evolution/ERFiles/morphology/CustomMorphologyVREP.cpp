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
	if (name != "Pioneer_p3dx") {
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
	}
	return jointHandles;
}

void CustomMorphologyVREP::update() {
	if (control) {
		vector<float> input; // get sensor parameters, not set in this case
		if (name == "Pioneer_p3dx") {
			int lightHandle = simGetObjectHandle("Light");
			if (settings->verbose == true) {
				cout << "controlling pioneer" << endl;
				cout << "Sensor handles size = " << sensorHandles.size() << endl;
				cout << "LightHandle = " << lightHandle << endl;
				cout << "Output handles size = " << outputHandles.size() << endl;
			}
			float sunPos[3];
			for (int i = 0; i < sensorHandles.size(); i++) {
				simGetObjectPosition(lightHandle, -1, sunPos);
				float proxSensPos[3];
				simGetObjectPosition(sensorHandles[i], -1, proxSensPos);
				float ax = sunPos[0] - proxSensPos[0];
				float ay = sunPos[1] - proxSensPos[1];
				float az = sunPos[2] - proxSensPos[2];

				float a = sqrt(((ax * ax) + (ay * ay) + (az * az)));
				ax = ax / a;
				ay = ay / a;
				az = az / a;
				if (settings->verbose == true) {
					cout << "magnitude = " << sqrt(((ax * ax) + (ay * ay) + (az * az))) << endl;
				}
				vector<float> normalizedRayVector;

				normalizedRayVector.push_back(ax);
				normalizedRayVector.push_back(ay);
				normalizedRayVector.push_back(az);

				float blue[3] = { 0,0,1 };
				//int rayHandle = simAddDrawingObject(sim_drawing_lines + sim_drawing_cyclic, 1, 0.0, -1, 2, blue, NULL, NULL, blue); // startEndLine );
				//float ab[6] = { proxSensPos[0], proxSensPos[1], proxSensPos[2], proxSensPos[0] +(normalizedRayVector[0] * 10), 
				//	proxSensPos[1] + (normalizedRayVector[1] * 10), proxSensPos[2] + (normalizedRayVector[2] * 10) };
				//simAddDrawingObjectItem(rayHandle, ab);

				float xAx[3] = { 0.0,0.0,0.0 };
				float yAx[3] = { 0.0,0.0,0.0 };
				float zAx[3] = { normalizedRayVector[0],normalizedRayVector[1],normalizedRayVector[2] };
				float matrix[12];
				simGetObjectMatrix(sensorHandles[i], -1, matrix);
				matrix[0] = xAx[0];
				matrix[4] = xAx[1];
				matrix[8] = xAx[2];
				matrix[1] = yAx[0];
				matrix[5] = yAx[1];
				matrix[9] = yAx[2];
				matrix[2] = zAx[0];
				matrix[6] = zAx[1];
				matrix[10] = zAx[2];
				simSetObjectMatrix(sensorHandles[i], -1, matrix);

				int doh[1]; // detected object handle
				simHandleProximitySensor(sensorHandles[i], NULL, doh, NULL);
				float pt[4];
				float vecs[3];
				simHandleProximitySensor(sensorHandles[i], pt, doh, vecs);
				int det = simReadProximitySensor(sensorHandles[i], pt, doh, vecs);
				if (det == 0) { // for measuring if the proximity sensor collided
					// notCollidedProxSensors.push_back(sensorHandles[i]);
					//	cout << "read doh? : " << doh[0] << " but " << det << endl;
					//	cout << "pt = " << pt[0] << ", " << pt[1] << ", " << pt[2] << ", " << pt[3] << endl;
				}
				int sParent = simGetObjectParent(sensorHandles[i]);
				float sOr[12];
				simGetObjectMatrix(sensorHandles[i],sParent, sOr);
				// simGetObjectOrientation(sensorHandles[i], sParent, sOr);
				input.push_back(sOr[10]);
			}
		}
		vector<float> output = control->update(input);
		for (int i = 0; i < output.size(); i++) {
			if (settings->verbose) {
				cout << "output " << i << " = " << output[i] << endl;
			}
			//if (i < outputHandles.size()) {
				//simSetJointTargetVelocity(outputHandles[i], output[i] * 1000);// output[i]);
				//simSetJointTargetPosition(outputHandles[i], output[i]);
				simSetJointTargetVelocity(outputHandles[i], output[i]);
			//}
		}
	}
	else {
		std::cout << "No controller present... " << endl;
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

	//	simLoadModel("C:\\Program Files\\V-REP3\\V-REP_PRO_EDU\\models\\NAOwithoutscript.ttm");
	//	mainHandle = simGetObjectHandle("NAO");
	if (control) {
		if (settings->verbose) {
			cout << "At create Custom morphology, control is present" << endl;
		}
	}
	simLoadModel("C:\\Program Files\\V-REP3\\V-REP_PRO_EDU\\models\\Pioneer_noscript.ttm");
	name = "Pioneer_p3dx";
	if (name == "Pioneer_p3dx") {
		mainHandle = simGetObjectHandle(name.c_str());
		int s1 = simGetObjectHandle("Pioneer_p3dx_ultrasonicSensor4");
		int s2 = simGetObjectHandle("Pioneer_p3dx_ultrasonicSensor6");

		int intpars[8] = { 1,1,1,1,1,1,0,0 };
		float sensorSize = 0.004;
		float floatpars[15] = { 0,10,0.0010,0.001,0.001,0.0001,0.001,0.0001,0.0001,0.0001,0.0001,0.0001,sensorSize,0,0 };

		float sensor1pos[3];
		float sensor2pos[3];
		simGetObjectPosition(s1, -1, sensor1pos);
		simGetObjectPosition(s2, -1, sensor2pos);

		int proxSens1 = simCreateProximitySensor(sim_proximitysensor_cone_subtype, sim_objectspecialproperty_detectable_ultrasonic, 0, intpars, floatpars, NULL);
		int proxSens2 = simCreateProximitySensor(sim_proximitysensor_cone_subtype, sim_objectspecialproperty_detectable_ultrasonic, 0, intpars, floatpars, NULL);

		sensorHandles.push_back(proxSens1);
		sensorHandles.push_back(proxSens2);

		float pos[3] = { 0.0, 0.0, 0.1 };
		simSetObjectPosition(proxSens1, s1, pos);
		simSetObjectPosition(proxSens2, s2, pos);
		simSetObjectParent(proxSens1, s1, true);
		simSetObjectParent(proxSens2, s2, true);
	}
	//	simSetObjectInt32Parameter(proxSens,10, 0);

	// mainHandle = createWei();
//	simLoadModel("models/robots/mobile/Snake.ttm");
//	mainHandle = simGetObjectHandle("snake");
//	simLoadModel("models/robots/mobile/Marty the Robot.ttm");
//	mainHandle = simGetObjectHandle("Marty");
	vector<int> jointHandles = getJointHandles(mainHandle);
	outputHandles = jointHandles;
}


shared_ptr<Morphology> CustomMorphologyVREP::clone() const {
	// shared_ptr<Morphology>()
	shared_ptr<CustomMorphologyVREP> cat = make_unique<CustomMorphologyVREP>(*this);
	//	cat->va = va->clone();
	cat->control = control->clone();
	return cat;
}
