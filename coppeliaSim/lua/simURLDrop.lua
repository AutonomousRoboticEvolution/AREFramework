local simURLDrop = loadPlugin 'simURLDrop';

function simURLDrop.rewriteURL(url)
    local p = string.split(url, '/')
    if p[1] == 'https:' and p[2] == '' and p[3] == 'github.com' and p[6] == 'blob' then
        local r = table.join(table.slice(p, 7), '/')
        return string.format('https://raw.githubusercontent.com/%s/%s/%s', p[4], p[5], r)
    end
    return url
end

(require 'simURLDrop-typecheck')(simURLDrop)

return simURLDrop
