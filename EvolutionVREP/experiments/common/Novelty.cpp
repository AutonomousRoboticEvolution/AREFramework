#include "Novelty.hpp"

using namespace are;

int Novelty::k_value = 15;
double Novelty::novelty_thr = 0.9;
double Novelty::archive_adding_prob = 0.4;

//parallel_reduce(
//        blocked_range<float*>( array, array+n ),
//        0.f,
//        [](const blocked_range<float*>& r, float init)->float {
//            for( float* a=r.begin(); a!=r.end(); ++a )
//                init += *a;
//            return init;
//        },
//        []( float x, float y )->float {
//            return x+y;
//        }
//    );

double Novelty::sparseness(const std::vector<double> &dist){

    double sum = 0;
    if(dist.size() >  k_value + 1){

//        tbb::parallel_reduce(tbb::blocked_range<size_t>(0,dist.size()),0,
//                             [&](const tbb::blocked_range<size_t>& r, double init) -> double {
//            for(size_t i = r.begin(); i != r.end(); i++){
//                init+=dist[i];
//                if(i>=k_value) return init;
//            }
//            return init;
//        },
//        [&](double x, double y) -> double {

//        }
//                             )

        for(int i = 0; i < k_value; i++)
            sum += dist[i];

    }
    if(isnan(sum/static_cast<double>(k_value))){
        std::cerr << "NaN found" << std::endl;
    }
    return sum/static_cast<double>(k_value);
}

std::vector<double> Novelty::distances(const Eigen::VectorXd &ind_desc,
                                       const std::vector<Eigen::VectorXd> &archive,
                                       const std::vector<Eigen::VectorXd> &pop){

    std::vector<double> dist(archive.size() + pop.size());
    // Comparing with archive
    tbb::parallel_for(tbb::blocked_range<size_t>(0,archive.size()),
                      [&](tbb::blocked_range<size_t> r){

        for(size_t i = r.begin(); i != r.end(); i++){
            dist[i] = (archive[i] - ind_desc).norm();
        }
    });


    // Comparing with population
    tbb::parallel_for(tbb::blocked_range<size_t>(0,pop.size()),
                      [&](tbb::blocked_range<size_t> r){
        for(size_t i = r.begin(); i != r.end(); i++){
            if(pop[i] == ind_desc)
                dist[i+archive.size()] = 1.;
            else
                dist[i+archive.size()] = (pop[i] - ind_desc).norm();
        }
    });

    std::sort(dist.begin(),dist.end()); // Sorting distances

    return dist;
}


std::vector<double> Novelty::distances(const Eigen::VectorXd &ind_desc,
                                       const std::vector<Eigen::VectorXd> &archive,
                                       const std::vector<Eigen::VectorXd> &pop,
                                       std::vector<size_t> & sorted_pop_indexes){

    std::vector<double> dist(archive.size() + pop.size());

    // Comparing with achive
    tbb::parallel_for(tbb::blocked_range<size_t>(0,archive.size()),
                      [&](tbb::blocked_range<size_t> r){

        for(size_t i = r.begin(); i != r.end(); i++){
            dist[i] = (archive[i] - ind_desc).norm();
        }
    });

    // Comparing with population
    std::vector<double> pop_dist(pop.size());
    sorted_pop_indexes.resize(pop.size());
    tbb::parallel_for(tbb::blocked_range<size_t>(0,pop.size()),
                      [&](tbb::blocked_range<size_t> r){
        for(size_t i = r.begin(); i != r.end(); i++){
            sorted_pop_indexes[i] = i;
            if(pop[i] == ind_desc)
                pop_dist[i] = 1.;
            else
                pop_dist[i] = (pop[i] - ind_desc).norm();
            dist[i+archive.size()] = pop_dist[i];
        }
    });


    //sort indexes of population from closest to farthest of ind_desc.
    std::sort(sorted_pop_indexes.begin(),sorted_pop_indexes.end(),[&](size_t a, size_t b){
        return pop_dist[a] < pop_dist[b];
    });
    std::sort(dist.begin(),dist.end()); // Sorting distances

    return dist;
}

void Novelty::update_archive(const Eigen::VectorXd &ind_desc,
                             double ind_nov,
                             std::vector<Eigen::VectorXd> &archive,
                             const misc::RandNum::Ptr &rn){

   if(ind_nov > novelty_thr || rn->randInt(0,1) < archive_adding_prob){
        archive.push_back(ind_desc);
    }

}
