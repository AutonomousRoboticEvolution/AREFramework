The various source code items can be found on https://github.com/CoppeliaRobotics
Clone each required repository with:

git clone --recursive https://github.com/CoppeliaRobotics/repositoryName

Use following directory structure:

coppeliaRobotics
    |__ coppeliaSimLib (CoppeliaSim main library)
    |__ programming
                  |__ include
                  |__ coppeliaSimClient
                  |__ coppeliaSimClientPython
                  |__ coppeliaGeometricRoutines
                  |__ coppeliaKinematicsRoutines
                  |__ simGeom
                  |__ simIK
                  |__ simDyn
                  |__ zmqRemoteApi
                  |__ wsRemoteApi
                  |__ simCodeEditor
                  |__ simExtJoystick
                  |__ simExtCam
                  |__ simURDF
                  |__ simSDF
                  |__ simRuckig
                  |__ simRRS1
                  |__ simMTB
                  |__ simUI
                  |__ simOMPL
                  |__ simICP
                  |__ simSurfRec
                  |__ simCmd
                  |__ simSkeleton
                  |__ simSkel
                  |__ simCHAI3D
                  |__ simConvex
                  |__ simPovRay
                  |__ simQML
                  |__ simVision
                  |__ simIM
                  |__ simIGL
                  |__ simBubble
                  |__ simAssimp
                  |__ simOpenMesh
                  |__ simOpenGL3
                  |__ simGLTF
                  |__ simZMQ
                  |__ simURLDrop
                  |__ simSubprocess
                  |__ simEigen
                  |__ simMIDI
                  |__ simEvents
                  |__ simLDraw
                  |__ bubbleRobServer
                  |__ bubbleRobZmqServer
                  |__ configUi-2
                  |__ rcsServer
                  |__ mtbServer
                  |
                  |__ ros_packages
                  |            |__ simROS
                  |            |__ ros_bubble_rob
                  |
                  |__ ros2_packages
                               |__ simROS2
                               |__ ros2_bubble_rob
                               

           
Following are the main Items:
-----------------------------

-   'coppeliaSimLib' (requires 'include'):         
    https://github.com/CoppeliaRobotics/coppeliaSimLib

-   'coppeliaSimClient' (requires 'include'):
    https://github.com/CoppeliaRobotics/coppeliaSimClient

-   'coppeliaSimClientPython' (requires 'include'):
    https://github.com/CoppeliaRobotics/coppeliaSimClientPython


Various common items:
---------------------

-   'include'
    https://github.com/CoppeliaRobotics/include

-   'zmqRemoteApi' (requires 'include'): 
    https://github.com/CoppeliaRobotics/zmqRemoteApi

-   'wsRemoteApi' (requires 'include'):
    https://github.com/CoppeliaRobotics/wsRemoteApi

-   'coppeliaGeometricRoutines' (requires 'include'):
    https://github.com/CoppeliaRobotics/coppeliaGeometricRoutines

-   'coppeliaKinematicsRoutines' (requires 'include'):
    https://github.com/CoppeliaRobotics/coppeliaKinematicsRoutines
    
Major plugins:
--------------

-   'simDyn' (requires 'include' + various physics engine dependencies):
    https://github.com/CoppeliaRobotics/simDyn

-   'simGeom' (requires 'include' and 'coppeliaGeometricRoutines'):
    https://github.com/CoppeliaRobotics/simGeom

-   'simIK' (requires 'include' and 'coppeliaKinematicsRoutines'):
    https://github.com/CoppeliaRobotics/simIK

-   'simCodeEditor' (requires 'include' and 'QScintilla'):
    https://github.com/CoppeliaRobotics/simCodeEditor


Various plugins:
----------------

-   'simExtJoystick' (requires 'include'):
    https://github.com/CoppeliaRobotics/simExtJoystick (Windows only)

-   'simExtCam' (requires 'include'):
    https://github.com/CoppeliaRobotics/simExtCam (Windows only)

-   'simURDF' (requires 'include'):
    https://github.com/CoppeliaRobotics/simURDF

-   'simSDF' (requires 'include'):
    https://github.com/CoppeliaRobotics/simSDF

-   'simRuckig' (requires 'include'):
    https://github.com/CoppeliaRobotics/simRuckig

-   'simRRS1' (requires 'include'):
    https://github.com/CoppeliaRobotics/simRRS1

-   'simMTB' (requires 'include'):
    https://github.com/CoppeliaRobotics/simMTB

-   'simUI' (requires 'include'):
    https://github.com/CoppeliaRobotics/simUI

-   'simOMPL' (requires 'include'):
    https://github.com/CoppeliaRobotics/simOMPL

-   'simICP' (requires 'include'):
    https://github.com/CoppeliaRobotics/simICP

-   'simIGL' (requires 'include'):
    https://github.com/CoppeliaRobotics/simIGL

-   'simSurfRec' (requires 'include'):
    https://github.com/CoppeliaRobotics/simSurfRec

-   'simQML' (requires 'include'):
    https://github.com/CoppeliaRobotics/simQML

-   'simROS' (requires 'include'):
    https://github.com/CoppeliaRobotics/simROS

-   'simROS2' (requires 'include'):
    https://github.com/CoppeliaRobotics/simROS2

-   'simCmd' (requires 'include'):
    https://github.com/CoppeliaRobotics/simCmd

-   'simCHAI3D' (requires 'include'):
    https://github.com/CoppeliaRobotics/simCHAI3D

-   'simConvex' (requires 'include'):
    https://github.com/CoppeliaRobotics/simConvex

-   'simPovRay' (requires 'include'):
    https://github.com/CoppeliaRobotics/simPovRay

-   'simOpenMesh' (requires 'include'):
    https://github.com/CoppeliaRobotics/simOpenMesh

-   'simVision' (requires 'include'):
    https://github.com/CoppeliaRobotics/simVision

-   'simIM' (requires 'include'):
    https://github.com/CoppeliaRobotics/simIM

-   'simBubble' (requires 'include'):
    https://github.com/CoppeliaRobotics/simBubble

-   'simOpenGL3' (requires 'include'):
    https://github.com/CoppeliaRobotics/simOpenGL3

-   'simGLTF' (requires 'include'):
    https://github.com/CoppeliaRobotics/simGLTF

-   'simZMQ' (requires 'include'):
    https://github.com/CoppeliaRobotics/simZMQ

-   'simURLDrop' (requires 'include'):
    https://github.com/CoppeliaRobotics/simURLDrop

-   'simSubprocess' (requires 'include' and Qt):
    https://github.com/CoppeliaRobotics/simSubprocess

-   'simEigen' (requires 'include' and Eigen):
    https://github.com/CoppeliaRobotics/simEigen

Various other repositories:		
---------------------------

-   'bubbleRobServer' (requires 'include'):
    https://github.com/CoppeliaRobotics/bubbleRobServer
    
-   'bubbleRobZmqServer' (requires 'include'):
    https://github.com/CoppeliaRobotics/bubbleRobZmqServer
    
-   'rcsServer' (requires 'include'):
    https://github.com/CoppeliaRobotics/rcsServer

-   'mtbServer' (requires 'include'):
    https://github.com/CoppeliaRobotics/mtbServer

-   'ros_bubble_rob' (requires 'include'):
    https://github.com/CoppeliaRobotics/ros_bubble_rob

-   'ros2_bubble_rob' (requires 'include'):
    https://github.com/CoppeliaRobotics/ros2_bubble_rob

-   'PyRep':
    https://github.com/stepjam/PyRep
