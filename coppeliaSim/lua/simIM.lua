local simIM = loadPlugin 'simIM';

function simIM.numActiveHandles()
    local h = simIM.handles()
    return #h
end

-- @fun dataURL Encode image data according to "data" URL scheme (RFC 2397)
-- @arg string imgHandle Handle to the image
-- @arg {type='string',default='BMP'} format Image format (BMP, JPG, or PNG)
-- @ret string output Buffer with encoded data
function simIM.dataURL(imgHandle, fmt)
    local mime = {BMP = 'image/bmp', PNG = 'image/png', JPG = 'image/jpeg'}
    if not mime[fmt] then error('invalid format: ' .. fmt) end
    local buf = simIM.encode(imgHandle, fmt)
    return 'data:' .. mime[fmt] .. ';base64,' ..
               sim.transformBuffer(buf, sim.buffer_uint8, 1, 0, sim.buffer_base64)
end

-- @fun getMarkerBitSize Get the bit size of the specified marker dictionary
-- @arg int dictType type of marker dictionary
-- @ret int size The bit size of the markers
function simIM.getMarkerBitSize(dictType)
    if dictType == simIM.dict_type._4X4_100 then return 4 end
    if dictType == simIM.dict_type._4X4_1000 then return 4 end
    if dictType == simIM.dict_type._4X4_250 then return 4 end
    if dictType == simIM.dict_type._4X4_50 then return 4 end
    if dictType == simIM.dict_type._5X5_100 then return 5 end
    if dictType == simIM.dict_type._5X5_1000 then return 5 end
    if dictType == simIM.dict_type._5X5_250 then return 5 end
    if dictType == simIM.dict_type._5X5_50 then return 5 end
    if dictType == simIM.dict_type._6X6_100 then return 6 end
    if dictType == simIM.dict_type._6X6_1000 then return 6 end
    if dictType == simIM.dict_type._6X6_250 then return 6 end
    if dictType == simIM.dict_type._6X6_50 then return 6 end
    if dictType == simIM.dict_type._7X7_100 then return 7 end
    if dictType == simIM.dict_type._7X7_1000 then return 7 end
    if dictType == simIM.dict_type._7X7_250 then return 7 end
    if dictType == simIM.dict_type._7X7_50 then return 7 end
    if dictType == simIM.dict_type._APRILTAG_16h5 then return 4 end
    if dictType == simIM.dict_type._APRILTAG_25h9 then return 5 end
    if dictType == simIM.dict_type._APRILTAG_36h10 then return 6 end
    if dictType == simIM.dict_type._APRILTAG_36h11 then return 6 end
    if dictType == simIM.dict_type._ARUCO_ORIGINAL then return 6 end
end

(require 'simIM-typecheck')(simIM)

return simIM
