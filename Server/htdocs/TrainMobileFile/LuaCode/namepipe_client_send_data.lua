package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua;D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/namepipe/?.lua'
--idlcpp for apache.
--url format: fs_namepipe_client_send_data.php?id=xx&key=xx&m1=xx&m2=xx...
local jt = require 'json_tool'
function handle(r)
    r.content_type = "text/plain"
    if r.method == 'GET' or r.method == 'POST' then
        local reg_table = {}
        for k, v in pairs( r:parseargs() ) do
                reg_table[k] = v
        end

        local uid = reg_table["id"]
        local ukey = reg_table["key"]
        if nil == uid or nil == ukey then 
            r:puts("{\"result\":\"Unsupported http method.id and key should be here.\",\"code\":0,\"method\":\"namepipe_client_send_data\"}")
            r.status = 405
            return apache2.OK
        end
        reg_table["id"] = nil
        reg_table["key"] = nil
        local msg = jt.encode(reg_table)


        require "paflua"
        paf.pafcore.System.LoadDLL("LuaNamepipeClient.dll")
        local nps_name = require "nps_name"
        local npc = paf.Namepipe.Client()
        local main_nps = "\\\\localhost\\pipe\\"..nps_name
        local client_npc = "\\\\localhost\\pipe\\user_"..uid
        local client_nps = "\\\\.\\pipe\\user_"..uid
        local npc_login_ret = npc:LoginNamepipeServer(main_nps,client_nps,client_npc)._

        if true == npc_login_ret then
            local client_wm_ret = npc:WriteMessage(ukey,msg)._
            r:puts("{\"result\":\"message send\",\"code\":1,\"method\":\"namepipe_client_send_data\"}")
        else
            r:puts("{\"result\":\"namepipe not found or id is invalid.\",\"code\":0,\"method\":\"namepipe_client_send_data\"}")
        end
        
    elseif r.method == 'PUT' then
-- use our own Error contents
        r:puts("{\"result\":\"Unsupported http method.\",\"code\":0,\"method\":\"namepipe_client_send_data\"}")
        r.status = 405
        return apache2.OK
    else
-- use the ErrorDocument
        return 501
    end
    return apache2.OK
end
