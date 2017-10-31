package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua'
require 'string'
local fs = require 'file_system'
local ut = require 'utility_tool'
local jt = require 'json_tool'

--get infomation of specific user
--url format: fs_user_get_info.php?id=qianqian

function handle(r)
    r.content_type = "text/plain"
    if r.method == 'GET' or r.method == 'POST' then
        local base_path = fs.getBaseDataFilePath()
        local reg_table = {}
        for k, v in pairs( r:parseargs() ) do
            reg_table[k] = v
        end

        if nil == reg_table["id"] then 
            r:puts("{\"result\":\"Unsupported http method.id should be here.\",\"code\":0,\"method\":\"user_get_info\"}")
            r.status = 405
            return apache2.OK
        end

        local user_file_path = base_path.."users/user_"..reg_table["id"]..".json"
        if false == fs.fileExists(user_file_path) then
            r:puts("{\"result\":\"User was not registered yet.\",\"code\":0,\"method\":\"user_get_info\"}")
            r.status = 405
            return apache2.OK
        end

        local uele = jt.decode(fs.getFile(user_file_path))
        ut.setUserAbility(uele)

        r:puts("{\"result\":"..jt.encode(uele)..",\"code\":1,\"method\":\"user_get_info\"}")

    elseif r.method == 'PUT' then
-- use our own Error contents
        r:puts("{\"result\":\"Unsupported http method.\",\"code\":0,\"method\":\"user_get_info\"}")
        r.status = 405
        return apache2.OK
    else
-- use the ErrorDocument
        return 501
    end
    return apache2.OK
end
