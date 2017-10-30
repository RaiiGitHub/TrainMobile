package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua'
require 'string'
local fs = require 'file_system'

--deleting files.
--url format: fs_del_file.php

function handle(r)
    r.content_type = "text/plain"
    if r.method == 'GET' or r.method == 'POST' then
        local base_path = fs.getBaseProjectFilePath()
        local bin_path = fs.getBaseBinFilePath()
        local reg_table = {}
        for k, v in pairs( r:parseargs() ) do
            reg_table[k] = v
        end
        
        if nil == reg_table["file_path"] and nil == reg_table["bin_path"] then 
            r:puts("{\"result\":\"parameter file_path or bin_path is wanted.\",\"code\":0,\"method\":\"del_file\"}")
        end
        if nil ~= reg_table["file_path"] then
            fs.delFile(base_path..reg_table["file_path"])
        elseif nil ~= reg_table["bin_path"] then
            fs.delFile(bin_path..reg_table["bin_path"])
        end
        r:puts("{\"result\":\"file has been deleted.\",\"code\":1,\"method\":\"del_file\"}")

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
