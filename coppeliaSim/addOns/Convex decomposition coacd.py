#python

'''luaExec
function sysCall_info()
    return {autoStart = false, menu = 'Geometry / Mesh\nConvex decomposition\nCoACD...'}
end
'''

def sysCall_nonSimulation():
    if self.leaveNow:
        simUI.destroy(self.ui)
        if hasattr(self, 'params'):
            generated = []
            if len(self.sel) > 0:
                generated = generate(self.params)
            for c in self.convexSel:
                # Some shapes are already convex. Simply duplicate them:
                generated.append(sim.copyPasteObjects([c], 2|4|8|16|32)[0])
            sim.setObjectSel(generated)
            convex = True
            for c in generated:
                inf, *_ = sim.getShapeGeomInfo(c)
                if (inf & 4) == 0:
                    convex = False
            if not convex:
                sim.addLog(sim.verbosity_scripterrors, 'One or more of the generated shapes is not convex.') 
        else:
            if not self.abort:
                simUI.msgBox(simUI.msgbox_type.info, simUI.msgbox_buttons.ok, "Convex Decomposition", 'The resulting selection is effectively empty, indicating it does not contain any non-convex shapes that meet the specified inclusion criteria..')
                sim.setObjectSel({})
        return {'cmd' : 'cleanup'} 

def sysCall_init():
    sim = require('sim')
    simUI = require('simUI')
    simConvex = require('simConvex')

    self.leaveNow = False
    self.abort = False
    sel = sim.getObjectSel()
    if len(sel) == 0 or sim.getSimulationState() != sim.simulation_stopped:
        simUI.msgBox(simUI.msgbox_type.info, simUI.msgbox_buttons.ok, "Convex Decomposition", 'Make sure that at least one object is selected, and that simulation is not running.')
    else:
        missingPackages = checkPackages()
        if len(missingPackages) > 0:
            msg = 'You are missing some Python package that you should install with:\npip install ' + missingPackages
            simUI.msgBox(simUI.msgbox_type.warning, simUI.msgbox_buttons.ok, "Convex Decomposition", msg)
            sim.addLog(sim.verbosity_errors, msg)
        self.threshold = 100
        self.max_convex_hull = 101
        self.preprocess_mode = 102
        self.preprocess_resolution = 103
        self.resolution = 104
        self.mcts_nodes = 105
        self.mcts_iterations = 106
        self.mcts_max_depth = 107
        self.seed = 108
        self.pca = 109
        self.merge = 110
        self.model_shapes = 111
        self.hidden_shapes = 112
        self.adopt_colors = 113
        self.gen = 114
        self.ui = simUI.create(
          '''
          <ui title="CoACD Convex Decomposition" closeable="true" on-close="onClose" modal="true">
            <group flat="true" content-margins="0,0,0,0" layout="form">
                <label text="threshold:" />
                <spinbox id="100" minimum="0.02" maximum="1" value="0.05" step="0.01" on-change="updateUi" />
                <label text="max_convex_hull:" />
                <spinbox id="101" minimum="-1" maximum="10000" value="-1" step="10" on-change="updateUi" />
                <label text="preprocess_mode:" />
                <edit id="102" value="auto" on-change="updateUi" />
                <label text="preprocess_resolution:" />
                <spinbox id="103" minimum="1" maximum="1000" value="30" step="1" on-change="updateUi" />
                <label text="resolution:" />
                <spinbox id="104" minimum="1" maximum="10000" value="2000" step="100" on-change="updateUi" />
                <label text="mcts_nodes:" />
                <spinbox id="105" minimum="1" maximum="100" value="20" step="1" on-change="updateUi" />
                <label text="mcts_iterations:" />
                <spinbox id="106" minimum="1" maximum="1000" value="150" step="10" on-change="updateUi" />
                <label text="mcts_max_depth:" />
                <spinbox id="107" minimum="1" maximum="10" value="3" step="1" on-change="updateUi" />
                <label text="seed:" />
                <spinbox id="108" minimum="0" maximum="10000" value="0" step="1" on-change="updateUi" />
            </group>
            <checkbox id="109" text="pca" checked="false" on-change="updateUi" />
            <checkbox id="110" text="merge" checked="true" on-change="updateUi" />
            <checkbox id="111" text="include model shapes" checked="false" on-change="updateUi" />
            <checkbox id="112" text="exclude hidden shapes" checked="false" on-change="updateUi" />
            <checkbox id="113" text="adopt colors" checked="true" on-change="updateUi" />
            <button id="114" text="Generate" on-click="initGenerate" />
          </ui>
          '''
        )

def checkPackages():
    retVal = ''
    try:
        import numpy as np
    except:
        if len(retVal) != 0:
            retVal += ' '
        retVal += 'numpy'
    try:
        import coacd
    except:
        if len(retVal) != 0:
            retVal += ' '
        retVal += 'coacd'
    return retVal

def onClose(ui):
    self.leaveNow = True
    self.abort = True

def updateUi(ui, a, b):
    pass

