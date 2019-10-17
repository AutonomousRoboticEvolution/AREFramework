#pragma once

#include <memory>
#include <vector>
#include "Settings.h"
#include "Genome.h"
#include "../RandNum.h"

class DefaultGenome : public Genome
{
public:
	DefaultGenome(std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st);
        DefaultGenome(MorphologyFactory::Ptr factory,std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st);
        DefaultGenome() {};

	~DefaultGenome();
	virtual std::shared_ptr<Genome> clone() const override;

	void createInitialMorphology(int individualNumber);

	float mutationRate = 0.05f;

	void savePhenotype(int indNum, int sceneNum) override;
	virtual void init() override;
	virtual void create() override;
	virtual void update() override;
	virtual void mutate() override;
	virtual bool loadGenome(int indNum, int sceneNum) override;
	virtual bool loadGenome(std::istream &input, int indNum) override;
	virtual void saveGenome(int indNum) override;
	virtual const std::string generateGenome() const override;
	void clearGenome();
	virtual void checkGenome() override;
	bool loadMorphologyGenome(int indNum, int sceneNum);
	bool loadMorphologyGenome(std::istream &input, int indNum);
    std::shared_ptr<Genome> cloneGenome();

	void init_noMorph();
	void createAtPosition(float x, float y, float z);

	//void mutate();
	float genomeFitness;

	void loadBaseMorphology(int indNum, int sceneNum); // old

    std::function<std::shared_ptr<Control>
        (int type, std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st)> controlFactory;
    std::function<std::shared_ptr<Morphology>
        (int type, std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st)> morphologyFactory;

//protected:
	// virtual functions to extend with simulator specific classes
//	virtual std::shared_ptr<MorphologyFactory> newMorphologyFactory();
};
