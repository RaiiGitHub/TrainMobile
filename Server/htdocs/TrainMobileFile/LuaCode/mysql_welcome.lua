package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua'
require 'string'
local jt = require 'json_tool'

--using mysql,welcome test.
--url format: fs_mysql_welcome.php?tn=welcome_tb&method=select
--url format: fs_mysql_welcome.php?tn=welcome_tb&method=insert&data={"f_id":9,"f_name":"qiaoda9","f_desc":"new_to_insert9"}
--url format: fs_mysql_welcome.php?tn=welcome_tb&method=delete&data={"f_id":9}
--url format: fs_mysql_welcome.php?method=sql&data={"sql":"select * from welcome_tb where f_id=9"}

function handle(r)
    r.content_type = "text/plain"
    if r.method == 'GET' or r.method == 'POST' then
        local reg_table = {}
        for k, v in pairs( r:parseargs() ) do
            reg_table[k] = v
        end
        if nil == reg_table["method"] then 
            r:puts("{\"result\":\"parameter method,which claims the method to process were wanted.\",\"code\":0,\"method\":\"mysql_welcome\"}")
            return apache2.OK
        end
        local sqlmethod = reg_table["method"]
        local sqltable = reg_table["tn"]
        local sqldata = reg_table["data"]
        local sqldataval = {}
        local luasql = require "luasql.mysql"
        local env = assert(luasql.mysql())
        local conn = assert(env:connect("mysql_beginer","root","mysql","localhost",3306))
        local sqlstring = ""
        if sqlmethod == "select" then
            if nil == sqltable then 
                r:puts("{\"result\":\"parameter tn,which claims the table to process were wanted.\",\"code\":0,\"method\":\"mysql_welcome\"}")
                return apache2.OK
            end
            sqlstring = [[select * from ]]..sqltable
        elseif sqlmethod == "insert" then
            if nil == sqltable then 
                r:puts("{\"result\":\"parameter tn,which claims the table to process were wanted.\",\"code\":0,\"method\":\"mysql_welcome\"}")
                return apache2.OK
            end
            sqldataval = jt.decode(sqldata)
            local keys = ""
            local vals = ""
            for k,v in pairs(sqldataval) do
                keys = keys..k..','
                vals = vals..(type(v) == "number" and v or '"'..v..'"')..','
            end
            keys = string.sub(keys, 1, -2)
            vals = string.sub(vals, 1, -2)
            sqlstring = string.format([[insert into %s(%s) values(%s)]],sqltable,keys,vals)
        elseif sqlmethod == "delete" then
            if nil == sqltable then 
                r:puts("{\"result\":\"parameter tn,which claims the table to process were wanted.\",\"code\":0,\"method\":\"mysql_welcome\"}")
                return apache2.OK
            end
            sqldataval = jt.decode(sqldata)
            local condition = ""
            for k,v in pairs(sqldataval) do
                if 0 == #condition then
                    condition = "where "
                end
                condition = condition .. k .. ' = ' .. (type(v) == "number" and v or '"'..v..'"') .. " and "
            end
            condition = string.sub(condition, 1, -6)
            sqlstring = string.format([[delete from %s %s]],sqltable,condition)
        elseif sqlmethod == "sql" then
            sqldataval = jt.decode(sqldata)
            sqlstring =  sqldataval["sql"]
            if nil == sqlstring then
                r:puts("{\"result\":\"parameter sql in the data json data were wanted.\",\"code\":0,\"method\":\"mysql_welcome\"}")
                return apache2.OK
            end
        end 
        local cursor = nil
        local errorString = nil
        local dbdata = {}
        cursor,errorString = conn:execute(sqlstring)
        if nil == errorString and nil ~= cursor and "userdata" == type(cursor) then
            local row = cursor:fetch ({}, "a")
            while row do
                local row_data = {}
                for k,v in pairs(row) do
                    row_data[k] = v
                end
                table.insert(dbdata,row_data)
                -- reusing the table of results
                row = cursor:fetch (row, "a")
            end
        end
        conn:close()
        env:close()

        r:puts("{\"result\":"..jt.encode(dbdata)..(nil == errorString and "" or (",\"errorString\":\""..errorString..","..sqlstring.."\""))..",\"code\":1,\"method\":\"mysql_welcome\"}")

    elseif r.method == 'PUT' then
-- use our own Error contents
        r:puts("{\"result\":\"Unsupported http method.\",\"code\":0,\"method\":\"mysql_welcome\"}")
        r.status = 405
        return apache2.OK
    else
-- use the ErrorDocument
        return 501
    end
    return apache2.OK
end
