#! /usr/bin/python3

import sys
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

def read_file(filename):
  with open(filename) as file:
    lines = file.readlines()
    nbr_inputs = int(lines[1].split(";")[0])
    nbr_outputs = int(lines[1].split(";")[1])
    values = []
    for line, n in zip(lines,range(len(lines))):
      if(n < 2):
        continue
      line_split = line.split(";")
      line_split[-1] = line_split[-1][:-1]
      for i in range(nbr_inputs):
        values.append([n,float(line_split[i]),"i"+str(i)])
      for o in range(nbr_outputs):
        values.append([n,float(line_split[o+nbr_inputs]),"o"+str(o)])
      for o in range(nbr_outputs):
        values.append([n,float(line_split[o+nbr_inputs+nbr_outputs]),"h"+str(o)])
      for o in range(nbr_outputs):
        values.append([n,float(line_split[o+nbr_inputs+nbr_outputs*2]),"cpg"+str(o)])
 
    return pd.DataFrame(values,columns=["index","value","name"])

if __name__ == '__main__':
  data = read_file(sys.argv[1]) 
  ax = sns.relplot(data=data,x="index",y="value",row="name",kind="line")
  plt.show()

    
