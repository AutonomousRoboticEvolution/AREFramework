-- lua

function sysCall_init()
    sim = require('sim')

    -- Put some initialization code here
    -- sim.setStepping(true) -- enabling stepping mode
end

function sysCall_thread()
    -- Put your main code here, e.g.:
    --
    -- while true do
    --     local p = sim.getObjectPosition(objHandle)
    --     p[1] = p[1] + 0.001
    --     sim.setObjectPosition(objHandle, p)
    --     sim.step() -- resume in next simulation step (if simulation is running and stepping enabled)
    -- end
end

-- See the user manual or the available code snippets for additional callback functions and details
