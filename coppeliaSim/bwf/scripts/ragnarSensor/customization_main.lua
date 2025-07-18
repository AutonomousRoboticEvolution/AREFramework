function model.getAvailableConveyors()
    local l=sim.getObjectsInTree(sim.handle_scene,sim.handle_all,0)
    local retL={}
    for i=1,#l,1 do
        local data=sim.readCustomStringData(l[i],simBWF.modelTags.CONVEYOR)
        if data and #data > 0 then
            retL[#retL+1]={simBWF.getObjectAltName(l[i]),l[i]}
        end
    end
    return retL
end

function model.getAvailableInputs()
    local thisInfo=model.readInfo()
    local l=sim.getObjectsInTree(sim.handle_scene,sim.handle_all,0)
    local retL={}
    for i=1,#l,1 do
        if l[i]~=model.handle then
            local data1=sim.readCustomStringData(l[i],simBWF.modelTags.RAGNARDETECTOR)
            local data2=sim.readCustomStringData(l[i],simBWF.modelTags.VISIONWINDOW)
            local data3=sim.readCustomStringData(l[i],simBWF.modelTags.RAGNARSENSOR)
            local data4=sim.readCustomStringData(l[i],simBWF.modelTags.TRACKINGWINDOW)
            local data5=sim.readCustomStringData(l[i],simBWF.modelTags.THERMOFORMER) -- has internal trigger and pallet
            if (data1 and #data1 > 0) or (data2 and #data2 > 0) or (data3 and #data3 > 0) or (data4 and #data4 > 0) or (data5 and #data5 > 0) then
                retL[#retL+1]={simBWF.getObjectAltName(l[i]),l[i]}
            end
        end
    end
    return retL
end

function sysCall_init()
    model.codeVersion=1
    
    model.dlg.init()
    
    model.handleJobConsistency(simBWF.isModelACopy_ifYesRemoveCopyTag(model.handle))
    model.updatePluginRepresentation()
end

function sysCall_nonSimulation()
    model.dlg.showOrHideDlgIfNeeded()
    model.alignCalibrationBallsWithInputAndReturnRedBall()
    model.updatePluginRepresentation()
    model.adjustSensor()
end

function sysCall_sensing()
    if model.simJustStarted then
        model.dlg.updateEnabledDisabledItems()
    end
    model.simJustStarted=nil
    model.dlg.showOrHideDlgIfNeeded()
    model.ext.outputPluginRuntimeMessages()
end

function sysCall_suspended()
    model.dlg.showOrHideDlgIfNeeded()
end

function sysCall_afterSimulation()
    model.dlg.updateEnabledDisabledItems()
end

function sysCall_beforeSimulation()
    model.simJustStarted=true
    model.ext.outputBrSetupMessages()
    model.ext.outputPluginSetupMessages()
end

function sysCall_beforeInstanceSwitch()
    model.dlg.removeDlg()
    model.removeFromPluginRepresentation()
end

function sysCall_afterInstanceSwitch()
    model.updatePluginRepresentation()
end

function sysCall_cleanup()
    model.dlg.removeDlg()
    model.removeFromPluginRepresentation()
    model.dlg.cleanup()
end
