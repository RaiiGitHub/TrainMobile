package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua'
require 'string'
local fs = require 'file_system'

--upload file.
--url format: fs_upload.php

--upload html content
local uhc = [[
 <!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
        <html xmlns="http://www.w3.org/1999/xhtml">
        <head>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
        <meta http-equiv="expires" content="-1"/>
        <title>移动端管理入口</title>
        </head>
        <script type="text/javascript">
        function getPath(src)    
        {    
            strs=src.split("\\");
            if( strs.length > 0 )
                return strs[strs.length-1];
            return ""
        } 
        function handleFile()
        {
            var addpath = document.getElementById("path").value;
            var filename = getPath(document.getElementById("filename").value);
            var fileform = document.getElementsByTagName("form")[0];
            fileform.action = "fs_upload.php?file_name=" + addpath + filename;
            //alert(filename + " will be uploaded.Press the 'submit' button.");
            return true;
        }
        function sendMsgToCpp()
        {
             if (typeof(JSInterface) == 'undefined')
             {
                 alert('接口未定义！');
                 return;
             }
            JSInterface.sendMsg('Hello,world!');
        }
        </script>
        <form enctype="multipart/form-data" onSubmit="return handleFile()" action="" method=post>
            <input id="filename" name="content" value="xx" type="file"><br>
            additional saving path:<input id="path" value="TrainMobileFile/LuaCode/" type="text"><br>
            <input type="submit" value="submit"><br>
            <a href="trainmobilefile">view file dir</a><br>
            <input type="button" value="sendMsgToCpp" onClick="sendMsgToCpp()"><br>
            <a href="fs_skynet_server_cmd.php?cmd=checkandlaunch">launchSkynetServer</a>
            <a href="fs_skynet_server_cmd.php?cmd=restart">restartSkynetServer</a>
            <a href="fs_skynet_server_cmd.php?cmd=shutdown">shutdownSkynetServer</a>
            <a href="fs_namepipe_server_cmd.php?cmd=checkandlaunch">launchNamepipeServer</a>
            <a href="fs_namepipe_server_cmd.php?cmd=restart">restartNamepipeServer</a>
            <a href="fs_namepipe_server_cmd.php?cmd=shutdown">shutdownNamepipeServer</a>
        </form>
        </html>
]]

function print_args(r, simple, complex)
  local s = "%s: %s\n"
  for k, v in pairs(simple) do
    r:puts(s:format(k, v))
  end
end

function handle(r)
    if r.method == 'GET' then
        r.content_type = "text/html"
        r:puts(uhc)
    elseif r.method == 'POST' then
        r.content_type = "text/plain"
        local base_path = fs.getBaseProjectFilePath()
        local reg_table = {}
        for k, v in pairs( r:parseargs() ) do
           reg_table[k] = v
        end
        if nil == reg_table["file_name"] then
            r:puts("{\"result\":\"file_name parameters should be here.\",\"code\":0,\"method\":\"upload\"}")
            return apache2.OK
        end
        --print_args(r,r:parsebody())
        local sfp = base_path..reg_table["file_name"]
        local content = r:parsebody()["content"]
        fs.writeFile(sfp,content,'w')
        r:puts("{\"result\":\""..content.."\",\"code\":1,\"method\":\"upload\"}")
    elseif r.method == 'PUT' then
-- use our own Error contents
        r:puts("{\"result\":\"Unsupported http method.\",\"code\":0,\"method\":\"upload\"}")
        r.status = 405
        return apache2.OK
    else
-- use the ErrorDocument
        return 501
    end
    return apache2.OK
end
