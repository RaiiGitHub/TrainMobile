package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua'
require 'string'
local fs = require 'file_system'

--upload file.
--url format: fs_upload_bin.php

--upload html content
local uhc = [[<script type="text/javascript">
 function readFile()
 {
    var read=new FileReader();
    read.readAsBinaryString(document.getElementById("filename").files[0]);
    read.onload=function(e)
    {
        var addpath = document.getElementById("path").value;
        var filename = document.getElementById("filename").value;
        var final_url = "fs_upload_bin.php?file_name=" + addpath + filename;
        var form_obj = document.getElementById("post_form");
        var di = document.getElementById("data_input");
        form_obj.action = final_url;
        di.value = read.result;
        //alert(di.value);
        form_obj.submit();
    }
 }
</script>
<input id="filename" name="content" type="file"><br>
additional saving path:<input id="path" value="pages/" type="text"><br>
<input type="button" value="submit" onClick="readFile()"><br>
<a href="trainmobilefile">view file dir</a>
<form id="post_form" enctype="multipart/form-data" action="" method=post>
    <input id="data_input" name="content" value="xx" type="hidden"><br>
</form>
]]

function print_args(r, simple, complex)
  local s = "%s: %s\n"
  for k, v in pairs(simple) do
    r:puts(s:format(k, v))
  end
end

function handle(r)
    r.content_type = "text/html"
    if r.method == 'GET' then
        r:puts(uhc)
    elseif r.method == 'POST' then
        local base_path = fs.getBaseProjectFilePath()
        local reg_table = {}
        for k, v in pairs( r:parseargs() ) do
           reg_table[k] = v
        end
        if nil == reg_table["file_name"] then
            r:puts("{\"result\":\"file_name parameters should be here.\",\"code\":0,\"method\":\"upload\"}")
            return apache2.OK
        end
        local sfp = base_path..reg_table["file_name"]
        local content = r:parsebody()["content"]
        --r:puts(#content)
        --fs.writeFile(sfp,content,'wb')
        r:puts("{\"result\":\""..content.."\",\"code\":1,\"method\":\"upload\"}")
        print_args(r,r:parsebody())
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
