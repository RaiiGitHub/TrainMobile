package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua'
--url format
--fs_user_upload_team.php?data={}

require 'string'
local fs = require 'file_system'
local jt = require 'json_tool'
function handle(r)
    r.content_type = "text/plain"
    if r.method == 'GET' or r.method == 'POST' then
        local base_path = fs.getBaseDataFilePath()
        local reg_table = {}
        for k, v in pairs( r:parseargs() ) do
            reg_table[k] = v
        end
        if nil == reg_table["data"] then 
            r:puts("{\"result\":\"Unsupported http method. data should be here.\",\"code\":0,\"method\":\"user_upload_team\"}")
            r.status = 405
            return apache2.OK
        end
        local data = reg_table["data"]
        local team_data = jt.decode(data)
        local team_id = team_data["teamid"]
        if nil == team_id then
            r:puts("{\"result\":\"Unsupported http method. data.teamid should be here.\",\"code\":0,\"method\":\"user_upload_team\"}")
            r.status = 405
            return apache2.OK
        end

        local lobby_file_path = base_path.."lobbies/lobby_"..team_id..".json"
        if true == fs.fileExists(lobby_file_path) then
            r:puts("{\"result\":\"shouldn't upload twice of the same lobby.\",\"code\":0,\"method\":\"user_upload_team\"}")
            r.status = 405
            return apache2.OK
        end

        team_data["datetime"] = os.date("%Y-%m-%d %H:%M:%S");
        fs.writeFile( lobby_file_path,jt.encode(team_data),'w')
        r:puts("{\"result\":\"lobby accepted.\",\"code\":1,\"method\":\"user_upload_team\"}")
    elseif r.method == 'PUT' then
-- use our own Error contents
        r:puts("{\"result\":\"Unsupported http method.\",\"code\":0,\"method\":\"user_upload_team\"}")
        r.status = 405
        return apache2.OK
    else
-- use the ErrorDocument
        return 501
    end
    return apache2.OK
end
