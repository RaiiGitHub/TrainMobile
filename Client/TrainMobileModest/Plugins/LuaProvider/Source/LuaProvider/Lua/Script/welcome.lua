local libTest = require "lualibTest"
function welcome()
    return [[Hello,this is lua.
您好，这里是Lua.]]
end
g_w = welcome()
if nil ~= libTest then
 g_w = libTest.method_1()
end