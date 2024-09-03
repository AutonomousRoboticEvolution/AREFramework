import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import sys

def load_trajectory(filename):
    traj = []
    with open(filename) as file :
        lines = file.readlines()
        
        t = 0
        for line in lines:
            line = line.split(";")[0].split(",")
            pos = [float(elt) for elt in line]
            traj.append([t] + pos)
            t+=1
    return traj

def traj_to_2d_coord(traj):
    return [t[1] for t in traj], [t[2] for t in traj]

if __name__ == "__main__":
    filename = sys.argv[1]
    task = sys.argv[2]

    traj = load_trajectory(filename)
    data_traj = pd.DataFrame(data=traj,columns=["time","x","y","z"])
    if task == "locomotion":
        fig = plt.gcf()
        fig.set_size_inches(30,10)
        ax = sns.scatterplot(data_traj,x="x",y="y",hue="time")
        ax.set_xlim([-6,6])
        ax.set_ylim([-1,1])
    elif task == "hill_climbing":
        fig = plt.gcf()
        fig.set_size_inches(20,20)
        ax = sns.scatterplot(data_traj,x="x",y="y",hue="time")
        ax.set_xlim([-2,2])
        ax.set_ylim([-2,2])
    
    plt.show()
