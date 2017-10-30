package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua'
local fs = require 'file_system'
local jt = require 'json_tool'
local ut = require 'utility_tool'
--url format: fs_rank_team.php?teamid=xx&userid=xx

function headContent()
    local hc = [[<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"><html xmlns="http://www.w3.org/1999/xhtml"><head><meta http-equiv="Content-Type" content="text/html; charset=utf-8" /><meta http-equiv="expires" content="-1"/><title>综合得分排行</title></head>
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
    <link  href="pages/gpiphone.css" rel="stylesheet" type="text/css" /><body><div><img class="gp_bg"/><div class="kuang"><div class="kuangin"><div class="top">组队演练结果</div><div class="content"><div class="contentin"><div class="conleft"><img src="pages/line.png" width="4" height="660" /></div><div class="conin">
    <ul class="tou">
    <li class="small">排名</li>
    <li>学员</li>
    <li>经验值</li>
    <li>执行任务数</li>
    <li>执行时长</li>
    <li class="big">能力值</li>
    </ul>
    <div class="scroll">
    ]]
    return hc
end
function topRank(is_current_user,class_style,rank_index,user_name,exp_val,exp_total,dur,task_count,total_ability,change_ability)
    local smh = ut.toTimeString(dur)
    local user_img = [[<img src="pages/user.png" width="32" height="32" />]]
    local user_none = ""
    local change_positive = "↑"
    local change_negative = "↓"
    local tr = string.format([[
        <ul class="%s">
            <li class="small">%s%d</li>
            <li>%s</li>
            <li>%d/%d</li>
            <li>%d</li>  
            <li>%s</li>
            <li class="last">%.2f%s%.2f</li> 
        </ul>
    ]],class_style,is_current_user and user_img or user_none,rank_index,user_name,exp_val,exp_total,task_count,smh,total_ability,change_ability > 0 and change_positive or change_negative,change_ability)
    return tr
end

function midRank(is_current_user,rank_index,user_name,exp_val,exp_total,dur,task_count,total_ability,change_ability) 
    local smh = ut.toTimeString(dur)
    local user_img = [[<img src="pages/user.png" width="32" height="32" />]]
    local user_none = ""
    local change_positive = "↑"
    local change_negative = "↓"
    local mr = string.format([[
        <ul>
            <li class="small">%s%d</li>
            <li>%s</li>
            <li>%d/%d</li>
            <li>%d</li>  
            <li>%s</li>
            <li class="last">%.2f%s%.2f</li> 
        </ul>
    ]],is_current_user and user_img or user_none,rank_index,user_name,exp_val,exp_total,task_count,smh,total_ability,change_ability > 0 and change_positive or change_negative,change_ability)
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
        if nil == reg_table["teamid"] or nil == reg_table["userid"] then
            r:puts("{\"result\":\"teamid and userid parameters should be here.\",\"code\":0,\"method\":\"rank_team\"}")
            return apache2.OK 
        end
        r:puts(headContent())

        local tmsc = {}
        tmsc = ut.getTeamMemberSortContent(reg_table["teamid"])
        --r:puts(jt.encode(tmsc))
        if nil == tmsc then
            r:puts("{\"result\":\"team data not found\",\"code\":0,\"method\":\"rank_team\"}")
            return apache2.OK 
        end
        local t_border_style = {"select","select","select bordernone"}
        for ti = 1,3 do
            if ti <= #tmsc.members then
                local member = tmsc.members[ti]
                local is_current_user = string.lower(reg_table["userid"]) == string.lower(member.id)
                local class_style = t_border_style[ti]
                local rank_index = ti
                local user_name = member.name
                local exp_val = member.exp_gain
                local exp_total = member.cur_exp
                local dur = member.time_spend
                local task_count = member.task_count
                local total_ability = member.cur_ability
                local change_ability = member.cur_ability_changed
                r:puts(topRank(
                   is_current_user,
                   class_style,
                   rank_index,
                   user_name,
                   exp_val,
                   exp_total,
                   dur,
                   task_count,
                   total_ability,
                   change_ability))
            end
        end

        for mi = 4,#tmsc.members do
            local member = tmsc.members[mi]
            local is_current_user = string.lower(reg_table["userid"]) == string.lower(member.id)
            local rank_index = mi
            local user_name = member.name
            local exp_val = member.exp_gain
            local exp_total = member.cur_exp
            local dur = member.time_spend
            local task_count = member.task_count
            local total_ability = member.cur_ability
            local change_ability = member.cur_ability_changed
             r:puts(midRank(
                is_current_user,
                rank_index,
                user_name,
                exp_val,
                exp_total,
                dur,
                task_count,
                total_ability,
                change_ability))
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