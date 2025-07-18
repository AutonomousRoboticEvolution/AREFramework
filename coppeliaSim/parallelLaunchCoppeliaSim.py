import subprocess

application_path = r'path/to/coppeliaSim.exe'

for i in range(5):
    #command_line_options = "-O1 -s15000 -q -csim.createScript(sim.scripttype_simulation,'function\\nsysCall_sensing();sim=require(\"sim\");printToConsole(sim.getSimulationTime());end')" # Windowed
    command_line_options = "-h -s200000 -q -csim.createScript(sim.scripttype_simulation,'function\\nsysCall_sensing();sim=require(\"sim\");printToConsole(sim.getSimulationTime());end')" # Headless
    
    command = f'{application_path} {command_line_options}'
 
    # open in same terminal:
    #subprocess.Popen(command, shell=True) # Linux
    #subprocess.Popen(command, shell=False) # Windows
    
    # open in different terminals:
    #subprocess.Popen(['gnome-terminal', '--', 'bash', '-c', command]) # Linux
    subprocess.Popen(['start', 'cmd', '/c', command], shell=True) # Windows
    
    # Linux, 'ps -C coppeliaSim' to see running processes, 'pkill coppeliaSim' to kill all coppeliaSim processes

    # Windows, 'tasklist | findstr coppeliaSim' to see running processes, 'taskkill /IM coppeliaSim.exe /F' to kill all coppeliaSim processes
