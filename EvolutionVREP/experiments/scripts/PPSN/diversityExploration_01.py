# Load libraries
# import pandas as pd
# data = pd.read_csv('fitnesses.csv')
# fitnessData = data.columns[3:-1]
# print(fitnessData)
# length = len(fitnessData)
# print("The length of data is: ", length)
import csv
import matplotlib.pyplot as plt
import numpy as np
# Import data from csv
my_list = []
with open('morphDesc.csv', 'r') as csvfile:
    data = csv.reader(csvfile, delimiter=',', quotechar='"')
    for row in data:
        my_list.append(row)
#print(my_list)
# Estimate average for each generation
evaluations = len(my_list)
featureSize = len(my_list[0])
print("Number of evaluations: ", evaluations, "Features size: ", featureSize)

dat = np.array([[float(y) for y in x] for x in my_list]) # Convert to array
counter = 0
progress = []

#for ev in range(50,evaluations,50):
#  for a in np.arange(0.05,0.95,0.1):
#    for b in np.arange(0.05,0.95,0.1):
#      for c in np.arange(0.05,0.95,0.1):
#        for d in np.arange(0.05,0.95,0.1):
#          for e in np.arange(0.05,0.95,0.1):
#            for f in np.arange(0.05,0.95,0.1):
#              for elem in dat[:ev,:]:
#                if((a - 0.06 < elem[0]) and (elem[0] < a + 0.06) and (b - 0.0 < elem[1]) and (elem[1] < b + 0.06) and (c - 0.06 < elem[2]) and (elem[2] < c + 0.06) and (d - 0.0 < elem[3]) and (elem[3] < d + 0.06) and (e - 0.0 < elem[4]) and (elem[4] < e + 0.06) and (f - 0.0 < elem[5]) and (elem[5] < f + 0.06)):
#                  counter = counter + 1
#                  print(str(counter) + "%")
#                  break;
#              progress.append(counter)
#              #print (counter)
#              counter = 0
#  print("Generation: " + str(ev/50))
#print(progress)

for a in np.arange(0.05,0.95,0.1):
  for b in np.arange(0.05,0.95,0.1):
    for c in np.arange(0.05,0.95,0.1):
      for d in np.arange(0.05,0.95,0.1):
        for e in np.arange(0.05,0.95,0.1):
          for f in np.arange(0.05,0.95,0.1):
            for elem in dat[:,:]:
              if((a - 0.06 < elem[0]) and (elem[0] < a + 0.06) and (b - 0.0 < elem[1]) and (elem[1] < b + 0.06) and (c - 0.06 < elem[2]) and (elem[2] < c + 0.06) and (d - 0.0 < elem[3]) and (elem[3] < d + 0.06) and (e - 0.0 < elem[4]) and (elem[4] < e + 0.06) and (f - 0.0 < elem[5]) and (elem[5] < f + 0.06)):
                counter = counter + 1
                print(str(counter) + "%")
                break;
progress.append(counter)
#print (counter)
counter = 0
print(progress)


      
xValues = list(range(50, evaluations, 50))
xValues = [x/50 for x in xValues]
plt.figure(0)
plt.plot(xValues, progress)
# plt.plot(xValues, dashedLine, '--')
plt.xlabel('Generations')
plt.ylabel('Progress')
axes = plt.gca()
#axes.set_xlim([0,1.0])
axes.set_ylim([0,100.0])
#plt.show()
plt.savefig('progressWidthDepth.png')

      
   
# Plot stuff
#plt.figure(0)
#plt.xlabel('Width')
#plt.ylabel('Depth')
##plt.scatter(dat[:,0],dat[:,1],c=dat[:,6])
#plt.scatter(dat[dat[:,6] > 0,0],dat[dat[:,6] > 0,1],marker='+')
#plt.scatter(dat[dat[:,6] == 0,0],dat[dat[:,6] == 0,1],marker='x')
#plt.legend(['Manufacturable robots','Non-Manufacturable robots'])
#axes = plt.gca()
#axes.set_xlim([0,1.0])
#axes.set_ylim([0,1.0])
#axes = plt.gca()
#plt.title("No manufacturability constraints - Gen 163")
##plt.show()
#plt.savefig('WidthDepth.png')

#plt.figure(1)
#plt.xlabel('Depth')
#plt.ylabel('Height')
#plt.scatter(dat[dat[:,6] > 0,1],dat[dat[:,6] > 0,2],marker='+')
#plt.scatter(dat[dat[:,6] == 0,1],dat[dat[:,6] == 0,2],marker='x')
#plt.legend(['Manufacturable robots','Non-Manufacturable robots'])
#axes = plt.gca()
#axes.set_xlim([0,1.0])
#axes.set_ylim([0,1.0])
#axes = plt.gca()
#plt.title("No manufacturability constraints - Gen 163")
##plt.show()
#plt.savefig('DepthHeight.png')

#plt.figure(2)
#plt.xlabel('Wheels')
#plt.ylabel('Sensors')
#plt.scatter(dat[dat[:,6] > 0,3],dat[dat[:,6] > 0,4],marker='+')
#plt.scatter(dat[dat[:,6] == 0,3],dat[dat[:,6] == 0,4],marker='x')
#plt.legend(['Manufacturable robots','Non-Manufacturable robots'])
#axes = plt.gca()
#axes.set_xlim([0,1.0])
#axes.set_ylim([0,1.0])
#axes = plt.gca()
#plt.title("No manufacturability constraints - Gen 163")
##plt.show()
#plt.savefig('WheelsSensors.png')

#plt.figure(3)
#plt.xlabel('Width')
#plt.ylabel('Voxels')
#plt.scatter(dat[dat[:,6] > 0,0],dat[dat[:,6] > 0,5],marker='+')
#plt.scatter(dat[dat[:,6] == 0,0],dat[dat[:,6] == 0,5],marker='x')
#plt.legend(['Manufacturable robots','Non-Manufacturable robots'])
#axes = plt.gca()
#axes.set_xlim([0,1.0])
#axes.set_ylim([0,1.0])
#axes = plt.gca()
#plt.title("No manufacturability constraints - Gen 163")
##plt.show()
#plt.savefig('WidthVoxels.png')

## Principal component analysis
## Nice tutorial https://www.datacamp.com/community/tutorials/principal-component-analysis-in-python
#from sklearn.decomposition import PCA
#pca_novel = PCA(n_components=2)
#print(pca_novel)
#principalComponents = pca_novel.fit_transform(dat[:,0:5])
##print(principalComponents)
#print('Explained variation per principal component: {}'.format(pca_novel.explained_variance_ratio_))

#plt.figure(4)
#plt.xlabel('Principal Component 1')
#plt.ylabel('Principal Component 2')
#plt.scatter(principalComponents[dat[:,6] > 0,0],principalComponents[dat[:,6] > 0,1],marker='+')
#plt.scatter(principalComponents[dat[:,6] == 0,0],principalComponents[dat[:,6] == 0,1],marker='x')
#axes = plt.gca()
#axes.set_xlim([-1.0,1.0])
#axes.set_ylim([-1.0,1.0])
#plt.legend(['Manufacturable robots','Non-Manufacturable robots'])
#axes = plt.gca()
#plt.title("Principal Component Analysis")
##plt.show()
#plt.savefig('pca.png')
