#pragma once
#include "Module_Rodrigo.h"
//#include "../robotModules/moduleControl.h"

class R_Module_Rodrigo : public Module_Rodrigo
{
public:
	int init(dynamixel::PacketHandler *packetH1, dynamixel::PacketHandler *packetH2, dynamixel::PortHandler *portH);
	R_Module_Rodrigo();
	shared_ptr<ER_Module> clone(); // every module needs a clone, also derived/sub classes
	~R_Module_Rodrigo();
	int init();
	float mutate();
//	void createModule();
	vector<float> updateModule(vector<float> input);
	void write(dynamixel::PortHandler * portHandler, dynamixel::PacketHandler * packetHandler, uint8_t id, uint16_t addr, uint16_t length, uint32_t value);
	void controlModule();

};

