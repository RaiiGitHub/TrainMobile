package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua'
require 'string'
local fs = require 'file_system'

--get infomation of specific user
--url format: fs_version.php

function handle(r)
    r.content_type = "text/plain"
    if r.method == 'GET' or r.method == 'POST' then
        local base_path = fs.getBaseDataFilePath()
        local reg_table = {}
        for k, v in pairs( r:parseargs() ) do
            reg_table[k] = v
        end

        local file_path = base_path.."system.json"
        if false == fs.fileExists(file_path) then
            r:puts("{\"result\":\"file not found.\",\"code\":0,\"method\":\"version\"}")
            r.status = 405
            return apache2.OK
        end

        r:puts("{\"result\":"..fs.getFile(file_path)..",\"code\":1,\"method\":\"version\"}")

    elseif r.method == 'PUT' then
-- use our own Error contents
        r:puts("{\"result\":\"Unsupported http method.\",\"code\":0,\"method\":\"version\"}")
        r.status = 405
        return apache2.OK
    else
-- use the ErrorDocument
        return 501
    end
    return apache2.OK
end
