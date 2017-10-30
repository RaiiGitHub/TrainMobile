package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua'
require 'string'
local fs = require 'file_system'
local jt = require 'json_tool'
--display user info
--url format: fs_user_page.php?

function headContent()
    local hc = [[
    <!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
    <html xmlns="http://www.w3.org/1999/xhtml">
    <head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="expires" content="-1"/>
    <title>用户资料查看</title>
    <script type="text/javascript">
    function BackToRank(uid)
    {
         window.location.href="fs_rank.php?id="+uid; 
    }
    </script>
    <link  href="pages/user.css" rel="stylesheet" type="text/css" />
    </head>
    <body>
    <div class="kuang1">
    <div class="usertitle">用户资料</div>
    <div class="usercon">
    <table width="75%" border="0" cellspacing="0" cellpadding="0" align="center" >
    ]]
    return hc
end

function userInfoContent(id,from_user_id,name,pno,school,institution,addr)
    local uic = string.format([[
    <tr>
        <td class="blue">ID:</td>
        <td>%s</td>
        <td class="blue">姓名:</td>
        <td>%s</td>
    </tr>
    <tr>
        <td class="blue">电话号码:</td>
        <td colspan="3">%s</td>
        </tr>
    <tr>
        <td class="blue">培训学校:</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
    </tr>
    <tr>
        <td colspan="4">%s</td>
        </tr>
    <tr>
        <td class="blue">所在单位:</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
    </tr>
    <tr>
        <td colspan="4">%s</td>
        </tr>
    <tr>
        <td class="blue">所在地址:</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
    </tr>
    <tr>
        <td colspan="4">%s</td>
        </tr>
    <tr>
        <td colspan="4" class="center"><input type="button" value="返回" class="button" onClick="BackToRank('%s')"/></td>
        </tr>
    ]],id,name,pno,school,institution,addr,from_user_id)
    return uic
end

function tailContent()
    local tc = [[
    </table>
    </div>
    </div>
    </body>
    </html>
    ]]
return tc
end


function handle(r)
    r.content_type = "text/html"
    if r.method == 'GET' or r.method == 'POST' then
        local base_path = fs.getBaseDataFilePath()
        local reg_table = {}
        for k, v in pairs( r:parseargs() ) do
            reg_table[k] = v
        end

        if nil == reg_table["id"] or nil == reg_table["current_id"] then
            r:puts("{\"result\":\"id and current_id parameters should be here.\",\"code\":0,\"method\":\"user_page\"}")
            return apache2.OK 
        end

        local user_file_path = base_path.."users/user_"..reg_table["id"]..".json"
        if false == fs.fileExists(user_file_path) then
            r:puts("{\"result\":\"User was not registered yet.\",\"code\":0,\"method\":\"user_page\"}")
            r.status = 405
            return apache2.OK
        end

        --head
        r:puts(headContent())
        local user_info = fs.getFile(user_file_path)
        local mem = jt.decode(user_info)
        --user info content
        r:puts(userInfoContent(mem["id"],reg_table["current_id"],mem["name"] or "",mem["phone_number"] or "",mem["train_school"] or "",mem["institution"] or "",mem["address"] or ""))

        --tail
         r:puts(tailContent())
    elseif r.method == 'PUT' then
-- use our own Error contents
        r:puts("{\"result\":\"Unsupported http method.\",\"code\":0,\"method\":\"user_page\"}")
        r.status = 405
        return apache2.OK
    else
-- use the ErrorDocument
        return 501
    end
    return apache2.OK
end
