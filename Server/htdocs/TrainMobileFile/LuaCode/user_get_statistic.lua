package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua'
require 'string'
local fs = require 'file_system'
local jt = require 'json_tool'
--get statistics of all users
--url format: fs_user_get_statistic.php?
function handle(r)
    r.content_type = "text/plain"
    if r.method == 'GET' or r.method == 'POST' then
        local user_statistic = {}
        local base_path = fs.getBaseDataFilePath()
        local reg_table = {}
        for k, v in pairs( r:parseargs() ) do
            reg_table[k] = v
        end

        local user_files = fs.fileList(base_path.."users")
        user_statistic["highest_exp"] = "0"
        user_statistic["highest_index"] = "0"
        user_statistic["users_base_info"] = {}

        for i,v in pairs( user_files ) do
            if true == fs.fileExists(v) then 
                local u_info = jt.decode(fs.getFile(v))
                local u_info_reserved = {}
                for u_info_k,u_info_v in pairs( u_info ) do
                    if u_info_k ~= "psw" then
                        u_info_reserved[u_info_k] = u_info_v
                    end
                    if u_info_k == "exp" then
                        if tonumber(user_statistic["highest_exp"]) < tonumber(u_info_reserved[u_info_k]) then
                            user_statistic["highest_exp"] = u_info_reserved[u_info_k]
                            if nil ~= u_info["index"] then
                                user_statistic["highest_index"] = u_info["index"]
                            end
                        end
                    end
                end
                table.insert(user_statistic["users_base_info"],u_info_reserved)
            end
        end
        user_statistic["user_count"] = #user_statistic["users_base_info"]
        r:puts("{\"result\":"..jt.encode(user_statistic)..",\"code:\":1,\"method\":\"user_get_statistic\"}")
    elseif r.method == 'PUT' then
-- use our own Error contents
        r:puts("{\"result\":\"Unsupported http method.\",\"code\":0,\"method\":\"user_get_statistic\"}")
        r.status = 405
        return apache2.OK
    else
-- use the ErrorDocument
        return 501
    end
    return apache2.OK
end
