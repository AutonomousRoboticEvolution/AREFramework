#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <list>
#include "ARE/Settings.h"
#include "v_repLib.h"

extern "C" {
#include "v_repConst.h"
#include "remoteApi/extApiPlatform.h"
#include "remoteApi/extApi.h"
}


namespace misc {

void split_line(const std::string &line,const std::string &delim,
                std::list<std::string> &values);

}//misc

namespace sim {

int loadScene(int instance_type,const std::string &scene_path, int clientID = 0);
int loadModel(int instance_type, const  std::string &model_path,  int clientID = 0);
int setJointVelocity(int instance_type,
                      int jointHandle,
                      float value,
                      int clientID = 0);
int pause(int instance_type, int clientID = 0);
void pauseCommunication(int instance_type,int shouldPause,int clientID = 0);
void setFloatingParameter(int instance_type, int param_name, float value, int clientID = 0);
void setObjectPosition(int instance_type, int hanlde, int base, float* position, int clientID = 0);
void getObjectPosition(int instance_type, int hanlde, int base, float* position, int clientID = 0);
int readProximitySensor(int instance_type, int handle, float* pos, int* targetHandle, float* vector, int clientID = 0);

}
#endif //UTILITIES_H
