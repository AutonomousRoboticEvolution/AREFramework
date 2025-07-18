-- Functions:
-------------------------------------------------------
function model.readInfo()
    -- Read all the data stored in the model
    
    local data=sim.readCustomStringData(model.handle,model.tagName)
    if data and #data > 0 then
        data=sim.unpackTable(data)
    else
        data={}
    end
    
    -- All the data stored in the model. Set-up default values, and remove unused values
    if not data['version'] then
        data['version']=1
    end
    
    return data
end

function model.writeInfo(data)
    -- Write all the data stored in the model. Before writing, make sure to always first read with readInfo()
    
    if data then
        sim.writeCustomStringData(model.handle,model.tagName,sim.packTable(data))
    else
        sim.writeCustomStringData(model.handle,model.tagName,'')
    end
end


-- referenced object slots (do not modify):
-------------------------------------------------------



-- Handles:
-------------------------------------------------------
model.handles={}

