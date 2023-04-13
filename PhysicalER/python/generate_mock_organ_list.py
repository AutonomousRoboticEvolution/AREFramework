#! /usr/bin/python3

import sys
import csv

def read_blueprint(filename):
    organ_type_list = []
    with open(filename) as file :
        csv_data = csv.reader(file,delimiter=',')
        for row in csv_data:
            organ_type_list.append(row[1])
    return organ_type_list

def print_organ_list(filename,organ_list):
    with open(filename,"access_mode") as file:
        for o in organ_list:
            if(o == "4"):
                continue
            file.write(o + ",\n")


if __name__ == "__main__":
    organ_list = read_blueprint(sys.argv[1])
    f_list = sys.argv[1].split("/")[:-1]
    id_ =  sys.argv[1].split("_")[-1]
    folder = ""
    for f in f_list:
        folder += "/" + f
    print_organ_list(folder + "/list_of_organs_" + id_ ,organ_list)

