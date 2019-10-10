## some useful functions that have utility in many places, so it makes sense to have them centrally accessible

from subprocess import Popen # for reading out messages
import math, serial
import numpy as np

DEBUG_PRINT = True
DEBUG_PRINT_VERBOSITY_LEVEL = 999 #higher value will mean more stuff gets spewed to command line.
READ_OUT_VERBOSITY_LEVEL = -1 #higher value will mean more stuff gets read out by the computerised voice

## Prints its input string.
# messageVerbosity sets the importance of this message
# 0 - very high level, e.g. "attaching next organ"
# ...
# 2 - very low level, e.g. raw messages and numbers
def debugPrint ( msg , messageVerbosity = 0):

    if DEBUG_PRINT_VERBOSITY_LEVEL >= messageVerbosity:
        for i in range(messageVerbosity): # indents the more low-level messages to improve readability
            msg="\t"+msg
        print ( msg )
    if READ_OUT_VERBOSITY_LEVEL >= messageVerbosity:
        cmd = 'espeak "'+msg+'"'
        Popen ( cmd, shell=True )


## helper function to make a coordinate transform from more human-friendly input.
#
#  Input should be a list of numbers which can be [x,y,z] to make a linear transform with no rotation
#   OR
#  [x,y,z,rx,ry,rz] where the rotations are angles IN RADIANS around Euler angles, applied in ZYX order.
#  This is different from convertPoseToTransform, which expects the rotation to be a rotation vector.
def makeTransform ( inputValues=None ):
    if inputValues is None:
        inputValues = [ 0.0, 0.0, 0.0 ]
    if len ( inputValues ) == 3:
        transformOut = np.matrix ( [ [ 1, 0, 0, inputValues [ 0 ] ],
                                     [ 0, 1, 0, inputValues [ 1 ] ],
                                     [ 0, 0, 1, inputValues [ 2 ] ],
                                     [ 0, 0, 0, 1 ] ] )
    elif len ( inputValues ) == 6:
        # see: https://en.wikipedia.org/wiki/Rotation_matrix#General_rotations
        # and: https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        # First compute the rotation matrix for each rotation in turn
        rx, ry, rz = inputValues [ 3: ]

        c1 = math.cos ( rz )
        s1 = math.sin ( rz )
        c2 = math.cos ( ry )
        s2 = math.sin ( ry )
        c3 = math.cos ( rx )
        s3 = math.sin ( rx )
        #just rotations
        rotationMatrix = np.matrix ( [ [ c1 * c2, c1 * s2 * s3 - c3 * s1, s1 * s3 + c1 * c3 * s2 ],
                                       [ c2 * s1, c1 * c3 + s1 * s2 * s3, c3 * s1 * s2 - c1 * s3 ],
                                       [ -s2, c2 * s3, c2 * c3 ] ] )
        #adds in transform
        transformOut = np.column_stack ( (rotationMatrix, np.transpose (
            np.matrix ( inputValues [ 0:3 ] ) )) )  # add the linear part of the transform to make the right column
        #finally scaling row
        transformOut = np.row_stack (
            (transformOut, np.matrix ( [ 0, 0, 0, 1 ] )) )  # append 0,0,0,1 to make the bottom row
    else:
        raise ValueError ( "inputValues is wrong length. Should either be [x,y,z] or [x,y,z,rx,ry,rz]." )
    return transformOut


## convert from a rotation vector style pose (ie the type understood by the UR5 polyscope program) into a transformation matrix.
def convertPoseToTransform ( inputPose ):
    if np.shape ( inputPose ) == (1, 6):
        inputPose = inputPose [ 0 ]
    if not np.shape ( inputPose ) == (6,):
        raise ValueError ( "Input pose should be list of 6, was " + str ( inputPose ) )
    # see: https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
    rotationVector = np.array ( inputPose [ 3:6 ] )
    theta = np.linalg.norm ( rotationVector )  # magnitude of rotation
    if theta == 0:  # no rotation
        return makeTransform ( inputPose [ 0:3 ] )
    else:
        unitRotationVector = rotationVector / theta
        # matching notation from https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula:
        kx = unitRotationVector [ 0 ]
        ky = unitRotationVector [ 1 ]
        kz = unitRotationVector [ 2 ]
        K = np.matrix ( [ [ 0, -kz, ky ], [ kz, 0, -kx ], [ -ky, kx, 0 ] ] )
        rotationMatrix = np.eye ( 3 ) + (math.sin ( theta )) * K + (1 - math.cos ( theta )) * (K ** 2)

        transformOut = np.column_stack ( (rotationMatrix, np.transpose (
            np.matrix ( inputPose [ 0:3 ] ) )) )  # add the linear part of the transform to make the right column
        transformOut = np.row_stack (
            (transformOut, np.matrix ( [ 0, 0, 0, 1 ] )) )  # append 0,0,0,1 to make the bottom row
        return np.matrix(transformOut)


