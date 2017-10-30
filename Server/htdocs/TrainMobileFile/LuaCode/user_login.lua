package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua'
require 'string'
local fs = require 'file_system'
local jt = require 'json_tool'
local ut = require 'utility_tool'

--login verify
--url format fs_user_login.php?id=qianqian&psw=147895263
function handle(r)
    r.content_type = "text/plain"
    if r.method == 'GET' or r.method == 'POST' then
        local base_path = fs.getBaseDataFilePath()
        local reg_table = {}
        for k, v in pairs( r:parseargs() ) do
            reg_table[k] = v
        end

        if nil == reg_table["id"] or nil == reg_table["psw"] then 
            r:puts("{\"result\":\"Unsupported http method.id and psw should be here.\",\"code\":0,\"method\":\"user_login\"}")
            r.status = 405
            return apache2.OK
        end

        local user_file_path = base_path.."users/user_"..reg_table["id"]..".json"
        if false == fs.fileExists(user_file_path) then
            r:puts("{\"result\":\"User was not registered yet.\",\"code\":0,\"method\":\"user_login\"}")
            --r.status = 405
            return apache2.OK
        end

        local user_info = fs.getFile(user_file_path)
        local mem = jt.decode(user_info)
        if nil == mem["psw"] then 
            r:puts("{\"result\":\"unexpected error.this user should be registered again.\",\"code\":0,\"method\":\"user_login\"}")
            r.status = 405
            return apache2.OK
        elseif reg_table["psw"] ~= mem["psw"] then
            r:puts("{\"result\":\"password was not correct.confirm it be right and try again.\",\"code\":0,\"method\":\"user_login\"}")
            --r.status = 405
            return apache2.OK
        end

        --write login time
        local login_time = os.date("%Y-%m-%d %H:%M:%S")
        if nil == mem["login_time"] then
            mem["login_time"] = login_time
            mem["last_login_time"] = "「首次」"
        else
            mem["last_login_time"] = "「"..mem["login_time"].."」"
            mem["login_time"] = login_time
        end

        fs.writeFile(user_file_path,jt.encode(mem),'w')
        ut.setUserAbility(mem)

        --write system.json to the requester only. and do not write to the file.
        local system_file_path = base_path.."system.json"
        local system_info = jt.decode(fs.getFile(system_file_path))
        for k,v in pairs(system_info) do
            mem[k] = v
        end

        r:puts("{\"result\":"..jt.encode(mem)..",\"code\":1,\"method\":\"user_login\"}")
    elseif r.method == 'PUT' then
-- use our own Error contents
        r:puts("{\"result\":\"Unsupported HTTP method:PUT\",\"code\":0,\"method\":\"user_login\"}")
        r.status = 405
        return apache2.OK
    else
-- use the ErrorDocument
        return 501
    end
    return apache2.OK
end
