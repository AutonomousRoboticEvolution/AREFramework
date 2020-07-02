#ifndef NSGA2_HPP
#define NSGA2_HPP

#include "ARE/EA.h"

namespace are {

struct NSGAInfo
{
    std::vector<Individual::Ptr> dominated; //List of dominated individual by this
    int domination_counter; //number of individuals who dominates this
    float crowd_dist;
    int rank;
};

class NSGAIndividual
{
public:
    NSGAIndividual(){}
    NSGAIndividual(const NSGAIndividual& ind) :
        nsga_info(ind.nsga_info){}


    const NSGAInfo& get_nsga_info(){return nsga_info;}
    NSGAInfo& access_nsga_info(){return nsga_info;}

protected:
    NSGAInfo nsga_info;
};

template<class individual_t>
class NSGA2 : public EA
{
public:

    typedef std::shared_ptr<individual_t> indPtr;

    NSGA2() : EA(){
    }
    NSGA2(const settings::ParametersMapPtr& param) : EA(param){
    }
    ~NSGA2() {
        parents.clear();
        mixed_pop.clear();
        front.clear();
    }

    // base functions of EA
    void selection(){
        assert(population.size()%4 == 0);


        //selection
        std::vector<indPtr> childrens(population.size());

        auto shuffle = [&]() -> std::vector<size_t>{
                std::vector<size_t> res;
                std::vector<size_t> idxes;
                for(size_t i = 0; i < population.size(); i++)
                idxes.push_back(i);

        for(size_t i = 0; i < population.size(); i++){
            int n = randomNum->randInt(0,idxes.size()-1);
            res.push_back(idxes[n]);
            idxes.erase(idxes.begin() + n);
            idxes.shrink_to_fit();
        }
        return res;
    };

    std::vector<size_t> a1 = shuffle();
    std::vector<size_t> a2 = shuffle();

    for(size_t i = 0; i < population.size() - 3; i+=4){
        indPtr ind11 = tournament(parents[a1[i]],parents[a1[i+1]]);
        indPtr ind12 = tournament(parents[a1[i+2]],parents[a1[i+3]]);
        indPtr ind21 = tournament(parents[a2[i]],parents[a2[i+1]]);
        indPtr ind22 = tournament(parents[a2[i+2]],parents[a2[i+3]]);
        //crossover op should be symmetric, so no diff between ind11.cross(ind12) and ind12.cross(ind11)
        individual_t child1 ,child2, child3, child4;
        std::dynamic_pointer_cast<individual_t>(ind11)->crossover(ind12,child1,child2);
        std::dynamic_pointer_cast<individual_t>(ind21)->crossover(ind22,child3,child4);
        childrens[i] = std::make_shared<individual_t>(child1);
        childrens[i+1] = std::make_shared<individual_t>(child2);
        childrens[i+2] = std::make_shared<individual_t>(child3);
        childrens[i=3] = std::make_shared<individual_t>(child4);
    }

                population.clear();
                for(const auto &ind : childrens)
                population.push_back(ind);
    }

                void mutation(){
            for(const auto& ind : population)
                ind->mutate();
        }

        void epoch(){
            bool compute_crowd_distance = settings::getParameter<settings::Boolean>(parameters,"#useNSGA2Diversity").value;

            mixed_pop = parents;
            for(const auto& ind : population)
                mixed_pop.push_back(std::dynamic_pointer_cast<individual_t>(ind));
            parents.clear();

            non_dominated_sort(mixed_pop);

            int i = 0;
            while(parents.size() + front[i].size() < population.size()){
                if(compute_crowd_distance)
                    crowding_distance(i);
                parents.insert(parents.end(),front[i].begin(),front[i].end());
                i++;
            }


            if(compute_crowd_distance)
                crowding_distance(i);
            std::sort(front[i].begin(),front[i].end(),[&](const indPtr &ind1,const indPtr &ind2) -> bool{
                if(ind1->get_nsga_info().rank < ind2->get_nsga_info().rank)
                    return true;
                else if(compute_crowd_distance &&
                        ind1->get_nsga_info().rank == ind2->get_nsga_info().rank &&
                        ind1->get_nsga_info().crowd_dist > ind2->get_nsga_info().crowd_dist)
                    return true;
                return false;
            });

            int extra = population.size() - parents.size();
            for(size_t j = 0; j < extra; j++)
                parents.push_back(front[i][j]);
        }

        void init_next_pop(){
            selection();
            mutation();
        }

