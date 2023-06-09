# RoboFab
Authour: Matt Hale
Readme last updated: 07/08/19

## Documentation
This can be generated by Doxygen (open a terminal in this folder, and run `doxygen`) - this will create a new folder called "Documentation" containing html and LaTeX versions. Most classes/functions have a just a brief description of what they are/do. Tell me (Matt) if anything isn't clear, I will update it!

I've included a Doxygen config file (with the default name Doxygen), but excluded the documents themselves in the .gitignore to prevent huge numbers of html files etc. on the git.

## Overall structure
The RoboFab repository contains the files needed to drive the Robot Fabricator. It is spit into several files:

- __RoboFab.py__ contains the top-level class, and will load the configuration file (configuration_BRL.json) and a blueprint, and create the other classes and tell them what to do.
- __robotComponents.py__ contains classes to describe an individual robot
- __UR5_host.py__ contains classes to control the UR5 robotic arm and the gripper
- __RoboFabComponets.py__ contains classes to describe/control various parts of the Robot Fabricator (except those in the UR5_host.py)
- __configuration_BRL.json__ contains lots of the "hard coded" settings, including positions of things and various options. At some point, we will need to make separate versions for BRL and York
- __makeConfigurationFile.py__ is not need to run the code, but is useful for generating a new json if you want to change any of the settings
