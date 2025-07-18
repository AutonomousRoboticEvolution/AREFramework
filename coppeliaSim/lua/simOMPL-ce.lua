local codeEditorInfos=[[
string stateSpaceHandle = simOMPL.createStateSpace(string name, int type, int objectHandle, float[] boundsLow, float[] boundsHigh, int useForProjection, float weight=1.0, int refObjectHandle=-1)
simOMPL.destroyStateSpace(string stateSpaceHandle)
simOMPL.setDubinsParams(string stateSpaceHandle, float turningRadius, bool isSymmetric)
string taskHandle = simOMPL.createTask(string name)
simOMPL.destroyTask(string taskHandle)
simOMPL.printTaskInfo(string taskHandle)
simOMPL.setVerboseLevel(string taskHandle, int verboseLevel)
simOMPL.setStateValidityCheckingResolution(string taskHandle, float resolution)
simOMPL.setStateSpace(string taskHandle, string[] stateSpaceHandles)
int dim = simOMPL.getStateSpaceDimension(string taskHandle)
simOMPL.setAlgorithm(string taskHandle, int algorithm)
simOMPL.setCollisionPairs(string taskHandle, int[] collisionPairHandles)
simOMPL.setStartState(string taskHandle, float[] state)
simOMPL.setGoalState(string taskHandle, float[] state)
simOMPL.addGoalState(string taskHandle, float[] state)
simOMPL.setGoal(string taskHandle, int robotDummy, int goalDummy, float tolerance=0.001, float[] metric={1.0, 1.0, 1.0, 0.1}, int refDummy=-1)
simOMPL.setup(string taskHandle)
bool solved = simOMPL.solve(string taskHandle, float maxTime)
simOMPL.simplifyPath(string taskHandle, float maxSimplificationTime=-1.0)
simOMPL.interpolatePath(string taskHandle, int stateCnt=0)
bool result = simOMPL.hasSolution(string taskHandle)
bool result = simOMPL.hasExactSolution(string taskHandle)
bool result = simOMPL.hasApproximateSolution(string taskHandle)
float distance = simOMPL.getGoalDistance(string taskHandle)
float[] states = simOMPL.getPath(string taskHandle)
float[] states, int[] tags, float[] tagsReal, int[] edges, float[] edgeWeights, int[] startVertices, int[] goalVertices = simOMPL.getPlannerData(string taskHandle)
bool solved, float[] states = simOMPL.compute(string taskHandle, float maxTime, float maxSimplificationTime=-1.0, int stateCnt=0)
float[] state = simOMPL.readState(string taskHandle)
simOMPL.writeState(string taskHandle, float[] state)
bool valid = simOMPL.isStateValid(string taskHandle, float[] state)
bool valid = simOMPL.isStateWithinBounds(string taskHandle, float[] state)
float[] state = simOMPL.enforceBounds(string taskHandle, float[] state)
float distance = simOMPL.stateDistance(string taskHandle, float[] a, float[] b)
float[] projection = simOMPL.projectStates(string taskHandle, float[] state)
simOMPL.setProjectionEvaluationCallback(string taskHandle, func callback, int projectionSize)
simOMPL.setStateValidationCallback(string taskHandle, func callback)
simOMPL.setGoalCallback(string taskHandle, func callback)
simOMPL.setValidStateSamplerCallback(string taskHandle, func callback, func callbackNear)
simOMPL.setGoalStates(string taskHandle, any[1..*] states)
int count = simOMPL.getPathStateCount(string taskHandle, float[] path)
float[] state = simOMPL.getPathState(string taskHandle, float[] path, int index)
simOMPL.getProjectedPathLength(string taskHandle, float[] path)
float[] reversedPath = simOMPL.getReversedPath(string taskHandle, float[] path)
int size = simOMPL.projectionSize(string taskHandle)
int[] dwos = simOMPL.drawPath(string taskHandle, float[] path, float lineSize, float[3] color, int extraAttributes)
int[] dwos = simOMPL.drawPlannerData(string taskHandle, float pointSize, float lineSize, float[3] color, float[3] startColor, float[3] goalColor)
simOMPL.removeDrawingObjects(string taskHandle, int[] dwos)
int ssHandle = simOMPL.createStateSpaceForJoint(string name, int jointHandle, int useForProjection=0, float weight=1)
simOMPL.setStateSpaceForJoints(string taskHandle, int[] jointHandles, int[] useForProjection={}, float[] weight={})
simOMPL.Algorithm.BiTRRT
simOMPL.Algorithm.BITstar
simOMPL.Algorithm.BKPIECE1
simOMPL.Algorithm.CForest
simOMPL.Algorithm.EST
simOMPL.Algorithm.FMT
simOMPL.Algorithm.KPIECE1
simOMPL.Algorithm.LazyPRM
simOMPL.Algorithm.LazyPRMstar
simOMPL.Algorithm.LazyRRT
simOMPL.Algorithm.LBKPIECE1
simOMPL.Algorithm.LBTRRT
simOMPL.Algorithm.PDST
simOMPL.Algorithm.PRM
simOMPL.Algorithm.PRMstar
simOMPL.Algorithm.RRT
simOMPL.Algorithm.RRTConnect
simOMPL.Algorithm.RRTstar
simOMPL.Algorithm.SBL
simOMPL.Algorithm.SPARS
simOMPL.Algorithm.SPARStwo
simOMPL.Algorithm.STRIDE
simOMPL.Algorithm.TRRT
simOMPL.StateSpaceType.position2d
simOMPL.StateSpaceType.pose2d
simOMPL.StateSpaceType.position3d
simOMPL.StateSpaceType.pose3d
simOMPL.StateSpaceType.joint_position
simOMPL.StateSpaceType.dubins
simOMPL.StateSpaceType.cyclic_joint_position
]]

registerCodeEditorInfos("simOMPL",codeEditorInfos)
