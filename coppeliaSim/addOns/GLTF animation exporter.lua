sim = require 'sim'

function sysCall_info()
    return {autoStart = false, menu = 'Exporters\nGLTF animation exporter...'}
end

function sysCall_init()
    simGLTF = require 'simGLTF'
    simUI = require 'simUI'
    if simUI.msgBox(
        simUI.msgbox_type.info, simUI.msgbox_buttons.okcancel, "GLTF Animation Export",
        'This add-on allows to record GLTF animation for a given time period. Recording will start after pressing Ok, and will end by stopping simulation or by selecting the add-on menu item again. After stopping, the location where to save the file can be selected.\n\nPress Ok to start recording, or Cancel to abort.'
    ) == simUI.msgbox_result.ok then
        sim.addLog(
            sim.verbosity_scriptinfos,
            'Recording GLTF animation... (stop the add-on to save to file)'
        )
        local fmt, fmtName = simGLTF.getExportTextureFormat()
        sim.addLog(
            sim.verbosity_scriptinfos, 'Texture export format is set to "' .. fmtName ..
                '". You can change that with simGLTF.setExportTextureFormat(format).'
        )
        simGLTF.recordAnimation(true)
    else
        return {cmd = 'cleanup'}
    end
end

function sysCall_cleanup()
    simGLTF.recordAnimation(false)
    simGLTF.clear()
end

function sysCall_addOnScriptSuspend()
    sysCall_afterSimulation()
    return {cmd = 'cleanup'}
end

function sysCall_afterSimulation()
    if simGLTF.animationFrameCount() > 0 then
        local scenePath = sim.getStringParam(sim.stringparam_scene_path)
        local sceneName = sim.getStringParam(sim.stringparam_scene_name):match("(.+)%..+")
        if sceneName == nil then sceneName = 'untitled' end
        local fileNames = simUI.fileDialog(
                             simUI.filedialog_type.save, 'Export animation to glTF...', scenePath,
                             sceneName .. '.gltf', 'glTF file', 'gltf'
                         )
        if #fileNames > 0 then
            local fileName = fileNames[1]
            simGLTF.exportAnimation()
            simGLTF.saveASCII(fileName)
            simGLTF.recordAnimation(false)
            simGLTF.clear()
            sim.addLog(sim.verbosity_scriptinfos, 'Exported GLTF animation to ' .. fileName)
        end
        return {cmd = 'cleanup'}
    end
end

function sysCall_beforeInstanceSwitch()
    return {cmd = 'cleanup'}
end
