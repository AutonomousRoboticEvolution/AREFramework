local simMIDI = loadPlugin 'simMIDI';
(require 'simMIDI-typecheck')(simMIDI)

--@fun processIncomingMessages Process incoming messages on given input port, on the specified channels (or all channels) passing the data to the handlers specified in the third argument.
--@arg string midiInPortHandle handle of a MIDI input device
--@arg table.int channels a list of MIDI channels, or if nil all channels from 1 to 16 will be used
--@arg table funcs a map of functions, e.g. {onNoteOn = function(note, velocity) ... end, onNoteOff = function(note, velocity) ... end, onPolyAftertouch = function(note, value) ... end, onControlChange = function(cc, value) ... end, onChannelAftertouch = function(v) ... end, onPitchBend = function(p) ... end, onMessage = function(m1, m2, m3) ... end} all keys are optional
function simMIDI.processIncomingMessages(midiInPortHandle, channels, funcs)
    if channels == nil then
        channels = range(16)
    end
    local channelMap = {}
    for _, channel in ipairs(channels) do channelMap[channel] = true end

    local function dispatch(fn, ...)
        if type(funcs[fn]) == 'function' then
            funcs[fn](...)
        end
    end

    while true do
        local m = simMIDI.getMessage(midiInPortHandle)
        if #m == 0 then return end
        if #m == 3 then
            local m1, m2, m3 = table.unpack(m)
            local chan = 1 + (m1 & 0x0F)
            if channelMap[chan] then
                dispatch('onMessage', m1, m2, m3)
                m1 = m1 & 0xF0
                if m1 == 0x80 then
                    dispatch('onNoteOff', m2, m3)
                    dispatch('onNoteOff' .. m2, m3)
                elseif m1 == 0x90 then
                    dispatch('onNoteOn', m2, m3)
                    dispatch('onNoteOn' .. m2, m3)
                elseif m1 == 0xA0 then
                    dispatch('onPolyAftertouch', m2, m3)
                    dispatch('onPolyAftertouch' .. m2, m3)
                elseif m1 == 0xB0 then
                    dispatch('onControlChange', m2, m3)
                    dispatch('onControlChange' .. m2, m3)
                elseif m1 == 0xC0 then
                    dispatch('onProgramChange', m2)
                    dispatch('onProgramChange' .. m2)
                elseif m1 == 0xD0 then
                    dispatch('onChannelAftertouch', m2)
                elseif m1 == 0xE0 then
                    dispatch('onPitchBend', m3 << 7 | m2)
                end
            end
        end
    end
end

return simMIDI
