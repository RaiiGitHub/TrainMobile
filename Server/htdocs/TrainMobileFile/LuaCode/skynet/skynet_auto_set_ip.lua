package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/?.lua;D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/skynet/?.lua'
local fs = require 'file_system'
local jt = require 'json_tool'
local lib_path = fs.getBaseLibFilePath()
local db_path = fs.getBaseDataFilePath()
local ip_path = lib_path.."../skynet/ip"
local ip_content = fs.getFile(ip_path)
local system_cfg_file = db_path.."system.json"
local scf_content = ""
local scf_rest = ""
scf_content,scf_rest = fs.getFileLine(system_cfg_file)
local json_scf = jt.decode(scf_content)
json_scf["socket_server_ip"] = (0 == #ip_content and "127.0.0.1" or ip_content)
fs.writeFile(system_cfg_file,jt.encode(json_scf),'w')