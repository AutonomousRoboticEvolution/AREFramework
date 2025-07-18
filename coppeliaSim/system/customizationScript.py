#python

def sysCall_init():
    sim = require('sim')

    # do some initialization here
    #
    # Instead of using globals, you can do e.g.:
    # self.myVariable = 21000000

def sysCall_nonSimulation():
    # is executed when simulation is not running
    pass

def sysCall_beforeSimulation():
    # is executed before a simulation starts
    pass

def sysCall_afterSimulation():
    # is executed before a simulation ends
    pass

def sysCall_cleanup():
    # do some clean-up here
    pass

# See the user manual or the available code snippets for additional callback functions and details
