import zipfile as zf
import pandas as pd
import sys

if __name__ == "__main__":
    repository = sys.argv[1]
    best_filename = sys.argv[2]
    zip_archive = sys.argv[3]
    best_robots = pd.read_csv(best_filename)

    with zf.ZipFile(repository + "/" + zip_archive,'r') as zipfile:
        for replicate, id in zip(best_robots["replicate"],best_robots["robot index"]):
            folder_name = replicate.split(repository)[1]
            print("Extracting data for replicate ",replicate," robot ",id)
            try:
                zipfile.extract(folder_name + "/traj_" + str(id),path=repository)
            except:
                print(folder_name + "/traj_" + str(id) + " not found")
            try:
                zipfile.extract(folder_name + "/rollout_" + str(id),path=repository)
            except:
                print(folder_name + "/rollout_" + str(id) + " not found")