#pragma once
#include "Module_LeafChloe.h"

class R_Module_LeafChloe :
	public Module_LeafChloe
{
public:
	R_Module_LeafChloe();
	~R_Module_LeafChloe();
	int init();
		//	void createModule();
	vector<float> updateModule(vector<float> input);
	void controlModule();
	int init(dynamixel::PacketHandler *packetH1, dynamixel::PacketHandler *packetH2, dynamixel::PortHandler *portH);


private:
	int objectPhysics = 8;
};

