import time

def sysCall_init():
    print("From non-threaded Python child script, sysCall_init")
    sim = require('sim')
    simUI = require('simUI')
    xml ='<ui title="Custom UI"> <button text="Click me!" on-click="systemCallback"/> </ui>'
    global ui
    ui=simUI.create(xml)

def sysCall_actuation():
    print("From non-threaded Python child script, sysCall_actuation, sim time: {:.3f}".format(sim.getSimulationTime()))

def sysCall_sensing():
    print("From non-threaded Python child script, sysCall_sensing, sim time: {:.3f}".format(sim.getSimulationTime()))

def sysCall_cleanup():
    print("From non-threaded Python child script, sysCall_cleanup")
    simUI.destroy(ui)

def systemCallback(ui,button):
    startTime = time.time()
    print("button was clicked!")
    print("Elapsed time: {:.3f}".format(time.time() - startTime))

