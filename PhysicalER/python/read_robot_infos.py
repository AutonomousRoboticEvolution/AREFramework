#! /usr/bin/python3

import sys
import argparse
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

def load_robot_infos(filename):
	with open(filename) as file:
		fitnesses = []
		eval_nbr = []
		trajectories = dict()
		lines = file.readlines()
		state = 0
		for line in lines:
			if(line.split("_")[0] == "evaluation"):
				state = 0
				eval_nbr.append(int(line.split("_")[1]))
			elif(line.split(";")[0] == "fitnesses"):
				fitnesses.append(float(line.split(";")[1]))
			elif(line.split(";")[0] == "trajectory"):
				state = 1
			elif(line.split(";")[0] == "ctrl_genome"):
				state = 2
			elif(state == 1):
				if eval_nbr[-1] in trajectories:
					trajectories[eval_nbr[-1]] += [[float(p) for p in line.split(";")[0].split(",")]]
				else:
					trajectories[eval_nbr[-1]] = [[float(p) for p in line.split(";")[0].split(",")]]
		return eval_nbr, fitnesses, trajectories

def plot_fitnesses(eval_nbr,fitnesses):
	lines = [[e,f] for e, f in zip(eval_nbr,fitnesses)]
	data = pd.DataFrame(data=lines,columns=["eval","fitness"])
	sns.lineplot(data=data,x="eval",y="fitness")
	plt.show()

def plot_trajectory(eval_nbr,trajectories):
	lines = trajectories[eval_nbr]
	if(len(lines[0]) == 3):
		data = pd.DataFrame(data=lines,columns=["x","y","z"])
	else:
		data = pd.DataFrame(data=lines,columns=["x","y"])
	ax = sns.scatterplot(data=data,x="x",y="y")
	ax.set_xlim([-1,1])
	ax.set_ylim([-1,1])
	plt.show()	

def best_fitnesses(n,eval_nbr,fitnesses):
	index_sort = np.argsort(np.array(fitnesses))
	best_fits = []
	for i in index_sort[-n:]:
		best_fits.append((eval_nbr[i],fitnesses[i]))
	return best_fits
if __name__ == '__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument('filename',type=str,help="path to robot infos file")
	parser.add_argument('-f',help="plot fitnesses")
	parser.add_argument('--traj',type=int,default=0,help="plot trajectory of specific evaluation")
	parser.add_argument('--best',type=int,default=1,help="print N best fitnesses and corresponding eval number")
	args = parser.parse_args()
	
	evals, fitnesses, trajectories = load_robot_infos(args.filename)
	if(args.best > 0):
		print(best_fitnesses(args.best,evals,fitnesses))
	if(args.f):
		plot_fitnesses(evals,fitnesses)
	if(not args.traj == 0):
		plot_trajectory(args.traj,trajectories)
			
