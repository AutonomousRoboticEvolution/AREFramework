#ifndef CPPNINDIVIDUAL_H
#define CPPNINDIVIDUAL_H

#include "ARE/Individual.h"
#include "ARE/CPPNGenome.h"
#include "simulatedER/Morphology_CPPNMatrix.h"
#include "v_repLib.h"
#include "eigen_boost_serialization.hpp"
#include "NNControl.h"
#include "mazeEnv_multiTarget.h"

namespace are {

    class CPPNIndividual : public Individual
    {
    public :
        CPPNIndividual() : Individual(){}
        CPPNIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen);

        CPPNIndividual(const CPPNIndividual& ind) :
                Individual(ind),
                final_position(ind.final_position)
        {}



//    CPPNIndividual(const CPPNIndividual& ind) :
//            Individual(ind), nn(ind.nn), morphDesc(ind.morphDesc), testRes(ind.testRes){}

        Individual::Ptr clone();
        /**
         * @brief update the morphlogy and control
         */
        void update(double delta_time);

        template<class archive>
        void serialize(archive &arch, const unsigned int v)
        {
            arch & morphGenome;
            arch & ctrlGenome;
            arch & morphDesc;
            //arch & testRes;
            //arch & manScore;
            arch & objectives;
            arch & final_position;
        }
        // Serialization
        std::string to_string();
        void from_string(const std::string &str);

        void set_final_position(const std::vector<double> fp){final_position = fp;}
        const std::vector<double> get_final_position(){return final_position;}
        // Setters and getters
        NEAT::NeuralNetwork getGenomeMorphology(){return nn_morph;};
        NEAT::NeuralNetwork getGenomeController(){return nn_contrl;};
        Eigen::VectorXd getMorphDesc(){return morphDesc;};
        Eigen::VectorXd getSymDesc(){return symDesc;};
        std::vector<bool> getManRes(){return testRes;};
        std::vector<std::vector<float>> getRawMat(){return rawMat;};
        std::vector<std::vector<float>> getProtoPhenotype(){return protoPhenotype;};
        double getManScore(){ return manScore;};
        void setGenome();
        void setMorphDesc();
        void setManRes();
        void setProtoPhenotype();
        //void setManScore();

        Eigen::VectorXd descriptor(std::vector<double> final_position);
//    {
//        double arena_size = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;
//        Eigen::VectorXd desc(3);
//        desc << (final_position[0]+arena_size/2.)/arena_size, (final_position[1]+arena_size/2.)/arena_size, (final_position[2]+arena_size/2.)/arena_size;
//        return desc;
//    }

        void addObjective(double obj){objectives.push_back(obj);}
        /// Find the best individual all over generations?
        int findBestIndividual();
        void split_line(std::string& line, std::string delim, std::list<std::string>& values);

    protected:
        void createController() override;
        void createMorphology() override;
        void createSubstrate();

        NEAT::NeuralNetwork nn_morph;
        NEAT::NeuralNetwork nn_contrl;
        Eigen::VectorXd morphDesc;
        Eigen::VectorXd symDesc;
        std::vector<bool> testRes;
        double manScore;
        std::vector<std::vector<float>> rawMat;
        std::vector<std::vector<float>> protoPhenotype;
        std::vector<double> final_position;
    };

}//are

#endif //CPPNINDIVIDUAL_H
