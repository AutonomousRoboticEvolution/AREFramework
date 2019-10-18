#include "FixedBaseMorphology.h"
#include <iostream>


void FixedBaseMorphology::init() {
    std::cout << "FixedBaseMorphology.init() is called" << std::endl;
	addDefaultMorphology();
	BaseMorphology::init();
}

void FixedBaseMorphology::addDefaultMorphology() {
    std::cout << "addDefaultMorphology is called but the function is deprived" << std::endl;
}

void FixedBaseMorphology::saveGenome(int indNum, float fitness) {
    std::cout << "saving Fixed Genome " << std::endl << "-------------------------------- " << std::endl;
    std::ofstream genomeFile;
    std::ostringstream genomeFileName;
	genomeFileName << settings->repository << "/morphologies" << settings->sceneNum << "/genome" << indNum << ".csv";
	//	genomeFileName << indNum << ".csv";
	genomeFile.open(genomeFileName.str());
	if (!genomeFile) {
        std::cerr << "Error opening file \"" << genomeFileName.str() << "\" to save genome." << std::endl;
	}
	if (settings->morphologyType == settings->CAT_MORPHOLOGY) {
        genomeFile << ",#GenomeType,CatGenome," << std::endl;
	}
	else {
        genomeFile << ",#GenomeType,unknown," << std::endl;
	}
    genomeFile << "#Individual:" << indNum << std::endl;
    genomeFile << "#Fitness:," << fitness << std::endl;
    genomeFile << "#ControlParams:," << std::endl;
    genomeFile << "	#ControlType,0," << std::endl;
	if (control) {
        genomeFile << control->getControlParams().str() << std::endl;
	}
    genomeFile << "#EndControlParams" << std::endl;
    genomeFile << "end of fixed Morphology" << std::endl;
    std::cout << "saved cat" << std::endl;
	genomeFile.close();
}

// Generate sample from distribution
// TODO: Move from here
//Eigen::MatrixXd getSamples(int numberOfSamples, const std::vector<float> &mean, const std::vector<float> &sigma)
//{
//	/*
//	  Draw nn samples from a size-dimensional normal distribution
//	  with a specified mean and covariance
//	  From: https://stackoverflow.com/questions/6142576/sample-from-multivariate-normal-gaussian-distribution-in-c
//	*/
//	Eigen::internal::scalar_normal_dist_op<double> randN; // Gaussian functor
//	Eigen::internal::scalar_normal_dist_op<double>::rng.seed(1); // Seed the rng
//	// Define mean and covariance of the distribution
//	Eigen::VectorXd means(3);
//	Eigen::MatrixXd covar(3, 3);
//	means << mean.at(0), mean.at(1), mean.at(2);
//	covar << sigma.at(0), 0, 0,
//		0, sigma.at(1), 0,
//		0, 0, sigma.at(2);
//
//	Eigen::MatrixXd normTransform(3, 3);
//
//	Eigen::LLT<Eigen::MatrixXd> cholSolver(covar);
//
//	// We can only use the cholesky decomposition if
//	// the covariance matrix is symmetric, pos-definite.
//	// But a covariance matrix might be pos-semi-definite.
//	// In that case, we'll go to an EigenSolver
//	if (cholSolver.info() == Eigen::Success) {
//		// Use cholesky solver
//		normTransform = cholSolver.matrixL();
//	}
//	else {
//		// Use eigen solver
//		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigenSolver(covar);
//		normTransform = eigenSolver.eigenvectors()
//			* eigenSolver.eigenvalues().cwiseSqrt().asDiagonal();
//	}
//
//	Eigen::MatrixXd samples = (normTransform
//		* Eigen::MatrixXd::NullaryExpr(3, numberOfSamples, randN)).colwise()
//		+ means;
//
//	//std::std::cout << "Mean\n" << means << std::std::endl;
//	//std::std::cout << "Covar\n" << covar << std::std::endl;
//	//std::std::cout << "Samples\n" << samples << std::std::endl;
//
//	return samples;
//}
