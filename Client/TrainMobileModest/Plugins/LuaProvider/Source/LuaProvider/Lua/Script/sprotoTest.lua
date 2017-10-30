local sproto = require "sproto"
g_sproto_tell = "nothing to tell."
if nil ~= sproto then
    local x = sproto.getcanyoufindme()
    if nil ~= x then
        g_sproto_tell = x["tell"]
    end
end

