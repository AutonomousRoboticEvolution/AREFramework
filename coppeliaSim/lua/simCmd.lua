local simCmd = loadPlugin 'simCmd';

function simCmd.helpText()
    local txt = [[## Commander plugin

Use this plugin for quick evaluation of expressions.

### Completion

]]
    if sim.getBoolParam(sim.boolparam_headless) then
        txt = txt ..
                  [[Begin to type the name of a function (e.g. "sim.getObjectHa") and press TAB to complete it.

]]
    else
        txt = txt ..
                  [[Begin to type the name of a function (e.g. "sim.getObjectHa") and press TAB to automatically complete it. If there are multiple matches, repeatedly press TAB to cycle through completions, and Shift+TAB to cycle back.

### Keyboard Shortcuts

- **Ctrl+Alt+C**: focus the text input.
- **TAB**: complete symbol / cycle to next completion.
- **Shift+TAB**: cycle to previous completion.
- **Enter**: accept completion (also works with '(' and '.').
- **Esc**: clear text field.
- **Up/Down** arrows: navigate/search through command history.
- **Ctrl+L**: clear statusbar.

]]
    end
    if false then
        txt = txt .. [[### String Rendering Flags

There are some flags that control how the results are displayed. Those are input by adding a comment at the end of the line, containing as comma separated list of key=value pairs, e.g.: "luaExpression --flag1=10,flag2=test". Flags can be abbreviated by typing only the initial part, e.g. "pre" instead of "precision", down to any length, provided it is not ambiguous.

- **depth**: (int) limit the maximum depth when rendering a map-table.
- **precision**: (int) number of floating point digits.
- **retvals**: (1, \*) print all the returned values (\*) or only the first (1).
- **sort**: (k, t, tk, off) how to sort map-table entries:
    - k: sort by keys;
    - t: sort by type;
    - tk: sort by type, then by key;
    - off: don't sort at all.
- **escape**: (0/1) enable/disable special character escaping.

]]
    end
    txt = txt .. [[### Special variables

Some special variables are set automatically before each evaluation:

- **H**: (function) alias for sim.getObject, e.g. H'/foo' as a shortcut for sim.getObject('/foo'); it also accepts TAB-completion.
- **SEL**: (table) the current object selection, i.e. a list of handles.
- **SEL1**: (int) the last selected object, i.e. the last element of SEL.


### Misc. utility functions

- **apropos**: use this to search in the API symbols (functions and constants), e.g. apropos'inertia'.
- **printBytes**: print a binary string in hexadecimal bytes, like hexdump.

]]
    return txt
end

function simCmd.help()
    if sim.getBoolParam(sim.boolparam_headless) then
        print(string.stripmarkdown(simCmd.helpText()))
    else
        simUI = require 'simUI'
        if not ui then
            ui = simUI.create(
                     '<ui title="Commander Plugin" closeable="true" on-close="simCmd.helpClose" modal="true" size="440,520"><text-browser text="' ..
                         string.gsub(simCmd.helpText(), '"', '&quot;') ..
                         '" type="markdown" /></ui>'
                 )
        end
    end
end

function simCmd.helpClose()
    if ui then
        simUI.destroy(ui)
        ui = nil
    end
end
(require 'simCmd-typecheck')(simCmd)

return simCmd
