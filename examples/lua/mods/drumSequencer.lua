require "lua/com/com"

local defaultAmountValue = 50

parameters = {
    { name="a", default="?" },
    { name="b", default="?" },
    { name="c", default="?" },
    { name="d", default="?" },
    { name="e", default="?" },
    { name="f", default="?" },
    { name="g", default="?" },
    { name="h", default="?" },
    { name="i", default="?" },
    { name="j", default="?" },
    { name="k", default="?" },
    { name="l", default="?" },
    { name="m", default="?" },
    { name="n", default="?" },
    { name="o", default="?" },
    { name="p", default="?" },
    { name="q", default="?" },
    { name="r", default="?" },
    { name="s", default="?" },
    { name="t", default="?" },
    { name="u", default="?" },
    { name="v", default="?" },
    { name="w", default="?" },
    { name="x", default="?" },
    { name="y", default="?" },
    { name="z", default="?" },
    { name="A", default="?" },
    { name="B", default="?" },
    { name="C", default="?" },
    { name="D", default="?" },
    { name="E", default="?" },
    { name="F", default="?" },
    { name="G", default="?" },
    { name="H", default="?" },
    { name="I", default="?" },
    { name="J", default="?" },
    { name="K", default="?" },
    { name="L", default="?" },
    { name="M", default="?" },
    { name="N", default="?" },
    { name="O", default="?" },
    { name="P", default="?" },
    { name="Q", default="?" },
    { name="R", default="?" },
    { name="S", default="?" },
    { name="T", default="?" },
    { name="U", default="?" },
    { name="V", default="?" },
    { name="W", default="?" },
    { name="X", default="?" },
    { name="Y", default="?" },
    { name="Z", default="?" },
}

function perform(events, params, timeinfo) 
    events[1].pitches[1].alias = "sn"
    dump(params)
    return events
end