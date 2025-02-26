import matplotlib.pyplot as plt
import numpy as np
import sys 

def load_matrix_desc(filename: str):
    matrix_descs = []
    with open(filename) as file:
        lines = file.readlines()
        for line in lines:
            i = 0
            j = 0
            roww = []
            row = []
            matrix = []
            for elt in line.split(',')[1:-1]:
                row.append(int(elt))
                i+=1
                if(i==11):
                    roww.append(row)
                    row = []
                    j+=1
                    i = 0
                if(j==11):
                    matrix.append(roww)
                    roww = []
                    j = 0
            matrix_descs.append(matrix)
        return matrix_descs

        
if __name__ == "__main__":

    filename = sys.argv[1]
    robot_id = int(sys.argv[2])
    matrix = np.array(load_matrix_desc(filename)[robot_id])



    colours = np.where(matrix == 1, "blue",matrix) #skeleton
    colours = np.where(colours == '2', "green",colours) #wheel
    colours = np.where(colours == '3', "red",colours) #sensor
    colours = np.where(colours == '4', "yellow",colours) #joint
    colours = np.where(colours == '5', "pink",colours) #caster

    # and plot everything
    ax = plt.figure().add_subplot(projection='3d')
    ax.voxels(matrix,facecolors=colours,edgecolor='k')

    plt.show()