        /**
     * @brief set objective boundaries in the following format : { { obj_1 max, obj_1 min}, {obj_2 max, obj_2 min} ... }
     * The number of objective is set on the size of the vector.
     * @param bounds
     */
        void set_obj_bounds(std::vector<std::vector<double>> bounds){
            nb_obj = bounds.size();
            for(std::vector<double>& b : bounds){
                max_obj.push_back(b[0]);
                min_obj.push_back(b[1]);
            }
        }

        //    bool update(const Environment::Ptr&);
        protected:
        NEAT::RNG rng;
        int nb_obj; //number of objectives;
        std::vector<double> max_obj; //maximum value of the objectives
        std::vector<double> min_obj; //minimum value of the objectives

        private:
        std::vector<indPtr> parents;
        std::vector<indPtr> mixed_pop;
        std::vector<std::vector<indPtr>> front;

        int check_dominance(const std::vector<double>& objs1,const std::vector<double>& objs2){
            assert(objs1.size() == objs2.size());

            bool flag1 = false, flag2 = false;

            for(size_t i = 0; i < objs1.size() ;i++){
                if(objs1[i] > objs2[i])
                    flag1 = true;
                else if(objs1[i] < objs2[i])
                    flag2 = true;
            }

            if(flag1 && !flag2)
                return 1; //1 "dominates" 2
            else if(!flag1 && flag2)
                return -1; //2 "dominates" 1
            else return 0; //non domination case
        }
        void non_dominated_sort(const std::vector<indPtr> &pop){
            //filling for the pareto front
            front.clear();
            front.resize(1);
            for(const indPtr& ind1 : pop){
                ind1->access_nsga_info().dominated.clear();
                ind1->access_nsga_info().domination_counter = 0;

                for(const indPtr& ind2 : pop){
                    int dom_comp = check_dominance(ind1->getObjectives(),ind2->getObjectives());
                    if(dom_comp == 1)
                        ind1->access_nsga_info().dominated.push_back(ind2);
                    else if(dom_comp == -1)
                        ind1->access_nsga_info().domination_counter++;
                }

                if(ind1->get_nsga_info().domination_counter == 0){
                    ind1->access_nsga_info().rank = 1;
                    front[0].push_back(ind1);
                }
            }

            //splitting the pareto front into several fronts according to the dominance
            int fi = 1;
            std::vector<indPtr> subfront; //front of next rank
            while (front[fi-1].size() > 0) {
                std::vector<indPtr>& fronti = front[fi - 1];
                subfront.clear();
                for(const auto& ind1_ : fronti){
                    indPtr ind1 = std::dynamic_pointer_cast<individual_t>(ind1_);
                    for(const auto& ind2_ : ind1->get_nsga_info().dominated){
                        indPtr ind2 = std::dynamic_pointer_cast<individual_t>(ind2_);
                        ind2->access_nsga_info().domination_counter--;
                        if(ind2->get_nsga_info().domination_counter == 0){
                            ind2->access_nsga_info().rank = fi + 1;
                            subfront.push_back(ind2);
                        }
                    }
                }

                fi++;
                front.push_back(subfront);
            }
        }

        void crowding_distance(int rank){
            std::vector<indPtr>& F = front[rank];
            if(F.size() == 0) return;

            for(const indPtr &ind : F)
                ind->access_nsga_info().crowd_dist = 0;

            for(size_t i = 0; i < nb_obj; i++)
            {
                std::sort(F.begin(),F.end(),[&](const indPtr& ind1, const indPtr& ind2) -> bool {
                    return ind1->getObjectives()[i] < ind2->getObjectives()[i];
                });
                F.front()->access_nsga_info().crowd_dist = 1e14;
                if(F.size() > 1)
                    F.back()->access_nsga_info().crowd_dist = 1e14;
                for(size_t j = 1; j < F.size() - 1; j++)
                    F[j]->access_nsga_info().crowd_dist +=
                            (F[j+1]->getObjectives()[i] - F[j-1]->getObjectives()[i])/
                            (max_obj[i] - min_obj[i]);
            }
        }
        indPtr tournament(const indPtr & ind1,const indPtr &ind2){
            bool diversity_prev = settings::getParameter<settings::Boolean>(parameters,"#useNSGA2Diversity").value;

            int flag = check_dominance(ind1->getObjectives(),ind2->getObjectives());
            if (flag == 1) // ind1 dominates ind2
                return ind1;
            else if (flag == -1) // ind2 dominates ind1
                return ind2;
            else if (diversity_prev && (ind1->get_nsga_info().crowd_dist > ind2->get_nsga_info().crowd_dist))
                return ind1;
            else if (diversity_prev && (ind2->get_nsga_info().crowd_dist > ind1->get_nsga_info().crowd_dist))
                return ind2;
            else if (randomNum->randInt(0,1))
                return ind1;
            else
                return ind2;
        }
    };


}

#endif //NSGA2_HPP

