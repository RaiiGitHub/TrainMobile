package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua'
require 'string'
local fs = require 'file_system'
local jt = require 'json_tool'
--respond command
--query succeed,and clear the command data.
--url format: fs_respond_command.php?id=xx&psw=xx[&clear_cmd=0/1]

function handle(r)
    r.content_type = "text/plain"
    if r.method == 'GET' or r.method == 'POST' then
        local base_path = fs.getBaseDataFilePath()
        local reg_table = {}
        for k, v in pairs( r:parseargs() ) do
            reg_table[k] = v
        end
        if nil == reg_table["id"] or nil == reg_table["psw"] then 
            r:puts("{\"result\":\"Unsupported http method.id and psw should be here.\",\"code\":0,\"method\":\"respond_command\"}")
            r.status = 405
            return apache2.OK
        end
        local clear_cmd = reg_table["clear_cmd"]
        local user_file_path = base_path.."users/user_"..reg_table["id"]..".json"
        if false == fs.fileExists(user_file_path) then
            r:puts("{\"result\":\"User was not registered yet.\",\"code\":0,\"method\":\"respond_command\"}")
            r.status = 405
            return apache2.OK
        end

        local user_info = fs.getFile(user_file_path)
        local mem = jt.decode(user_info)

        if reg_table["psw"] ~= mem["psw"] then
            r:puts("{\"result\":\"password was not correct.confirm it be right and try again.\",\"code\":0,\"method\":\"respond_command\"}") 
            r.status = 405
            return apache2.OK
        end

        local user_cmd = mem["cmd"]
        local result_code = 0
        if nil ~= user_cmd then
            result_code = 1   
            if nil ~= clear_cmd and "1" == clear_cmd then 
                mem["cmd"] = nil--remove the cmd element
            end
            fs.writeFile(user_file_path,jt.encode(mem),'w')
        else
            user_cmd = "removed."
        end
        r:puts("{\"result\":\""..user_cmd.."\",\"code\":"..result_code..",\"method\":\"respond_command\"}")

    elseif r.method == 'PUT' then
-- use our own Error contents
        r:puts("{\"result\":\"Unsupported http method.\",\"code\":0,\"method\":\"respond_command\"}")
        r.status = 405
        return apache2.OK
    else
-- use the ErrorDocument
        return 501
    end
    return apache2.OK
end
