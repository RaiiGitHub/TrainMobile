local cjson = require "cjson"  
local json_tool={}

--parse and decode a json content
json_tool.decode = 
function(str)
    local json_mem = cjson.decode(str) 
    return json_mem
end

json_tool.encode = 
function(jm)
    return cjson.encode(jm)
end

return json_tool
