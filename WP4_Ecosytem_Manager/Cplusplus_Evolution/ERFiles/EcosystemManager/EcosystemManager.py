from tkinter import *
import subprocess
import sys, string, os

window = Tk()
window.title("ARE Ecosystem Manager")
lbl = Label(window,text="Minimal Ecosystem Manager")
lbl.grid(column=0, row =0)
con = Label(window,text="Output")
con.grid(column=0, row =6)
repLab = Label(window,text="C:/Program Files/V-REP3/V-REP_PRO_EDU/")
repLab.grid(column=0, row = 5)
window.geometry('800x600')
repository=Entry(window,width=100)
repository.grid(column=0,row=4)
repository.configure(text="C:/Program Files/V-REP3/V-REP_PRO_EDU/")

def clickRun():
	con.configure(text="Starting Evolution")
	repository.configure(text="C:/Program Files/V-REP3/V-REP_PRO_EDU/")
	subprocess.Popen([r"C:/Program Files/V-REP3/V-REP_PRO_EDU/vrep.exe", "-g0", "-g2", "-gfiles"])
	# file = 'C:\\Program Files\\V-REP3\\V-REP_PRO_EDU\\vrep.exe'
	# os.system('"'+ file + '"' + ' -g10 - g2 -gfiles')

def clickRecallBest():
	con.configure(text="RECALLING BEST")
	repository.configure(text="C:/Program Files/V-REP3/V-REP_PRO_EDU/")
	subprocess.Popen([r"C:/Program Files/V-REP3/V-REP_PRO_EDU/vrep.exe", "-g0", "-g9", "-gfiles"])
	
def clickRunParallel():
	con.configure(text="Starting Parallel Evolution")
	repository.configure(text="C:/Program Files/V-REP3/V-REP_PRO_EDU/")
	for i in range(7):
		#arguments = "-h -g" + str(i) + " -g2 -gfiles -gREMOTEAPISERVERSERVICE_"+ str(i + 1) +"_FALSE_FALSE"
		subprocess.Popen([r"C:/Program Files/V-REP3/V-REP_PRO_EDU/vrep.exe" ,"-h","-g0" ,"-g1" ,"-gfiles", "-gREMOTEAPISERVERSERVICE_"+str(i+104000)+"_TRUE_TRUE"])
				   #-gREMOTEAPISERVERSERVICE_"+ str(i+1040000) + "_FALSE_FALSE"])

runB = Button(window, text="Run Evolution", command=clickRun)
recallB = Button(window, text="Recall Best", command=clickRecallBest)
runPB = Button(window, text="Run Parallel", command=clickRunParallel)
runB.grid(column=0, row=1);
recallB.grid(column=0, row=2);
runPB.grid(column=0,row=3);


window.mainloop()
