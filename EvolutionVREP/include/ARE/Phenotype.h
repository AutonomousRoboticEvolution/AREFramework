#ifndef PHENOTYPE_H
#define PHENOTYPE_H

#include <memory>

namespace are {

class Phenotype
{
public:
    typedef std::shared_ptr<Phenotype> Ptr;
    typedef std::shared_ptr<const Phenotype> ConstPtr;

    Phenotype(){}


};

}

#endif //PHENOTYPE_H
