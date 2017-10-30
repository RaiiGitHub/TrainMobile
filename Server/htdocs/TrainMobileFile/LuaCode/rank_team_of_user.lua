package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua'
local fs = require 'file_system'
local jt = require 'json_tool'
local ut = require 'utility_tool'
--url format: fs_rank_team_of_user_of_user.php?userid=xx

function headContent(uname)
    local hc = string.format([[<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"><html xmlns="http://www.w3.org/1999/xhtml"><head><meta http-equiv="Content-Type" content="text/html; charset=utf-8" /><meta http-equiv="expires" content="-1"/><title>综合得分排行</title></head>
    <script type="text/javascript">
    function sendMsgToCpp(cmd)
    {
             if (typeof(JSInterface) == 'undefined')
             {
                 alert('应用有新的版本可以更新。请到http://www.resafety.com/mobile/ 下载。');
                 return;
             }
            JSInterface.sendMsg(cmd);
    }
    function RespondUserCmd(cmd)
    {
        if(cmd == "exit_game")
        {
            var bln = window.confirm("退出应用?");
            if( false == bln ) 
            return;
        }
        sendMsgToCpp(cmd);
    }
    </script>
    <link  href="pages/gpiphone.css" rel="stylesheet" type="text/css" /><body><div><img class="gp_bg"/><div class="kuang"><div class="kuangin"><div class="top">%s的队伍</div><div class="content"><div class="contentin"><div class="conleft"><img src="pages/line.png" width="4" height="660" /></div><div class="conin">
    <ul class="tou">
    <li class="small">时间</li>
    <li>人数</li>
    <li>经验值</li>
    <li>执行任务数</li>
    <li>执行时长</li>
    <li class="big">能力值</li>
    </ul>
    <div class="scroll">
    ]],uname)
    return hc
end
function topRank(class_style,datetime,member_count,exp_val,exp_total,dur,task_count,total_ability,change_ability)
    local smh = ut.toTimeString(dur)
    local change_positive = "↑"
    local change_negative = "↓"
    local tr = string.format([[
        <ul class="%s">
            <li class="fontsmall">%s</li>
            <li>%d</li>
            <li>%d/%d</li>
            <li>%d</li>  
            <li>%s</li>
            <li class="last">%.2f%s%.2f</li> 
        </ul>
    ]],class_style,datetime,member_count,exp_val,exp_total,task_count,smh,total_ability,change_ability > 0 and change_positive or change_negative,change_ability)
    return tr
end

function midRank(datetime,member_count,exp_val,exp_total,dur,task_count,total_ability,change_ability) 
    local smh = ut.toTimeString(dur)
    local change_positive = "↑"
    local change_negative = "↓"
    local mr = string.format([[
        <ul>
            <li class="fontsmall">%s</li>
            <li>%d</li>
            <li>%d/%d</li>
            <li>%d</li>  
            <li>%s</li>
            <li class="last">%.2f%s%.2f</li> 
        </ul>
    ]],datetime,member_count,exp_val,exp_total,task_count,smh,total_ability,change_ability > 0 and change_positive or change_negative,change_ability)
    return mr
end

function tailContent()
    local tc = string.format([[</div></div><div class="conright"><img src="pages/line.png" width="4" height="660" /></div></div></div><div class="bottom"><input type="button" value="再练一次" class="button" onClick="RespondUserCmd('do_again')"/><input type="button" value="退出" class="button" onClick="RespondUserCmd('exit_game')"/></div></div>
    </div></div></body></html>]])
return tc
end
function handle(r)
    r.content_type = "text/html"
    if r.method == 'GET' or r.method == 'POST' then
        local base_path = fs.getBasePagesFilePath()
        local reg_table = {}

        for k, v in pairs( r:parseargs() ) do
            reg_table[k] = v
        end
        local data = {}
        data = ut.getUserTeams(reg_table["userid"])

        if nil == data then
            r:puts("{\"result\":\"no teams\",\"code\":0,\"method\":\"rank_team_of_user\"}")
            return apache2.OK 
        end

        r:puts(headContent(data.uname))
        --r:puts(jt.encode(data.teams[1]))

        local t_border_style = {"select","select","select bordernone"}
        for ti = 1,3 do
            if ti <= #data.teams then
                local rank_data = ut.getUserRankDataInTeam(reg_table["userid"],data.teams[ti])
                if nil ~= rank_data then
                    --r:puts(jt.encode(rank_data))
                    r:puts(topRank(
                        t_border_style[ti],
                        rank_data.datetime,
                        rank_data.member_count,
                        rank_data.exp_val,
                        rank_data.exp_total,
                        rank_data.dur,
                        rank_data.task_count,
                        rank_data.total_ability,
                        rank_data.change_ability
                        ))
                end
            end
        end

        for mi = 4,#data.teams do
           local rank_data = ut.getUserRankDataInTeam(reg_table["userid"],data.teams[mi])
           if nil ~= rank_data then
                r:puts(midRank(
                   rank_data.datetime,
                   rank_data.member_count,
                   rank_data.exp_val,
                   rank_data.exp_total,
                   rank_data.dur,
                   rank_data.task_count,
                   rank_data.total_ability,
                   rank_data.change_ability))
            end
        end

        r:puts(tailContent())

    elseif r.method == 'PUT' then
        r.status = 405
        return apache2.OK
    else
        return 501
    end
    return apache2.OK
end