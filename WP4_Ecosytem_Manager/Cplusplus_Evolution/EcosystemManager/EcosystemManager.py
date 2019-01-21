from tkinter import *
import subprocess
import sys, string, os

directory = os.getcwd()
if (os.name == 'nt'):
	print("Running windows")
	directory = "C:/Program Files/V-REP3/V-REP_PRO_EDU"
else:
	print("Not running windows")

print("directory = " + directory)

window = Tk()
window.title("ARE Ecosystem Manager")
lbl = Label(window,text="Minimal Ecosystem Manager")
lbl.grid(column=0, row =0)
con = Label(window,text="Output")
con.grid(column=0, row =6)
repLab = Label(window,text=directory)
repLab.grid(column=0, row = 5)
window.geometry('800x600')
repository=Entry(window,width=100)
repository.grid(column=0,row=4)
repository.configure(text=directory)

def clickRun():
	con.configure(text="Starting Evolution")
	repository.configure(text=directory)
	if (os.name == 'nt'):
		subprocess.Popen([r""+ directory + "/vrep.exe", "-g0", "-g2", "-gfiles"])
	else:
		subprocess.Popen([r""+ directory + "/vrep.sh", "-g0", "-g2", "-gfiles"])
	# file = 'C:\\Program Files\\V-REP3\\V-REP_PRO_EDU\\vrep.exe'
	# os.system('"'+ file + '"' + ' -g10 - g2 -gfiles')

def clickRecallBest():
	con.configure(text="RECALLING BEST")
	repository.configure(text=directory)
	if (os.name == 'nt'):
		subprocess.Popen([r""+ directory + "/vrep.exe", "-g0", "-g9", "-gfiles"])
	else:
		subprocess.Popen([r""+ directory + "/vrep.sh", "-g0", "-g9", "-gfiles"])
	
def clickRunParallel():
	con.configure(text="Starting Parallel Evolution")
	repository.configure(text=directory)
	for i in range(7):
		#arguments = "-h -g" + str(i) + " -g2 -gfiles -gREMOTEAPISERVERSERVICE_"+ str(i + 1) +"_FALSE_FALSE"
		if (os.name == 'nt'):
			subprocess.Popen([r""+directory+"/vrep.exe" ,"-h","-g0" ,"-g1" ,"-gfiles", "-gREMOTEAPISERVERSERVICE_"+str(i+104000)+"_TRUE_TRUE"])
		else:
			subprocess.Popen([r""+directory+"/vrep.sh" ,"-h","-g0" ,"-g1" ,"-gfiles", "-gREMOTEAPISERVERSERVICE_"+str(i+104000)+"_TRUE_TRUE"])
				   #-gREMOTEAPISERVERSERVICE_"+ str(i+1040000) + "_FALSE_FALSE"])
	if (os.name == 'nt'):
		subprocess.Popen([r""+directory+"/programming/v_repExtER_Minimal/x64/Debug/ERClient.exe",directory+"/files" ,"0" ,"7"])
	else:
		subprocess.Popen([r""+directory+"/programming/Cplusplus_Evolution/ERClient/ERClient", directory+"/files" ,"0" ,"7"])
		

runB = Button(window, text="Run Evolution", command=clickRun)
recallB = Button(window, text="Recall Best", command=clickRecallBest)
runPB = Button(window, text="Run Parallel", command=clickRunParallel)
runB.grid(column=0, row=1);
recallB.grid(column=0, row=2);
runPB.grid(column=0,row=3);


window.mainloop()