## convert from a transformation matrix to a pose suitable for sending to the UR
def convertTransformToPose ( inputTransform ):
    if not np.shape ( inputTransform ) == (4, 4):
        raise ValueError ( "input Transform not (4,4) in size" )
    # see: https://en.wikipedia.org/wiki/Rotation_matrix#Conversion_from_and_to_axis%E2%80%93angle
    rotationMatrix = inputTransform [ 0:3, 0:3 ]
    eigenvalues, eigenvectors = np.linalg.eig ( rotationMatrix )

    # the direction of the vector is the eigenvector corresponding to an eigenvalue of 1:
    from operator import itemgetter
    indexOfEigenvalueClosestToOne = \
        min ( enumerate ( [ abs ( value - 1 ) for value in eigenvalues ] ), key=itemgetter ( 1 ) ) [ 0 ]
    directionVector = np.reshape ( eigenvectors [ 0:3, indexOfEigenvalueClosestToOne ], (3, 1) )

    # the magnitude of the rotation is found from knowing that trace(rotationMatrix) = 1 + 2cos(theta)
    magnitude = math.acos ( (np.trace ( rotationMatrix ) - 1) / 2 )

    # we must be careful to select the right direction for the rotation, i.e. it could be plus or minus magnitude.
    # To do this, try each one and pick the one with smaller error when we convert back to transform and compare to the original.
    # This is a bit of a fudge but there's only two possibilities to try so it's not too bad.
    # It also gives a good sense check to ensure we've done it right!
    poseTranslationPart = np.reshape ( np.transpose ( inputTransform [ 0:3, 3 ] ),
                                       (1, 3) )  # this is the same in either case

    sign = [ -1, 1 ]
    poseToTest = [ 0, 0 ]
    sumOfSquaresError = [ 0, 0 ]
    for i in [ 0, 1 ]:
        poseRotationPart = np.reshape ( np.transpose ( np.real ( directionVector * sign [ i ] * magnitude ) ) [
                                            0 ], (1, 3) )  # a rotation vector
        # format into a list of floats as expected for a pose, [x,y,z,rx,ry,rz]
        poseToTest [ i ] = np.concatenate ( (poseTranslationPart,
                                             poseRotationPart), axis=1 ) [ 0 ].tolist ()
        sumOfSquaresError [ i ] = np.sum (
            np.square ( inputTransform - convertPoseToTransform ( poseToTest [ i ] ) ) )
    if sumOfSquaresError [ 0 ] < sumOfSquaresError [ 1 ]:
        outputPose = poseToTest [ 0 ]
    else:
        outputPose = poseToTest [ 1 ]

    # check for a suspiciously large error:
    if min ( sumOfSquaresError ) > 0.01:
        print ( "Error: " + str ( min ( sumOfSquaresError ) ) )
        print( inputTransform )
        print ( convertPoseToTransform ( outputPose ) )
        raise ValueError ( "No pose found that corresponds back to the right transform: " + str ( inputTransform ) )

    if np.shape ( outputPose ) == (1, 6):
        outputPose = outputPose [ 0 ]

    return outputPose


## creates a connection to an arduino.
# will try various ports, until it finds one where the "expectedMessage" is heard from the arduino at the other side.
# returns the serial object corresponding to this arduino.
def connect2USB ( expectedMessage ):
    debugPrint ( "Starting a USB connection, looking for: "+expectedMessage, messageVerbosity=1  )
    isConnected = False
    retries = 0
    port = '/dev/ttyUSB'
    # tries different usb post to connect to assembly stand
    while not isConnected:
        for i_port in range ( 10 ):
            debugPrint ( "trying " + port + str ( i_port ) )
            try:
                arduino = serial.Serial ( port + str ( i_port ), 9600, timeout=2 )
                in_string = str ( arduino.readline () )
                # confirmation of assembly fixture
                if expectedMessage in in_string:
                    isConnected = True
                    debugPrint ( "Arduino connected at: " + arduino.name , messageVerbosity=1 )
                    break
                else:
                    debugPrint ( in_string )
                    debugPrint ( "Arduino rejected from: " + arduino.name , messageVerbosity=2 )
                    arduino.close ()  # this is not the port you're looking for

            except serial.SerialException:
                pass
        if not isConnected:
            # failed to connect error
            retries += 1
            debugPrint ( "Connection reties: " + str ( retries ) )
            if retries > 10:
                raise serial.SerialException ( 'Cannot find USB' )
    return arduino

## computes the angle between the given axis of the two Transformation matrices A and B, returned in radians
## Axis should be "x", "y" or "z"
## if axis="x", will return the angle BETWEEN the x axis in A nad the x axis in B, NOT the rotation about that axis
def findAngleBetweenTransforms(A , B=makeTransform() , axis="x"):
    # returns the unit vector in the direction of the input
    # the vectors in a Transformation matrix should already be unit vectors, but it's good to make sure
    def make_unit_vector(v):
        return v / np.linalg.norm(v)


    switcher = { 'x': 0, 'y':1, 'z':2}
    columnToConsider = switcher[axis]

    vectorA = make_unit_vector(A[0:3,columnToConsider])
    vectorB = make_unit_vector(B[0:3,columnToConsider])

    dotProduct = np.dot(vectorA.transpose(), vectorB)
    return float( np.arccos( np.clip ( dotProduct , -1.0,1.0 )) )