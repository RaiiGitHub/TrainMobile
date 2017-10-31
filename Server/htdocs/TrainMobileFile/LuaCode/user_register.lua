package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua'
--url format
--fs_user_register.php?id=yq.sun&psw=147895263&sex=femal&name=小怪兽

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
        if nil == reg_table["id"] then 
            r:puts("{\"result\":\"Unsupported http method.id should be here.\",\"code\":0,\"method\":\"user_register\"}")
            r.status = 405
            return apache2.OK
        end
        local user_file_path = base_path.."users/user_"..reg_table["id"]..".json"
        if true == fs.fileExists(user_file_path) then
            r:puts("{\"result\":\"this id was already been registered.\",\"code\":0,\"method\":\"user_register\"}")
            r.status = 405
            return apache2.OK
        end
        --build basic datafile for this newcome.
        --statistic index of this newcome.
        local files = fs.fileList(base_path.."users")
        reg_table["index"] = #files + 1
        fs.writeFile( user_file_path,jt.encode(reg_table),'w')
        r:puts("{\"result\":\"register accepted.user_index:"..reg_table["index"].."\",\"code\":1,\"method\":\"user_register\"}")
    elseif r.method == 'PUT' then
-- use our own Error contents
        r:puts("{\"result\":\"Unsupported http method.\",\"code\":0,\"method\":\"user_register\"}")
        r.status = 405
        return apache2.OK
    else
-- use the ErrorDocument
        return 501
    end
    return apache2.OK
end
