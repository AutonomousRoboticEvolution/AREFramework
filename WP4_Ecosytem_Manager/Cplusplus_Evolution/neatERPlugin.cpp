#pragma once
#include <iostream>
#include <vector>
#include "NEAT/code/adaptiveNEAT/neat.h"
#include "NEAT/code/adaptiveNEAT/population.h"
#include "NEAT/code/adaptiveNEAT/experiments.h"
#include "NEAT/code/adaptiveNEAT/mazeExperiment.h"
#include "NEAT/code/adaptiveNEAT/mapMaze.h"
#include "time.h"
//#include "neat.h"
//#include "population.h"
//#include "experiments.h"
//#include "mazeExperiment.h"
//#include "mapMaze.h"

//#include "mainFrame.h"

using namespace std;
using namespace NEAT;

//#include "serialPortCommunication\Serial.h"


/**
TODO: test if modulatory signal can go negative. would be bad for hebbian learning formula
flush modulatory signals
incorporate Modularity-Neurons into Species-Distance
check if modulartory networks load correctly

variable archive treshold 

periodically recalculate novelty for all individuals

test doubleTMaze on more than two trials. has to generalise for the 4 different high reward locations

!Incorporate current Population into novelty search!

trait 2 0.0 0.5 0 0 0 0 0 0
trait 3 0.2 0 0 0 0 0 0 0
trait 4 0.3 0.1 0 0 0 0 0 0
trait 5 0.7 0.1 0 0 0 0 0 0
trait 6 0.3 0.5 0 0 0 0 0 0
trait 7 0.3 0.1 0 0 0 0 0 0
trait 8 0.3 0.6 0 0 0 0 0 0
*/


int main(int argc, char *argv[]) {
  /* Seed the random-number generator with current time so that
      the numbers will be different every time we run.    */
  srand( (unsigned)time( NULL ) );

  // if ( argc != 4) 
  // {// argc should be 2 for correct execution
    // We print argv[0] assuming it is the program name
  //  cout<<"usage: adaptiveNEAT <novelty|fitness> <identifier> <maxEvaluations>\n";
	//return 0;
  //}
  int numEvals;
  NEAT::load_neat_params("../data/mazeparams.ne", true);

  numEvals = 2000;
  numEvals = atoi(argv[3]);

//   cout<<argv[1]<<" "<<argv[2];

  //Load in the params

  cout<<"loaded"<<endl;

  char curword[20] = "cuparm";
  ostringstream file_name;
  file_name.str(argv[2]);

  MazeExperiment* mazeExperiment = new MazeExperiment();
 // mazeExperiment->test_realtime("../data/simpleTMaze.txt", false, file_name, numEvals);
  cout << "about to test real time" << endl;
  mazeExperiment->test_realtime("../data/simpleTMaze.txt", false, argv[2], numEvals);

  bool loadBest = false;
  if (loadBest == true) {

	  //Serial* serial = new Serial("\\\\.\\COM3");
	  Genome* g = Genome::new_Genome_load("r02_FITNESS_66.65_evals171_champ.txt");
	  cout << "LOADED GENOME" << endl;
		  //  Network* network = Genome::new_Genome_load("cuparm_FITNESS_35887_evals23_champ.txt")->genesis(0);
	  Organism* o = new Organism(0, g, 0, NULL);
	  //o->net = network;


	  for (int i = 0; i < 200; i++) {
		  mazeExperiment->evaluateComPort(o);
	  }
	  // pole1_test(100);

	 // XOR_test_realtime();
	 // if (strcmp(argv[1],"novelty")==0)
	 // {
	//	   mazeExperiment->test_realtime("../data/simpleTMaze.txt", true, argv[2], numEvals);
	 // }
	 // else if (strcmp(argv[1],"fitness")==0)
	 // {
	//	   mazeExperiment->test_realtime("../data/simpleTMaze.txt", false, argv[2], numEvals);
	 // }
	 // else
	  //{
		//  cout<<"adaptiveNEAT has to be called with either fitness or novelty";
	//  }
	//
  }
  cout << "done" << endl;
  return(0); 
}