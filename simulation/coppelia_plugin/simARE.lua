local simARE = loadPlugin 'simARE';

function getLogFolder(args)
    return simARE.getLogFolder(args)
end

function spawnRobot(args)
    return simARE.spawnRobot(args)
end

function sendRobotToClient(args)
    return simARE.sendRobotToClient(args)
end

function checkConnection(args)
    return simARE.checkConnection(args)
end

return simARE

