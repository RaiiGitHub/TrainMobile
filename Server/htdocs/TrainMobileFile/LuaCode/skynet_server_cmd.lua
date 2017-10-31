package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua;D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/skynet/?.lua'
--luanch the skynet from os command.
--tasklist | find /i "skynet.exe" && echo found. || echo not found.
--fs_skynet_server_cmd.php?cmd=check
--cmd:
--check,check wether the skynetnserver has been launched or not.
--restart,kill the server,then restart it.
--checkandlaunch,check first,if it do not exists,launch it.
--shutdown, shutdown forcely.
function handle(r)
    r.content_type = "text/plain"
    if r.method == 'GET' or r.method == 'POST' then
        local reg_table = {}
        for k, v in pairs( r:parseargs() ) do
                reg_table[k] = v
        end

        local cmd = reg_table["cmd"]
        if nil == cmd then 
            r:puts("{\"result\":\"Unsupported http method. cmd should be here.\",\"code\":0,\"method\":\"skynet_server_cmd\"}")
            r.status = 405
            return apache2.OK
        end
       
        local skynet_restart = "\"\"D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/skynet/skynetrestart.vbs\"\""
        local skynet_exit = "\"\"D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/skynet/skynetexit.vbs\"\""
        local skynet_run = "\"\"D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/skynet/skynetrun.vbs\"\""

        if cmd == "check" then
            local exist = require "skynet_exist_check"
            r:puts("{\"result\":\""..(exist and "skynet Server has been launched." or "skynet Server hasn't been launched yet.").."\",\"code\":1,\"method\":\"skynet_server_cmd\"}")
        elseif cmd == "restart" then
            os.execute(skynet_restart)
            local exist = require "skynet_exist_check"
            r:puts("{\"result\":\""..(exist and "skynet Server has been launched." or "skynet Server hasn't been launched yet.").."\",\"code\":1,\"method\":\"skynet_server_cmd\"}")
        elseif cmd == "checkandlaunch" then
            local exist = require "skynet_exist_check"
            if false == exist then
                os.execute(skynet_run)
            end
            local exist = require "skynet_exist_check"
            r:puts("{\"result\":\""..(exist and "skynet Server has been launched." or "skynet Server hasn't been launched yet.").."\",\"code\":1,\"method\":\"skynet_server_cmd\"}")
        elseif cmd == "shutdown" then
            os.execute(skynet_exit)
            r:puts("{\"result\":\"skynet Server has been shutdowned.\",\"code\":1,\"method\":\"skynet_server_cmd\"}")
        end

    elseif r.method == 'PUT' then
-- use our own Error contents
        r:puts("{\"result\":\"Unsupported http method.\",\"code\":0,\"method\":\"skynet_server_cmd\"}")
        r.status = 405
        return apache2.OK
    else
-- use the ErrorDocument
        return 501
    end
    return apache2.OK
end