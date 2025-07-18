-- put this in sysCall_actuation()
if coroutine.status(corout) ~= 'dead' then
    local ok, errorMsg = coroutine.resume(corout)
    if errorMsg then error(debug.traceback(corout, errorMsg), 2) end
end
