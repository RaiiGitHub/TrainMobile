package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua;D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/namepipe/?.lua'
--luanch a namepipeserver from os command.
--tasklist | find /i "httpd.exe" && echo found. || echo not found.
--fs_namepipe_server_cmd.php?cmd=check
--cmd:
--check,check wether the namepipenserver has been launched or not.
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
            r:puts("{\"result\":\"Unsupported http method. cmd should be here.\",\"code\":0,\"method\":\"namepipe_server_cmd\"}")
            r.status = 405
            return apache2.OK
        end
       
        local nps_restart = "\"\"D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/namepipe/npsrestart.vbs\"\""
        local nps_exit = "\"\"D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/namepipe/npsexit.vbs\"\""
        local nps_run = "\"\"D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/namepipe/npsrun.vbs\"\""

        if cmd == "check" then
            local exist = require "nps_exist_check"
            r:puts("{\"result\":\""..(exist and "Namepipe Server has been launched." or "Namepipe Server hasn't been launched yet.").."\",\"code\":1,\"method\":\"namepipe_server_cmd\"}")
        elseif cmd == "restart" then
            os.execute(nps_restart)
            local exist = require "nps_exist_check"
            r:puts("{\"result\":\""..(exist and "Namepipe Server has been launched." or "Namepipe Server hasn't been launched yet.").."\",\"code\":1,\"method\":\"namepipe_server_cmd\"}")
        elseif cmd == "checkandlaunch" then
            local exist = require "nps_exist_check"
            if false == exist then
                os.execute(nps_run)
            end
            local exist = require "nps_exist_check"
            r:puts("{\"result\":\""..(exist and "Namepipe Server has been launched." or "Namepipe Server hasn't been launched yet.").."\",\"code\":1,\"method\":\"namepipe_server_cmd\"}")
        elseif cmd == "shutdown" then
            os.execute(nps_exit)
            r:puts("{\"result\":\"Namepipe Server has been shutdowned.\",\"code\":1,\"method\":\"namepipe_server_cmd\"}")
        end

    elseif r.method == 'PUT' then
-- use our own Error contents
        r:puts("{\"result\":\"Unsupported http method.\",\"code\":0,\"method\":\"namepipe_server_cmd\"}")
        r.status = 405
        return apache2.OK
    else
-- use the ErrorDocument
        return 501
    end
    return apache2.OK
end