package.path = 'C:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua;C:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/namepipe/?.lua'
--idlcpp for apache.
--url format: fs_namepipe_client_get_data.php?id=xx&key=xx&pop=1/0...
function handle(r)
    r.content_type = "text/plain"
    if r.method == 'GET' or r.method == 'POST' then
        local reg_table = {}
        for k, v in pairs( r:parseargs() ) do
                reg_table[k] = v
        end

        local uid = reg_table["id"]
        local ukey = reg_table["key"]
        local upop = reg_table["pop"]
        if nil == uid or nil == ukey or nil == upop then 
            r:puts("{\"result\":\"Unsupported http method.id,key,pop should be here.\",\"code\":0,\"method\":\"namepipe_client_get_data\"}")
            r.status = 405
            return apache2.OK
        end

        require "paflua"
        paf.pafcore.System.LoadDLL("LuaNamepipeClient.dll")
        local nps_name = require "nps_name"
        local npc = paf.Namepipe.Client()
        local main_nps = "\\\\localhost\\pipe\\"..nps_name
        local client_npc = "\\\\localhost\\pipe\\user_"..uid
        local client_nps = "\\\\.\\pipe\\user_"..uid
        local npc_login_ret = npc:LoginNamepipeServer(main_nps,client_nps,client_npc)._

        if true == npc_login_ret then
            local umsg = paf.char._nullPtr_
            local client_gm_ret = npc:GetServerSideMessage(ukey,umsg,tonumber(upop) == 1)._			
            r:puts("{\"result\":"..(client_gm_ret and umsg._ or "\"no data.\"")..",\"key\":\""..ukey.."\",\"code\":1,\"method\":\"namepipe_client_get_data\"}")
        else
            r:puts("{\"result\":\"namepipe not found.\",\"code\":0,\"method\":\"namepipe_client_get_data\"}")
        end
        
    elseif r.method == 'PUT' then
-- use our own Error contents
        r:puts("{\"result\":\"Unsupported http method.\",\"code\":0,\"method\":\"namepipe_client_get_data\"}")
        r.status = 405
        return apache2.OK
    else
-- use the ErrorDocument
        return 501
    end
    return apache2.OK
end
