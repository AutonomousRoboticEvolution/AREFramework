#include "R_Module_Rodrigo.h"
#include <cmath>

int R_Module_Rodrigo::init(dynamixel::PacketHandler * packetH1, dynamixel::PacketHandler * packetH2, dynamixel::PortHandler * portH)
{
	return -1;
}

R_Module_Rodrigo::R_Module_Rodrigo()
{

}

shared_ptr<ER_Module> R_Module_Rodrigo::clone() {
	shared_ptr<R_Module_Rodrigo> clonedModule = make_unique<R_Module_Rodrigo>(*this);
	//	clonedModule->control = control->clone();
	return clonedModule;
}

R_Module_Rodrigo::~R_Module_Rodrigo()
{
	
}

int R_Module_Rodrigo::init() {
	return 1;
}
float R_Module_Rodrigo::mutate() {
	return 1.0;
}

vector<float> R_Module_Rodrigo::updateModule(vector<float> input) {
	ER_Module::updateModule(input);
	vector<float> output = input;
	if (output.size() != 1) {
		cout << "ERROR:: output size is not 1, but should be" << endl;
	}
	//	cout << "output[0] = " << output[0] << endl;

	if (output[0] > 1.0) {
		output[0] = 1.0;
	}
	else if (output[0] < -1.0) {
		output[0] = -1.0;
	}
	simSetJointTargetPosition(controlHandles[0], 0.5 * output[0] * M_PI);
	//	simSetJointTargetPosition(controlHandles[0], input[0] * M_PI);

	int movePars[3] = { moduleID, 32,  300 };
	write(settings->portHandler, settings->packetHandler1, movePars[0], movePars[1], 2, movePars[2]);
	int angle = (int)(((((1 + (0.5 * output[0]) * 1.3) * 500) - 500)) + 500);
	if (angle > 820) {
		angle = 820;
	}
	if (angle < 200) {
		angle = 200;
	}
	int pars[3] = { moduleID, 30, (int)angle };
	float jPos[3];
	simGetJointPosition(controlHandles[0], jPos);
	cout << "jointPos = " << jPos[0] << endl;
	cout << "angle ID " << moduleID << " is " << angle << endl;
	cout << "jointPos = " << jPos[0] << endl;

	write(settings->portHandler, settings->packetHandler1, pars[0], pars[1], 2, pars[2]);
	return input; // 
}

void R_Module_Rodrigo::write(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint8_t id, uint16_t addr, uint16_t length, uint32_t value)
{
	uint8_t dxl_error = 0;
	int dxl_comm_result = COMM_TX_FAIL;

	if (length == 1)
	{
		dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, id, addr, (uint8_t)value, &dxl_error);
	}
	else if (length == 2)
	{
		dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, id, addr, (uint16_t)value, &dxl_error);
	}
	else if (length == 4)
	{
		dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, id, addr, (uint32_t)value, &dxl_error);
	}

	if (dxl_comm_result == COMM_SUCCESS)
	{
		if (dxl_error != 0) packetHandler->printRxPacketError(dxl_error);
		//		fprintf(stderr, "\n Success to write\n\n");
	}
	else
	{
		packetHandler->printTxRxResult(dxl_comm_result);
		//		fprintf(stderr, "\n Fail to write! \n\n");
	}
}

void R_Module_Rodrigo::controlModule()
{
	vector<float> sensorValues;
	sensorValues.push_back(1);
	//	control->update(sensorValues);
	vector<float> output = control->update(sensorValues);
	if (output.size() != 1) {
		cout << "ERROR:: output size is not 1, but should be" << endl;
	}
	//	cout << "output[0] = " << output[0] << endl;

	if (output[0] > 1.0) {
		output[0] = 1.0;
	}
	else if (output[0] < -1.0) {
		output[0] = -1.0;
	}
	simSetJointTargetPosition(controlHandles[0], 0.5 * output[0] * M_PI);
//	simSetJointTargetPosition(controlHandles[0], input[0] * M_PI);

	int movePars[3] = { moduleID, 32,  300 };
	write(settings->portHandler, settings->packetHandler1, movePars[0], movePars[1], 2, movePars[2]);
	int angle = (int)(((((1 + (0.5 * output[0]) * 1.3) * 500) - 500)) + 500);
	if (angle > 820) {
		angle = 820;
	}
	if (angle < 200) {
		angle = 200;
	}
	int pars[3] = { moduleID, 30, (int)angle };
	float jPos[3];
	simGetJointPosition(controlHandles[0], jPos);
	cout << "jointPos = " << jPos[0] << endl;
	cout << "angle ID " << moduleID << " is " << angle << endl;
	cout << "jointPos = " << jPos[0] << endl;

	write(settings->portHandler, settings->packetHandler1, pars[0], pars[1], 2, pars[2]);

}

