#include "FixedBaseMorphology.h"
#include <iostream>
using namespace std;

FixedBaseMorphology::FixedBaseMorphology()
{
}

FixedBaseMorphology::~FixedBaseMorphology()
{
//	cout << "should delete FixedBaseMorphology" << endl; 
}

void FixedBaseMorphology::init() {
	cout << "FixedBaseMorphology.init() is called" << endl; 
	addDefaultMorphology();
	BaseMorphology::init();
}

void FixedBaseMorphology::addDefaultMorphology() {
	cout << "addDefaultMorphology is called but the function is deprived" << endl; 
}

void FixedBaseMorphology::saveGenome(int indNum, float fitness) {
	cout << "saving Fixed Genome " << endl << "-------------------------------- " << endl;
	ofstream genomeFile;
	ostringstream genomeFileName;
	genomeFileName << settings->repository << "/morphologies" << settings->sceneNum << "/genome" << indNum << ".csv";
	//	genomeFileName << indNum << ".csv";
	genomeFile.open(genomeFileName.str());
	if (!genomeFile) {
		std::cerr << "Error opening file \"" << genomeFileName.str() << "\" to save genome." << std::endl;
	}
	if (settings->morphologyType == settings->CAT_MORPHOLOGY) {
		genomeFile << ",#GenomeType,CatGenome," << endl;
	}
	else {
		genomeFile << ",#GenomeType,unknown," << endl;
	}
	genomeFile << "#Individual:" << indNum << endl;
	genomeFile << "#Fitness:," << fitness << endl;
	genomeFile << "#ControlParams:," << endl;
	genomeFile << "	#ControlType,0," << endl;
	if (control) {
		genomeFile << control->getControlParams().str() << endl;
	}
	genomeFile << "#EndControlParams" << endl;
	genomeFile << "end of fixed Morphology" << endl;
	cout << "saved cat" << endl;
	genomeFile.close();
}

// Generate sample from distribution
// TODO: Move from here
Eigen::MatrixXd getSamples(int numberOfSamples, const std::vector<float> &mean, const std::vector<float> &sigma)
{
	/*
	  Draw nn samples from a size-dimensional normal distribution
	  with a specified mean and covariance
	  From: https://stackoverflow.com/questions/6142576/sample-from-multivariate-normal-gaussian-distribution-in-c
	*/
	Eigen::internal::scalar_normal_dist_op<double> randN; // Gaussian functor
	Eigen::internal::scalar_normal_dist_op<double>::rng.seed(1); // Seed the rng
	// Define mean and covariance of the distribution
	Eigen::VectorXd means(3);
	Eigen::MatrixXd covar(3, 3);
	means << mean.at(0), mean.at(1), mean.at(2);
	covar << sigma.at(0), 0, 0,
		0, sigma.at(1), 0,
		0, 0, sigma.at(2);

	Eigen::MatrixXd normTransform(3, 3);

	Eigen::LLT<Eigen::MatrixXd> cholSolver(covar);

	// We can only use the cholesky decomposition if
	// the covariance matrix is symmetric, pos-definite.
	// But a covariance matrix might be pos-semi-definite.
	// In that case, we'll go to an EigenSolver
	if (cholSolver.info() == Eigen::Success) {
		// Use cholesky solver
		normTransform = cholSolver.matrixL();
	}
	else {
		// Use eigen solver
		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigenSolver(covar);
		normTransform = eigenSolver.eigenvectors()
			* eigenSolver.eigenvalues().cwiseSqrt().asDiagonal();
	}

	Eigen::MatrixXd samples = (normTransform
		* Eigen::MatrixXd::NullaryExpr(3, numberOfSamples, randN)).colwise()
		+ means;

	//std::cout << "Mean\n" << means << std::endl;
	//std::cout << "Covar\n" << covar << std::endl;
	//std::cout << "Samples\n" << samples << std::endl;

	return samples;
}
