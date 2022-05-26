#ifndef NSGA2_HPP
#define NSGA2_HPP

#include "ARE/EA.h"
#include <tbb/tbb.h>

namespace are {

template <class individual_t>
struct NSGAInfo
{
    std::vector<std::shared_ptr<individual_t>> dominated; //List of dominated individual by this
    int domination_counter; //number of individuals who dominates this
    float crowd_dist;
    int rank_;
};

template <class individual_t>
class NSGAIndividual
{
public:
    NSGAIndividual(){}
    NSGAIndividual(const NSGAIndividual& ind) :
        nsga_info(ind.nsga_info){}


    const NSGAInfo<individual_t>& get_nsga_info(){return nsga_info;}
    NSGAInfo<individual_t>& access_nsga_info(){return nsga_info;}

protected:
    NSGAInfo<individual_t> nsga_info;
};

template<class individual_t>
class NSGA2 : public EA
{
public:

    typedef std::shared_ptr<individual_t> indPtr;

    NSGA2() : EA(){
    }
    NSGA2(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}
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

        //Fill indexes 4 by 4
        std::vector<size_t> indexes(
                [&]() -> std::vector<size_t> {
                    std::vector<size_t> v;
                    for(size_t i = 0; i<population.size();i+=4) v.push_back(i);
                    return v;
                }());

        tbb::parallel_for(tbb::blocked_range<size_t>(0,indexes.size()),
                          [&](tbb::blocked_range<size_t> r){
            for(size_t i = r.begin(); i < r.end(); i+=4){
                size_t idx = indexes[i];
                indPtr ind11 = tournament(parents[a1[idx]],parents[a1[i+1]]);
                indPtr ind12 = tournament(parents[a1[idx+2]],parents[a1[i+3]]);
                indPtr ind21 = tournament(parents[a2[idx]],parents[a2[i+1]]);
                indPtr ind22 = tournament(parents[a2[idx+2]],parents[a2[i+3]]);
                //crossover op should be symmetric, so no diff between ind11.cross(ind12) and ind12.cross(ind11)
                indPtr child1(new individual_t(*ind11)) ,child2(new individual_t(*ind12)), child3(new individual_t(*ind21)), child4(new individual_t(*ind22));
                std::dynamic_pointer_cast<individual_t>(ind11)->crossover(ind12,*child1,*child2);
                std::dynamic_pointer_cast<individual_t>(ind21)->crossover(ind22,*child3,*child4);
                childrens[idx] = child1;
                childrens[idx+1] = child2;
                childrens[idx+2] = child3;
                childrens[idx+3] = child4;
            }
        });
        population.clear();
        for(const auto&ind : childrens)
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
            if(ind1->get_nsga_info().rank_ < ind2->get_nsga_info().rank_)
                return true;
            else if(compute_crowd_distance &&
                    ind1->get_nsga_info().rank_ == ind2->get_nsga_info().rank_ &&
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

    //    bool update(const Environment::Ptr&);

    /**
     * @brief set objective boundaries in the following format : { { obj_1 max, obj_1 min}, {obj_2 max, obj_2 min} ... }
     * The number of objective is set on the size of the vector.
     * @param bounds
     */
    void set_obj_bounds(std::vector<std::vector<double>> bounds){
        nb_obj = bounds.size();
        max_obj.clear();
        min_obj.clear();
        for(std::vector<double>& b : bounds){
            max_obj.push_back(b[0]);
            min_obj.push_back(b[1]);
        }
    }

protected:
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

    //Class to do a parallel a non dominating sort
    class NonDominatedSort {
    public:
        NonDominatedSort(NSGA2* nsga2, std::vector<indPtr> pop, indPtr curr_ind) :
           _nsga2(nsga2), _dom_counter(0), _pop(pop), _curr_ind(curr_ind)
        {}

        void operator()( const tbb::blocked_range<size_t>& r ) {
            for(size_t i = r.begin(); i != r.end(); i++){
                int dom_comp = _nsga2->check_dominance(_curr_ind->getObjectives(),_pop[i]->getObjectives());
                if(dom_comp == 1)
                    _dominated.push_back(_pop[i]);
                else if(dom_comp == -1)
                    _dom_counter++;
            }
        }

        NonDominatedSort( NonDominatedSort& x, tbb::split ) :
            _nsga2(x._nsga2), _dom_counter(0), _pop(x._pop), _curr_ind(x._curr_ind) {}

        void join( const NonDominatedSort& y ) {
            _dominated.insert(_dominated.end(),y._dominated.begin(),y._dominated.end());
            _dom_counter+=y._dom_counter;
        }

        std::vector<indPtr> get_dominated(){return _dominated;}
        int get_dom_counter(){return _dom_counter;}
    private:
        int _dom_counter = 0;
        std::vector<indPtr> _dominated;
        std::vector<indPtr> _pop;
        indPtr _curr_ind;
        NSGA2* _nsga2;
    };

    void non_dominated_sort(const std::vector<indPtr> &pop){
        //filling for the pareto front
        front.clear();
        front.resize(1);

        for(const indPtr& ind1 : pop){
            NonDominatedSort nds(this,pop,ind1);
            tbb::parallel_reduce(tbb::blocked_range<size_t>(0,pop.size()),nds);
            ind1->access_nsga_info().dominated = nds.get_dominated();
            ind1->access_nsga_info().domination_counter = nds.get_dom_counter();


            if(ind1->get_nsga_info().domination_counter == 0){
                ind1->access_nsga_info().rank_ = 1;
                front[0].push_back(ind1);
            }
        }

        //TODO parallelisation
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
                        ind2->access_nsga_info().rank_ = fi + 1;
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
