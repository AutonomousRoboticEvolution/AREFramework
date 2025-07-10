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

if __name__ == "__main__":
    filename = sys.argv[1]
    x_max = int(sys.argv[2])
    x_min = int(sys.argv[3])
    y_max = int(sys.argv[4])
    y_min = int(sys.argv[5])

    traj = load_trajectory(filename)
    df_traj = pd.DataFrame(traj,columns=["time","x","y","z"])
    ax = sns.scatterplot(df_traj,x="x",y="y",hue="time")
    ax.set_xlim(x_min,x_max)
    ax.set_ylim(y_min,x_max)
    plt.show()