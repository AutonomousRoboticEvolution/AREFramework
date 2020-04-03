#include "Novelty.hpp"

using namespace are;

int Novelty::k_value = 15;
double Novelty::novelty_thr = 0.9;
double Novelty::archive_adding_prob = 0.4;

double Novelty::sparseness(const std::vector<double> &dist){


    double sum = 0;
    if(dist.size() >  k_value + 1){
        //sum = std::accumulate(dist.begin(),dist.begin() + kValue, 0);  \\\ \todo EB: This method is not working
        for(int i = 0; i < dist.size(); i++){
            sum += dist[i];
            if(i >= k_value) break;
        }
    }
    if(isnan(sum/static_cast<double>(k_value))){
        std::cerr << "NaN found" << std::endl;
    }
    return sum/static_cast<double>(k_value);
}

std::vector<double> Novelty::distances(const Eigen::VectorXd &ind_desc,
                                       const std::vector<Eigen::VectorXd> &archive,
                                       const std::vector<Eigen::VectorXd> &pop){

    std::vector<double> dist;
    for(const Eigen::VectorXd& desc : archive)
        dist.push_back((desc - ind_desc).norm());
    /// \todo EB: IMPORTANT Compute Novelty with population as well.
    // Comparing with population
    for(const Eigen::VectorXd& desc : pop) {
        if(desc == ind_desc)
            dist.push_back(1.);
        else
            dist.push_back((desc - ind_desc).norm());
    }
    std::sort(dist.begin(),dist.end()); // Sorting distances

    return dist;
}


std::vector<double> Novelty::distances(const Eigen::VectorXd &ind_desc,
                                       const std::vector<Eigen::VectorXd> &archive,
                                       const std::vector<Eigen::VectorXd> &pop,
                                       std::vector<size_t> & sorted_pop_indexes){

    std::vector<double> dist;
    for(const Eigen::VectorXd& desc : archive)
        dist.push_back((desc - ind_desc).norm());
    /// \todo EB: IMPORTANT Compute Novelty with population as well.
    // Comparing with population
    std::vector<double> pop_dist;
    int i = 0;
    for(const Eigen::VectorXd& desc : pop) {
        sorted_pop_indexes.push_back(i);
        i++;
        if(desc == ind_desc)
            pop_dist.push_back(1.);
        else
            pop_dist.push_back((desc - ind_desc).norm());
        dist.push_back(pop_dist.back());
    }
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
