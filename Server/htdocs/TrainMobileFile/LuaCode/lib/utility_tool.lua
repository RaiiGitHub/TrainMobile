require 'string'
local fs = require 'file_system'
local jt = require 'json_tool'

local ut = {}

ut.toTimeString=
function(seconds)
    local ensure_seconds = tonumber(seconds)
    if nil == ensure_seconds then
        ensure_seconds = 0
    end
    local s = ensure_seconds % 60
    local m = ((ensure_seconds - s)/60) % 60
    local h = (ensure_seconds - m*60 - s)/3600
    local smh = string.format("%02d:%02d:%02d",h,m,s)
    return smh
end

ut.getSortContent=
function (r)
    local base_path = fs.getBaseDataFilePath()
    local user_files = fs.fileList(base_path.."users")
    --r:puts(#user_files)
    local u_eles = {}
    local index = 1
    local max_exp = 0
    local max_epm = 0
    local max_dur = 0
    for i,v in pairs( user_files ) do
        if true == fs.fileExists(v) then 
            local u_info = jt.decode(fs.getFile(v))
            local cur_exp_dur = nil ~= u_info["exp_duration"] and tonumber(u_info["exp_duration"]) or 0
            local cur_exp = nil ~= u_info["exp"] and tonumber(u_info["exp"]) or 0
            local cur_emp = cur_exp_dur > 0 and cur_exp/cur_exp_dur or 0
            u_eles[index] = {}
            u_eles[index]["exp_duration"] = cur_exp_dur
            u_eles[index]["exp"] = cur_exp
            u_eles[index]["name"] = u_info["name"]
            u_eles[index]["id"] = u_info["id"]
            max_exp = max_exp > cur_exp and max_exp or cur_exp
            max_epm = max_epm > cur_emp and max_epm or cur_emp
            max_dur = max_dur > cur_exp_dur and max_dur or cur_exp_dur
            --r:puts("<br>cur_exp:"..cur_exp..",cur_exp_dur:"..cur_exp_dur..",cur_exp_emp:"..cur_emp)
            index = index + 1  
        end
    end
    --r:puts("<br>max_emp:"..max_epm..",max_exp:"..max_exp..",max_dur:"..max_dur)
    local ability_weight = {.1,.5,.4}
    local ele_index
    for ele_index=2,#u_eles do
        local pre = ele_index - 1
        local temp = u_eles[ele_index]
        local temp_dur = tonumber(temp["exp_duration"])
        while pre >= 1 do
            local pre_dur = tonumber(u_eles[pre]["exp_duration"])  
            if 0 == pre_dur then 
                pre_dur = 1 --illegal data.
            end
            if 0 == temp_dur then
                temp_dur = 1 --illegal data.
            end
            local epm_pre = u_eles[pre]["exp"]/pre_dur/max_epm*ability_weight[1]
            local epm_temp = temp["exp"]/temp_dur/max_epm*ability_weight[1]
            local exp_pre = u_eles[pre]["exp"]/max_exp*ability_weight[2]
            local exp_temp = temp["exp"]/max_exp*ability_weight[2]
            local dur_pre = u_eles[pre]["exp_duration"]/max_dur*ability_weight[3]
            local dur_temp = temp["exp_duration"]/max_dur*ability_weight[3]
            if epm_pre+exp_pre+dur_pre > epm_temp+exp_temp+dur_temp then 
                break
            end
            u_eles[pre+1] = u_eles[pre]
            pre = pre - 1
        end
        u_eles[pre+1] = temp
    end
    --for k,v in pairs(u_eles) do
    --    local epm = v["exp"]/v["exp_duration"]
    --    local ability = epm/max_epm*30+v["exp"]/max_exp*50+v["exp_duration"]/max_dur*20
    --    r:puts(k..":"..v["id"]..","..v["exp"]..","..epm..","..ability.."<br>")
    --end
    return max_epm,max_exp,max_dur,u_eles
end

ut.getUserStatistics=
function()
    local base_path = fs.getBaseDataFilePath()
    local user_files = fs.fileList(base_path.."users")
    --r:puts(#user_files)
    local u_eles = {}
    local index = 1
    local max_exp = 0
    local max_epm = 0
    local max_dur = 0
    for i,v in pairs( user_files ) do
        if true == fs.fileExists(v) then 
            local u_info = jt.decode(fs.getFile(v))
            local cur_exp_dur = nil ~= u_info["exp_duration"] and tonumber(u_info["exp_duration"]) or 0
            local cur_exp = nil ~= u_info["exp"] and tonumber(u_info["exp"]) or 0
            local cur_emp = cur_exp_dur > 0 and cur_exp/cur_exp_dur or 0
            u_eles[index] = {}
            u_eles[index]["exp_duration"] = cur_exp_dur
            u_eles[index]["exp"] = cur_exp
            u_eles[index]["name"] = u_info["name"]
            u_eles[index]["id"] = u_info["id"]
            max_exp = max_exp > cur_exp and max_exp or cur_exp
            max_epm = max_epm > cur_emp and max_epm or cur_emp
            max_dur = max_dur > cur_exp_dur and max_dur or cur_exp_dur
            index = index + 1  
        end
    end
    return max_epm,max_exp,max_dur
end

ut.setUserAbility=
function(uele)
    local u_dur = uele["exp_duration"]
    local u_exp = uele["exp"]
    if nil == u_dur or nil == u_exp then
        uele["ability"] = 0.0
    else
        local max_epm = 0
        local max_exp = 0
        local max_dur = 0
        local ability_weight = {10,50,40}
        max_epm,max_exp,max_dur = ut.getUserStatistics()
        local uexp = u_exp/max_exp*ability_weight[2]
        local uepm = (u_dur==0 and 0 or u_exp/u_dur/max_epm)*ability_weight[1]
        local udur = u_dur/max_dur*ability_weight[3]
        uele["ability"] = uexp+uepm+udur
    end
end

ut.getUserData=
function(userid)
    local base_path = fs.getBaseDataFilePath()
    local user_file = base_path.."users/user_"..userid..".json"
    if true == fs.fileExists(user_file) then 
        return jt.decode(fs.getFile(v))
    end
    return nil
end

ut.getTeamMemberSortContent=
function (teamid)
    local base_path = fs.getBaseDataFilePath()
    local lobby_file = base_path.."lobbies/lobby_"..teamid..".json"
    local lobby_data = {}
    if true == fs.fileExists(lobby_file) then 
        lobby_data = jt.decode(fs.getFile(lobby_file))
    else
        return nil
    end
    local lobby_members = lobby_data["members"]
    local ele_index = 0
    for ele_index=2,#lobby_members do
        local pre = ele_index - 1
        local temp = lobby_members[ele_index]
        local temp_cac = tonumber(temp["cur_ability_changed"])
        while pre >= 1 do
            local pre_cac = tonumber(lobby_members[pre]["cur_ability_changed"])  
            if pre_cac > temp_cac then 
                break
            end
            lobby_members[pre+1] = lobby_members[pre]
            pre = pre - 1
        end
        lobby_members[pre+1] = temp
    end
    return lobby_data
end

ut.getUserTeams = 
function(uid)
    local data = {}
    data.uname = ""
    data.teams = {}

    local base_path = fs.getBaseDataFilePath()
    local lobby_files = fs.fileList(base_path.."lobbies")
    for i,v in pairs( lobby_files ) do
        if true == fs.fileExists(v) then 
            local team_info = jt.decode(fs.getFile(v))
            if nil ~= team_info.members then
                for _k,_v in pairs(team_info.members) do
                    if uid == _v.id then
                        table.insert(data.teams,team_info)
                        data.uname = _v.name
                    end
                end
            end
        end
    end

    --sort data by datetime.
    local ele_index = 0
    for ele_index=2,#data.teams do
        local pre = ele_index - 1
        local temp = data.teams[ele_index]
        local temp_dt = temp["datetime"]
        while pre >= 1 do
            local pre_dt = data.teams[pre]["datetime"]
            if pre_dt > temp_dt then 
                break
            end
            data.teams[pre+1] = data.teams[pre]
            pre = pre - 1
        end
        data.teams[pre+1] = temp
    end

    return data
end

ut.getUserRankDataInTeam = 
function(uid,team)
     if nil == team or nil == team.members or nil == uid then
         return nil
     end
     for k,v in pairs(team.members) do
        if v.id == uid then
            local rank_data = {}
            rank_data.datetime = team.datetime
            rank_data.member_count = #team.members
            rank_data.exp_val = v.exp_gain
            rank_data.exp_total = v.cur_exp
            rank_data.dur = v.time_spend
            rank_data.task_count = v.task_count
            rank_data.total_ability = v.cur_ability
            rank_data.change_ability = v.cur_ability_changed
            return rank_data
        end
     end
     return nil
end

return ut