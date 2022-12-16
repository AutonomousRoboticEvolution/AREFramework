#include "ARE/learning/Novelty.hpp"

using namespace are;

int Novelty::default_params::k_value = 15;
double Novelty::default_params::novelty_thr = 0.9;
double Novelty::default_params::archive_adding_prob = 0.4;

Novelty::distance_fct_t Novelty::distance_fcts::euclidian = [](Eigen::VectorXd v,Eigen::VectorXd w) -> double {
    return (w-v).norm();
};

Novelty::distance_fct_t Novelty::distance_fcts::positional = [](Eigen::VectorXd v,Eigen::VectorXd w) -> double {
    auto L1 = [](std::array<int,3> p1, std::array<int,3> p2){// Distance L1 on integers
        return abs(p1[0] - p2[0]) + abs(p1[1] - p2[1]) + abs(p1[2] - p2[2]);
    };

    //Retrieve matrix coordinates of all organs with different positions and add to the sum _dim_ if two different organs are on the same position.
    int sum = 0;
    int dim = static_cast<int>(round(cbrt(v.rows()))); //cube root of the number of cells to obtain the dim of the 3D matrix
    std::vector<std::array<int,3>> v_coord, w_coord;
    for(int x = 0; x < dim; x++){
        for(int y = 0; y < dim; y++){
            for(int z = 0; z < dim; z++){
                int i = x + y*dim + z*dim*dim;
                if(w(i) > 0 && v(i) > 0){
                    sum += v(i) != w(i) ? dim : 0;
                    continue;
                }
                else if(v(i) > 0)
                    v_coord.push_back({x,y,z});
                else if(w(i) > 0)
                    w_coord.push_back({x,y,z});
            }
        }
    }

    //If one of the robot does not have any more organs
    //then just return sum of _dim_ of the number of extra organs of the other robot.
    if(v_coord.empty()){
        for(int i = 0; i < w_coord.size(); i++)
            sum+=dim;
        return sum;
    }
    else if(w_coord.empty()){
        for(int i = 0; i < v_coord.size(); i++)
            sum+=dim;
        return sum;
    }

    auto V = std::make_pair(v,v_coord);
    auto W = std::make_pair(w,w_coord);

    //Compute all the distances between the organs on different positions
    std::vector<std::vector<std::array<int,3>>> distances;
    auto& L = std::max(V,W,[](const auto& a,const auto& b) -> bool{return a.second.size() < b.second.size();}); //longest
    auto& S = std::min(W,V,[](const auto& a,const auto& b) -> bool{return a.second.size() < b.second.size();}); //shortest
    for(int i = 0; i < L.second.size(); i++){
        std::vector<std::array<int,3>> dists;
        for(int j = 0; j < S.second.size(); j++){
            dists.push_back({L1(L.second[i],S.second[j]),i,j});
        }
        //sort in ascendent order
        std::sort(dists.begin(),dists.end(),
                  [&](const auto &a, const auto &b){
                        return a[0] < b[0];
        });
        distances.push_back(dists);
    }



    //add to the sum the N first distances (closest organs) and _dim_ if the organs are different.
    //where N is the largest number of organs betweeen the two body-plans
    for(int i = 0; i < std::max(v_coord.size(),w_coord.size()); i++){
        const auto & dist = distances[i][0]; //get the pair with the smallest distance
        int j = L.second[dist[1]][0] + L.second[dist[1]][1]*dim + L.second[dist[1]][2]*dim*dim;
        int k = S.second[dist[2]][0] + S.second[dist[2]][1]*dim + S.second[dist[2]][2]*dim*dim;
        sum += dist[0] + (L.first(j) == S.first(k) ? dim : 0);
    }

    return sum;
};

Novelty::distance_fct_t Novelty::distance_fcts::positional_normalized = [](Eigen::VectorXd v,Eigen::VectorXd w) -> double {
    double dist = Novelty::distance_fcts::positional(v,w);
    double dim = v.rows()/3;
    return dist/(dim*dim*dim*dim);
};



std::vector<double> Novelty::distances(const Eigen::VectorXd &ind_desc,
                                       const std::vector<Eigen::VectorXd> &archive,
                                       const std::vector<Eigen::VectorXd> &pop,
                                       const distance_fct_t dist_fct){

    std::vector<double> dist(archive.size() + pop.size());
    // Comparing with archive
    tbb::parallel_for(tbb::blocked_range<size_t>(0,archive.size()),
                      [&](tbb::blocked_range<size_t> r){

        for(size_t i = r.begin(); i != r.end(); i++){
            dist[i] = dist_fct(archive[i],ind_desc);
        }
    });


    // Comparing with population
    tbb::parallel_for(tbb::blocked_range<size_t>(0,pop.size()),
                      [&](tbb::blocked_range<size_t> r){
        for(size_t i = r.begin(); i != r.end(); i++){
            if(pop[i] == ind_desc)
                dist[i+archive.size()] = 0;
            else
                dist[i+archive.size()] = dist_fct(pop[i],ind_desc);
        }
    });

    std::sort(dist.begin(),dist.end()); // Sorting distances

    return dist;
}


std::vector<double> Novelty::distances(const Eigen::VectorXd &ind_desc,
                                       const std::vector<Eigen::VectorXd> &archive,
                                       const std::vector<Eigen::VectorXd> &pop,
                                       std::vector<size_t> & sorted_pop_indexes,
                                       const distance_fct_t dist_fct){

    std::vector<double> dist(archive.size() + pop.size());

    // Comparing with achive
    tbb::parallel_for(tbb::blocked_range<size_t>(0,archive.size()),
                      [&](tbb::blocked_range<size_t> r){

        for(size_t i = r.begin(); i != r.end(); i++){
            dist[i] = dist_fct(archive[i],ind_desc);
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
                pop_dist[i] = 0;
            else
                pop_dist[i] = dist_fct(pop[i],ind_desc);
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



