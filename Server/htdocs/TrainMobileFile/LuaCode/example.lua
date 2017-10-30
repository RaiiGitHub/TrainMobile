package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua'
require 'string'
local fs = require 'file_system'
local jt = require 'json_tool'
--[[
     This is the default method name for Lua handlers, see the optional
     function-name in the LuaMapHandler directive to choose a different
     entry point.
--]]
function handle(r)
    r.content_type = "text/plain"
    local base_path = fs.getBaseDataFilePath()
    local user_list_file_path = base_path.."users"
    if r.method == 'GET' or r.method == 'POST' then
        r:puts("Hello Lua World!\n")
        local all_users = fs.fileList(user_list_file_path)
        for index, user_file in pairs(all_users) do   
            r:puts(index.."->\n")   
            local json_mem = jt.parse(fs.getFile(user_file))
            for key, value in pairs(json_mem) do      
                r:puts(key)
                r:puts(":")
                r:puts(value)
                r:puts("\n") 
            end  
        end
        local fp = fs.getFilePathFromList(all_users,[[user_amosyq]])
        r:puts("\nfind best path->"..fp)
        r:puts("\n15652318757.json:")
        r:puts(fs.fileExists(base_path.."15652318757.json") and "file exist." or "file not exist.")
        r:puts("\nusers.json:")
        r:puts(fs.fileExists(base_path.."users.json") and "file exist." or "file not exist.")

        for k, v in pairs( r:parseargs() ) do
            r:puts( string.format("%s: %s\n", k, v) )
        end
    elseif r.method == 'PUT' then
-- use our own Error contents
        r:puts("Unsupported HTTP method " .. r.method)
        r.status = 405
        return apache2.OK
    else
-- use the ErrorDocument
        return 501
    end
    return apache2.OK
end

