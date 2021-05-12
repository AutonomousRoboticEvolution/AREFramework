# Load libraries
# import pandas as pd
# data = pd.read_csv('fitnesses.csv')
# fitnessData = data.columns[3:-1]
# print(fitnessData)
# length = len(fitnessData)
# print("The length of data is: ", length)
import csv
import matplotlib.pyplot as plt
import statistics
# Import data from csv
my_list = []
with open('fitnesses.csv', 'r') as csvfile:
    data = csv.reader(csvfile, delimiter=',', quotechar='"')
    for row in data:
        my_list.append(row[3:-1])
print(my_list)
# Estimate average for each generation
numGenerations = len(my_list)
populationSize = len(my_list[0])
print("Generations: ", numGenerations, "Population: ", populationSize)
genFitAve = []
genMaxFit = []
genMedFit = []
for i in range(numGenerations):
    average = 0;
    for j in range(populationSize):
        average = average + float(my_list[i][j])
    average = average / populationSize
    genFitAve.append(average)
    genMedFit.append(statistics.median([float(j) for j in my_list[i]]))
    genMaxFit.append(max(my_list[i]))
print(genFitAve)
genMaxFit = [float(i) for i in genMaxFit]
print(genMaxFit)

# Plot stuff
xValues = list(range(1, numGenerations+1))
dashedLine = [1.5] * numGenerations
plt.figure(0)
plt.plot(xValues, genFitAve)
plt.plot(xValues, dashedLine, '--')
plt.xlabel('Generations')
plt.ylabel('Novelty')
axes = plt.gca()
axes.set_ylim([0,1.0])
plt.title("Average Novelty")
#plt.show()
plt.savefig('AverageFitness.png')

xValues = list(range(1, numGenerations+1))
dashedLine = [1.5] * numGenerations
plt.figure(1)
plt.plot(xValues, genMaxFit)
plt.plot(xValues, dashedLine, '--')
plt.xlabel('Generations')
plt.ylabel('Novelty')
axes = plt.gca()
axes.set_ylim([0,1.0])
plt.title("Highest Novelty")
#plt.show()
plt.savefig('HighestFitness.png')

xValues = list(range(1, numGenerations+1))
dashedLine = [1.5] * numGenerations
plt.figure(2)
plt.plot(xValues, genMedFit)
plt.plot(xValues, dashedLine, '--')
plt.xlabel('Generations')
plt.ylabel('Novelty')
axes = plt.gca()
axes.set_ylim([0,1.0])
plt.title("Median novelty")
#plt.show()
plt.savefig('MedianNovelty.png')
