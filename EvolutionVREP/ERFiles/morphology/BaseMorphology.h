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
	virtual shared_ptr<Morphology> clone() const override;
	
	virtual void printSome() override;
	virtual void update() override;

	// operators of the evolutionary algorithm
	virtual void crossover(shared_ptr<Morphology>, float cr) override {};
	void grow() {};
	virtual void mutate() override;
	virtual bool loadGenome(int individualNumber, int sceneNum) override;
	virtual bool loadGenome(std::istream &input, int individualNumber) override;
	virtual void saveGenome(int indNum, float fitness) override;
	virtual const std::string generateGenome(int indNum, float fitness) const override;
	// void checkControl(int individual, int sceneNum);
	
	virtual void savePhenotype(int ind, float fitness) override {};
	// note plural
		
	void setMainHandlePosition(float position[3]);
	void deleteCreatedMorph() {};
	virtual float getFitness() override;
//	vector<int> rayHandles;

	virtual void create() override;
//	void deleteRays();
//	vector<int> rayHandles;

	virtual vector<int> getObjectHandles(int) override;
	virtual vector<int> getJointHandles(int) override;
	virtual vector<int> getAllHandles(int) override;

	virtual void init() override;
	virtual void init_noMorph() override {};
	virtual void createAtPosition(float x, float y, float z) override;
	virtual int getMainHandle() override;
	virtual void saveBaseMorphology(int indNum, float fitness) override;
	virtual void loadBaseMorphology(int indNum, int sceneNum) override;
	virtual void clearMorph() override;
	
	// Modular Functions
	virtual int getAmountBrokenModules() override;
	virtual vector <shared_ptr<ER_Module>> getCreatedModules() override;

	virtual void setPhenValue() override;

	// variables
	int mainHandle;
	float morphFitness;
};
