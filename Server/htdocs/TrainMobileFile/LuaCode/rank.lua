package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua'
local fs = require 'file_system'
local jt = require 'json_tool'
local ut = require 'utility_tool'
--url format: fs_rank.php?

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
    function RedirectToMyTeam(uid)
    {
        window.location.href="fs_rank_team_of_user.php?userid="+uid
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
        //window.location.href="fs_user_update_info.php?id="+uid+"&psw="+upsw+"&cmd="+cmd;
    }
    </script>
    <link  href="pages/gpiphone.css" rel="stylesheet" type="text/css" /><body><div><img class="gp_bg"/><div class="kuang"><div class="kuangin"><div class="top">综合得分排行榜</div><div class="content"><div class="contentin"><div class="conleft"><img src="pages/line.png" width="4" height="660" /></div><div class="conin"><ul class="tou"><li class="small">排名</li><li>用户名</li><li>经验值</li><li class="big">能力值</li><li>练习时间</li><li>查看信息</li></ul><div class="scroll">]]
    return hc
end
function topRank(is_current_user,class_style,rank_index,user_id,current_user_id,user_name,exp_val,epm,dur)
    local smh = ut.toTimeString(dur and exp_val)
    local ensure_ev = tonumber(exp_val)
    local user_img = [[<img src="pages/user.png" width="32" height="32" />]]
    local user_none = ""
    local tr = string.format([[<ul class="%s"><li class="small">%s%d</li><li>%s</li><li>%d</li><li class="big">%.2f</li><li>%s</li><li class="last"><a href="fs_user_page.php?id=%s&current_id=%s">查看信息</a></li></ul>
    ]],class_style,is_current_user and user_img or user_none,rank_index,user_name,ensure_ev,epm,smh,user_id,current_user_id)
    return tr
end
function midRank(is_current_user,rank_index,user_id,current_user_id,user_name,exp_val,epm,dur) 
    local smh = ut.toTimeString(dur and exp_val)
    local ensure_ev = tonumber(exp_val)
    local user_img = [[<img src="pages/user.png" width="32" height="32" />]]
    local user_none = ""
    local mr = string.format([[<ul><li class="small">%s%d</li><li>%s</li><li>%d</li><li class="big">%.2f</li><li>%s</li><li class="last"><a href="fs_user_page.php?id=%s&current_id=%s">查看信息</a></li></ul>
    ]],is_current_user and user_img or user_none,rank_index,user_name,ensure_ev,epm,smh,user_id,current_user_id)
    return mr
end
function tailContent(uid)
    local tc = string.format([[</div></div><div class="conright"><img src="pages/line.png" width="4" height="660" /></div></div></div>
    <div class="bottom">
    <input type="button" value="我的队伍" class="button" onClick="RedirectToMyTeam('%s')"/>
    <input type="button" value="再练一次" class="button" onClick="RespondUserCmd('do_again')"/>
    <input type="button" value="退出" class="button" onClick="RespondUserCmd('exit_game')"/>
    </div></div>
    </div></div></body></html>]],uid)
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
        if nil == reg_table["id"] then
            r:puts("{\"result\":\"id parameters should be here.\",\"code\":0,\"method\":\"rank\"}")
            return apache2.OK 
        end
        r:puts(headContent())
        local max_epm = 0
        local max_exp = 0
        local max_dur = 0
        local ability_weight = {10,50,40}
        local u_sort_eles = {}
        max_epm,max_exp,max_dur,u_sort_eles = ut.getSortContent(r,reg_table["id"])
        local t_border_style = {"select","select","select bordernone"}
        for ti = 1,3 do
            if ti <= #u_sort_eles then
                local u_dur = u_sort_eles[ti]["exp_duration"]
                local u_exp = u_sort_eles[ti]["exp"]
                local uexp = u_exp/max_exp*ability_weight[2]
                local uepm = (u_dur==0 and 0 or u_exp/u_dur/max_epm)*ability_weight[1]
                local udur = u_dur/max_dur*ability_weight[3]
                r:puts(topRank(
                    string.lower(reg_table["id"]) == string.lower(u_sort_eles[ti]["id"]),
                    t_border_style[ti],
                    ti,
                    u_sort_eles[ti]["id"],
                    reg_table["id"],
                    u_sort_eles[ti]["name"],
                    u_exp,
                    uexp+uepm+udur,
                    u_dur))
            end
        end
        for mi = 4,#u_sort_eles do
            local u_dur = u_sort_eles[mi]["exp_duration"]
            local u_exp = u_sort_eles[mi]["exp"]
            local uexp = u_exp/max_exp*ability_weight[2]
            local uepm = (u_dur==0 and 0 or u_exp/u_dur/max_epm)*ability_weight[1]
            local udur = u_dur/max_dur*ability_weight[3]
             r:puts(midRank(
                string.lower(reg_table["id"]) == string.lower(u_sort_eles[mi]["id"]),
                mi,
                u_sort_eles[mi]["id"],
                reg_table["id"],
                u_sort_eles[mi]["name"],
                u_exp,
                uexp+uepm+udur,
                u_dur))
        end
         r:puts(tailContent(reg_table["id"]))
    elseif r.method == 'PUT' then
        r.status = 405
        return apache2.OK
    else
        return 501
    end
    return apache2.OK
end