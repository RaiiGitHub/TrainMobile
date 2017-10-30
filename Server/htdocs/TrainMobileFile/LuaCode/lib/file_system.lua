require 'string'

local fs = {}

fs.getBaseDataFilePath = 
function()
   return "D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/FileDB/"
end

fs.getBasePagesFilePath = 
function()
    return "D:/Program Files (x86)/HTTPD/htdocs/pages/"
end

fs.getBaseLibFilePath = 
function()
    return "D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/lib/"
end

fs.getBaseProjectFilePath = 
function()
    return "D:/Program Files (x86)/HTTPD/htdocs/"
end

fs.getBaseBinFilePath = 
function()
    return "D:/Program Files (x86)/HTTPD/bin/"
end

--read specific file.
fs.getFile = 
function(file_name)
  local f = assert(io.open(file_name, 'r'))
  local string = f:read("*all")
  f:close()
  return string
end

-- local lines,rest = f:read(BUFSIZE,"*line")
fs.getFileLine = 
function(file_name)
  local BUFSIZE = 84012
  local f = assert(io.open(file_name, 'r'))
  local lines,rest = f:read(BUFSIZE, "*line")
  f:close()
  return lines , rest
end

--wirte file,w overwrite， a append
fs.writeFile=
function(file_name,str,wa)
 local f = assert(io.open(file_name, wa))
 f:write(str)
 f:close()
end

--file exsit test
fs.fileExists = 
function(path)
  local file = io.open(path, "rb")
  if file then file:close() end
  return file ~= nil
end

--go throw all specific Files
fs.fileList = 
function(path)
    local lfs = require"lfs"
    local files = {}
    for file in lfs.dir(path) do
        if file ~= "." and file ~= ".." then
            local f = path..'/'..file
            local attr = lfs.attributes (f)
            if attr.mode ~= "directory" then
                table.insert(files,f)
            end
        end
    end
    return files
end

--get first specific files containing specific characters.
fs.getFilePathFromList = 
function(file_list,contain_name)
    if type(file_list) ~= "table" or type(contain_name) == "table" then return end
    for index,file in pairs(file_list) do
        local b,e = string.find(file,contain_name)
        if nil ~= b and nil ~= e then 
            return file
        end 
    end
end

--delete files.warning! delete file will be very dangerous.
fs.delFile = 
function(path)
    os.remove(path);
end

return fs