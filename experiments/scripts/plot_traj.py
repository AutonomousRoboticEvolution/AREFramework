import matplotlib.pyplot as plt
import seaborn as sns
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

    traj = load_trajectory(filename)

    plt.plot(traj[0],traj[1])
    plt.show()