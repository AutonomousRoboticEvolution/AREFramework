#pragma once
#include <vector>
#include "Morphology.h"
#include <list>
#include <memory>

using namespace std;

struct ObjectParameters {
	float size[3];
	float position[3];
	float orientation[3];
	float mass;
	int parent; // points to custom ID
	int type; 
	int state; // only used for LSystems? 
};

struct JointParameters {
	float size[3];
	float position[3];
	float orientation[3];
	float angleLimits[2]; 
	float maxForce; 
	float maxVelocity;
	int parent; // points to custom ID
	int type;
	int state;
};

class BaseMorphology : public Morphology
{

public:
	BaseMorphology();
	virtual ~BaseMorphology();
	
	void split_line(string& line, string delim, list<string>& values);
	shared_ptr<Morphology> clone() const;
	
	void printSome();
	void update();

	// operators of the evolutionary algorithm
	void crossover(shared_ptr<Morphology>, float cr) {};
	void grow() {};
	void mutate();
	void loadGenome(int individualNumber, int sceneNum) {};
	void saveGenome(int indNum, int sceneNum, float fitness);
	void checkControl(int individual, int sceneNum);
	
	// note plural
		
	void setMainHandlePosition(float position[3]); 
	int mainHandle;
	void deleteCreatedMorph() {};
	float morphFitness;
	float getFitness();
//	vector<int> rayHandles;

	void create();
//	void deleteRays();
//	vector<int> rayHandles;

	vector<int> getObjectHandles(int);
	vector<int> getJointHandles(int);
	vector<int> getAllHandles(int);

	void init();
	void init_noMorph() {};
	void createAtPosition(float x, float y, float z);
	int getMainHandle();
	void saveBaseMorphology(int indNum, int sceneNum, float fitness);
	void loadBaseMorphology(int indNum, int sceneNum);
	void clearMorph(); 
	
	// Modular Functions
	int getAmountBrokenModules();
	vector <shared_ptr<ER_Module>> getCreatedModules();
	

	void setPhenValue();

	// variables

};
