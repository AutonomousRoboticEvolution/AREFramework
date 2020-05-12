# Neuro-Evolution Experiment

This experiment is a basic example of experiment using the ARE Framework.

An evolutionary algorithm optimises the parameters of a fixed network. It uses tournament selection and mutation operator on the reals defined in MultiNEAT.
The network is either a feed-forward neural network or a continuous-time recurrent neural network. The task is a navigation task with as reward the normalised distance from the final position of the robot to the target position.

**To Do**:

- Replace the NN implenmation of MultiNEAT by nn2
- put a gaussian and/or polynomial mutation