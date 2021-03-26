#ifndef BLOB_TRACKER_HPP
#define BLOB_TRACKER_HPP

#include "ARE/EA.h"
#include "ARE/Settings.h"

namespace are{

class BlobInd : public Individual
{
public:

    typedef std::shared_ptr<BlobInd> Ptr;
    typedef std::shared_ptr<const BlobInd> ConstPtr;

    BlobInd() : Individual(){}
    Individual::Ptr clone() override{}
    void init() override;
    void update(double delta_time) override;
    void createController() override{}
    void createMorphology() override{}
};

class BlobTracker : public EA
{
public:
    typedef std::shared_ptr<BlobTracker> Ptr;
    typedef std::shared_ptr<const BlobTracker> ConstPtr;

    BlobTracker(){}
    BlobTracker(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}
    void init() override;

};

}//are

#endif
