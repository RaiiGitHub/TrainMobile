package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua'

--url format: fs_redirect.php?url=xxxx

function redirect(url)
    return string.format([[
 <!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
        <html xmlns="http://www.w3.org/1999/xhtml">
        <head>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
        <meta http-equiv="expires" content="-1"/>
        <title>redirect of url</title>
        <script type="text/javascript">
            window.location.href="http://%s"; 
        </script>
        </head>
    </html>
    ]],url)
end


function handle(r)
    if r.method == 'GET' then
        r.content_type = "text/html"
        local reg_table = {}
        for k, v in pairs( r:parseargs() ) do
            reg_table[k] = v
        end

        local url = reg_table["url"]
        if nil == url then
            r:puts("{\"result\":\"url should be here.\",\"code\":0,\"method\":\"redirect\"}")
            r.status = 405
            return apache2.OK
        end
        r:puts(redirect(url))
    elseif r.method == 'PUT' then
-- use our own Error contents
        r:puts("{\"result\":\"Unsupported http method.\",\"code\":0,\"method\":\"version\"}")
        r.status = 405
        return apache2.OK
    else
-- use the ErrorDocument
        return 501
    end
    return apache2.OK
end
