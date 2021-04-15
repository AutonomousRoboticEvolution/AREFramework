# Novelty Search with Local Competition

This experiment is an implementation of NSLC [1] based on NSGA-II [2]. The task is a navigation task with as reward the normalised distance between the final position of the robot and the target position. 
The robot is controlled thanks to a neural network which is either a feed-forward neural network or a continuous-time recurrent neural network. The genotype is the parameters of the neural network and the mutation operator is the one implemented in MultiNEAT
The EA is NSGA-II which is an elitist multi-objective EA. NSGA-II is implemented in [/common/NSGA2.hpp](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/master/EvolutionVREP/experiments/common/NSGA2.hpp).
The two objectives are the novelty and the local competition score. The implementation of the novelty score is in [/common/Novelty.cpp](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/master/EvolutionVREP/experiments/common/Novelty.cpp)
 and the local competition score is implemented in [NSLC.cpp](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/master/EvolutionVREP/experiments/nslc/NSLC.cpp).

References:

[1] Lehman, J., & Stanley, K. O. (2011, July). Evolving a diversity of virtual creatures through novelty search and local competition. In Proceedings of the 13th annual conference on Genetic and evolutionary computation (pp. 211-218).

[2] Deb, K., Pratap, A., Agarwal, S., & Meyarivan, T. A. M. T. (2002). A fast and elitist multiobjective genetic algorithm: NSGA-II. IEEE transactions on evolutionary computation, 6(2), 182-197.

**To Do**:

-    Replace the NN implemention of MultiNEAT by nn2
-    implement a gaussian and/or polynomial mutation
