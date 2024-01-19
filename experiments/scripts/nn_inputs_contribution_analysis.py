#! /usr/bin/python3

import os
import sys
import subprocess

def get_list_ctrl_genome_files(foldername):
  file_list = []
  for folder in os.listdir(foldername):
      for filename in os.listdir(foldername + "/" + folder):
          if(filename.split("G")[0] == "ctrl"):
              file_list.append(foldername + "/" + folder + "/" + filename)
  return file_list

def generate_nn_io(filename,step,cpg):
  folder = ""
  for path in filename.split("/")[:-1]:
      folder += path + "/"
  logfile = open(folder + "/nn_io_" + filename.split("_")[-2]+"_" +  filename.split("_")[-1],'w')
  subprocess.Popen(['/home/leni/git/nn2/build/nn_analysis',filename,step,cpg],stdout=logfile)

if __name__ == '__main__':
  file_list = get_list_ctrl_genome_files(sys.argv[1])
  print(file_list)
  for filename in file_list:
      generate_nn_io(filename,sys.argv[2],sys.argv[3])

