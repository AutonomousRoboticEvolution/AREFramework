import csv
import numpy as np
import os

def getDirectories(repository):
    print("Getting directories...")
    # Get all directories
    FOLDERS = []
    FOLDERS_NAMES = []
    # r=root, d=directories, f = files
    for r, d, f in os.walk(repository):
        for folder in d:
            if(folder == "images"):
               continue
            temp_folder = folder.split("_")
            FOLDERS_NAMES.append(temp_folder[1])
            FOLDERS.append(os.path.join(r, folder))
    print("--- Folders: ", len(FOLDERS))
    return FOLDERS, FOLDERS_NAMES

def getDataFromFiles(folders, filename, startingColumn):
    print("Getting data from files...")
    dataFiles = []
    for f in folders:
        temp_filepath = f + '/' + filename
        temp_list = []
        if os.path.isfile(temp_filepath):
            #print ("File exist: " + temp_filepath)
            with open(temp_filepath, 'r') as csvfile:
                data = csv.reader(csvfile, delimiter=',', quotechar='"')
                for row in data:
                    temp_list.append(row[startingColumn:-1]) # Last character is empty for some reason
            dataFiles.append(temp_list)
        else:
            print ("File not exist: " + temp_filepath)
            dataFiles.append(temp_list)
    return dataFiles

def transformDataToArray(data, numDirectories):
    print("Transform data to array...")
    # Transform to array
    replicates_data = []
    for rep in range(numDirectories):
        tempDat = []
        tempDat = np.array([[float(y) for y in x] for x in data[rep]]) # Convert to array
        replicates_data.append(tempDat)
    return replicates_data

