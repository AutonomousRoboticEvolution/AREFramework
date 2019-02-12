#pragma once
#include "BaseMorphology.h"
#include <Eigen/Dense>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>

class FixedBaseMorphology :
	public BaseMorphology
{
public:
	FixedBaseMorphology();
	~FixedBaseMorphology();
	void init();
	void addDefaultMorphology();
	void saveGenome(int indNum,float fitness);

};

// TODO: Move from here! (Edgar)
namespace Eigen {
	namespace internal {
		template<typename Scalar>
		struct scalar_normal_dist_op
		{
			static boost::mt19937 rng;    // The uniform pseudo-random algorithm
			mutable boost::normal_distribution<Scalar> norm;  // The gaussian combinator

			EIGEN_EMPTY_STRUCT_CTOR(scalar_normal_dist_op)

				template<typename Index>
			inline const Scalar operator() (Index, Index = 0) const { return norm(rng); }
		};
		template<typename Scalar> boost::mt19937 scalar_normal_dist_op<Scalar>::rng;

		template<typename Scalar>
		struct functor_traits<scalar_normal_dist_op<Scalar> >
		{
			enum { Cost = 50 * NumTraits<Scalar>::MulCost, PacketAccess = false, IsRepeatable = false };
		};
	} // end namespace internal
} // end namespace Eigen
template <typename Derived>
Eigen::MatrixXd createVoxelsFromPoints(const Eigen::MatrixBase<Derived>& allSamples);
Eigen::MatrixXd getSamples(int numberOfSamples, const std::vector<float> &mean, const std::vector<float> &sigma);