def initGenerate(ui, a):
    includeModelShapes = simUI.getCheckboxValue(self.ui, self.model_shapes) > 0
    excludeHiddenShapes = simUI.getCheckboxValue(self.ui, self.hidden_shapes) > 0
    self.adoptColors = simUI.getCheckboxValue(self.ui, self.adopt_colors) > 0
    s = sim.getObjectSel()
    selMap = {}
    for h in s:
        if sim.getModelProperty(h) == sim.modelproperty_not_model or not includeModelShapes:
            selMap[h] = True
        else:
            tree = sim.getObjectsInTree(h, sim.sceneobject_shape)
            for t in tree:
                selMap[t] = True
    self.sel = []
    self.convexSel = []
    for obj in selMap:
        if sim.getObjectType(obj) == sim.sceneobject_shape:
            if not excludeHiddenShapes or (sim.getObjectInt32Param(obj, sim.objintparam_visible) > 0):
                t, *_ = sim.getShapeGeomInfo(obj)
                if (t & 4) == 0:
                    # not convex
                    self.sel.append(obj)
                else:
                    self.convexSel.append(obj)
    
    self.leaveNow = True
    if len(self.sel) + len(self.convexSel) > 0:
        self.params = {}

        self.params['threshold'] = float(simUI.getSpinboxValue(self.ui, self.threshold))
        self.params['max_convex_hull'] = int(simUI.getSpinboxValue(self.ui, self.max_convex_hull))
        self.params['preprocess_mode'] = simUI.getEditValue(self.ui, self.preprocess_mode)
        self.params['preprocess_resolution'] = int(simUI.getSpinboxValue(self.ui, self.preprocess_resolution))
        self.params['resolution'] = int(simUI.getSpinboxValue(self.ui, self.resolution))
        self.params['mcts_nodes'] = int(simUI.getSpinboxValue(self.ui, self.mcts_nodes))
        self.params['mcts_iterations'] = int(simUI.getSpinboxValue(self.ui, self.mcts_iterations))
        self.params['mcts_max_depth'] = int(simUI.getSpinboxValue(self.ui, self.mcts_max_depth))
        self.params['seed'] = int(simUI.getSpinboxValue(self.ui, self.seed))
        self.params['pca'] = simUI.getCheckboxValue(self.ui, self.pca) > 0
        self.params['merge'] = simUI.getCheckboxValue(self.ui, self.merge) > 0
    
def generate(params):
    generated = []
    cnt = 1
    for h in self.sel:
        vert, ind, *_ = sim.getShapeMesh(h)
        sim.addLog(sim.verbosity_scriptinfos, 'Generating CoACD convex decomposed equivalent shape ({}/{})... (input shape has {} triangular faces)'.format(cnt, len(self.sel), len(ind) / 3)) 
        nh = getConvexDecomposed(h, params, self.adoptColors)
        generated.append(nh)
        vert, ind, *_ = sim.getShapeMesh(nh)
        sim.addLog(sim.verbosity_scriptinfos, 'Done. (output shape has {} triangular faces)'.format(len(ind) / 3))
        cnt += 1
    sim.announceSceneContentChange()
    return generated
    
def extractSimpleShapes(shapes):
    retVal = []
    for shape in shapes:
        t, *_ = sim.getShapeGeomInfo(shape)
        if t & 1 != 0:
            nshapes = sim.ungroupShape(shape)
            retVal.extend(extractSimpleShapes(nshapes))
        else:
            retVal.append(shape)
    return retVal

def getConvexDecomposed(shapeHandle, params, adoptColor):
    import numpy as np
    import coacd
    allShapes = sim.copyPasteObjects({shapeHandle}, 2|4|8|16|32)
    allShapes = extractSimpleShapes(allShapes)
    newShapes = []
    for shape in allShapes:
        pose = sim.getObjectPose(shape)
        vertices, indices, normals = sim.getShapeMesh(shape)
        vertices = sim.multiplyVector(pose, vertices)
        vertices = np.array(vertices).reshape(-1, 3)
        indices = np.array(indices).reshape(-1, 3)
        mesh = coacd.Mesh(vertices, indices)
        parts = coacd.run_coacd(mesh, **params)
        for part in parts:
            vert = part[0].flatten().tolist()
            ind = part[1].flatten().tolist()
            nshape = sim.createShape(0, 0.0, vert, ind)
            
            t, *_ = sim.getShapeGeomInfo(nshape)
            if (t & 4) == 0:
                # The sub-shape is not convex somehow. Improve:
                oldS = nshape
                nshape = simConvex.hull([oldS])
                sim.removeObjects([oldS])
            sim.relocateShapeFrame(nshape, [0, 0, 0, 0, 0, 0, 0])
            if adoptColor:
                sim.setObjectColor(nshape, 0, sim.colorcomponent_ambient_diffuse, sim.getObjectColor(shape, 0, sim.colorcomponent_ambient_diffuse))
                angle = sim.getObjectFloatParam(shape, sim.shapefloatparam_shading_angle)
                sim.setObjectFloatParam(nshape, sim.shapefloatparam_shading_angle, angle)
            newShapes.append(nshape)
    sim.removeObjects(allShapes)
    if len(newShapes) > 1:
        newShape = sim.groupShapes(newShapes)
    else:
        newShape = newShapes[0]

    # Pose, BB:
    pose = sim.getObjectPose(shapeHandle)
    sim.relocateShapeFrame(newShape, pose)
    sim.alignShapeBB(newShape, [0, 0, 0, 0, 0, 0, 0])

    # Dynamic aspects:
    sim.setObjectInt32Param(newShape, sim.shapeintparam_respondable, sim.getObjectInt32Param(shapeHandle, sim.shapeintparam_respondable))
    sim.setObjectInt32Param(newShape, sim.shapeintparam_respondable_mask, sim.getObjectInt32Param(shapeHandle, sim.shapeintparam_respondable_mask))
    sim.setObjectInt32Param(newShape, sim.shapeintparam_static, sim.getObjectInt32Param(shapeHandle, sim.shapeintparam_static))
    sim.setShapeMass(newShape, sim.getShapeMass(shapeHandle))
    inertiaMatrix, com = sim.getShapeInertia(shapeHandle)
    sim.setShapeInertia(newShape, inertiaMatrix, com)
    
    # Various:
    sim.setObjectAlias(newShape, sim.getObjectAlias(shapeHandle) + '_convexDecomposed')
    
    return newShape