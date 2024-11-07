#include "ARE/genetic_operators.hpp"

using namespace are;

mutators_t mutators::uniform = [](std::vector<double>& genome, double* param, const misc::RandNum::Ptr& rn){
    for(double& gene : genome){
        if(rn->randDouble(0,1) < param[0]){
            gene = gene + rn->randDouble(param[1],param[2]);
        }
    }
};

mutators_t mutators::gaussian = [](std::vector<double>& genome, double* param, const misc::RandNum::Ptr& rn){
    for(double& gene : genome){
        if(rn->randDouble(0,1) < param[0]){
            gene = gene + rn->normalDist(0,param[1]);
            if(gene > param[2])
                gene = param[2];
            if (gene < param[3])
                gene = param[3];
        }
    }
};

mutators_t mutators::polynomial = [](std::vector<double>& genome, double* param, const misc::RandNum::Ptr& rn){
    for(double& gene : genome){
        if(rn->randDouble(0,1) < param[0])
        {
            double ri = rn->randDouble(0,1);
            double delta_i = ri < 0.5 ? std::pow(2.0*ri, 1.0/(param[1] + 1.0)) - 1.0 :
                1.0 - std::pow(2.0*(1.0 - ri), 1.0/(param[1] + 1.0));

            gene = gene + delta_i;
            if(gene > param[2])
                gene = param[2];
            if (gene < param[3])
                gene = param[3];
        }
    }

};

crossover_t crossovers::sbx = [](const std::vector<double> &p1, const std::vector<double> &p2,
        std::vector<double> &c1,std::vector<double>& c2, double* param, const misc::RandNum::Ptr &rn){
    c1.resize(p1.size());
    c2.resize(p2.size());
    for(size_t i = 0; i < p1.size(); i++){
        double y1 = std::min(p1[i], p2[i]);
        double y2 = std::max(p1[i], p2[i]);
        const double lbound = param[0];
        const double ubound = param[1];
        const double eta = param[2];
        if(fabs(y1-y2) > std::numeric_limits<double>::epsilon()){
            double rand = rn->randDouble(0,1);
            double beta = 1.0 + 2.0*(y1 - lbound)/(y2 - y1);
            double alpha = 2.0 - pow(beta, -(eta + 1.0));
            double betaq = 0;
            if(rand <= (1.0/alpha))
                betaq = pow((rand * alpha), (1.0 / (eta + 1.0)));
            else
                betaq = pow ((1.0 / (2.0 - rand * alpha)) , (1.0 / (eta + 1.0)));
            double x1 = 0.5*((y1 + y2) - betaq*(y2-y1));

            beta = 1.0 + 2.0*(ubound - y2)/(y2 - y1);
            alpha = 2.0 - pow(beta, -(eta + 1.0));
            if(rand <= (1.0/alpha))
                betaq = pow((rand * alpha), (1.0 / (eta + 1.0)));
            else
                betaq = pow ((1.0 / (2.0 - rand * alpha)) , (1.0 / (eta + 1.0)));
            double x2 = 0.5*((y1 + y2) - betaq*(y2-y1));

            if(x1 < lbound) x1 = lbound;
            else if(x1 > ubound) x1 = ubound;
            if(x2 < lbound) x2 = lbound;
            else if(x2 > ubound) x2 = ubound;

            if(rn->randInt(0,1)){
                c1[i] = x1;
                c2[i] = x2;
            }else{
                c1[i] = x2;
                c2[i] = x1;
            }
        }
        else{
            c1[i] = y1;
            c2[i] = y2;
        }
    }
};
