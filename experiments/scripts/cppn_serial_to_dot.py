import os
import sys
import subprocess as sp

for filename in os.listdir(sys.argv[1]):
    if(filename.split('_')[0] != 'morph'):
        continue
    print(sys.argv[1] + "/" + filename)
    sp.Popen(["nn2_cppn_serial_to_dot",sys.argv[1] + "/" + filename])
