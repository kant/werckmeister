-- <command name="drumRudiments" where="mod" using="lua/mod/drumRudiments.lua">
-- <![CDATA[
-- Implements a collection of different drum rudiments. ([see Wikipedia](https://en.wikipedia.org/wiki/Drum_rudiment))
-- You can specify which notes are for `L` and `R` and performing duration.
-- This will be achived by using an chord as source material.
-- The chord pitches will be used for `L` and `R`.
-- The length of the chord event defines over which time the rudiment will be peformed.
-- An event tag specifies which rudiment will be performed.
-- For example:
-- This plays one paradiddle over a length of 1 quarter
-- using `sn1` for R and `sn2` for L:
-- ```
-- /mod: drumRudiments/\n
--                 R     L \n
-- "paradiddle"@<"sn1" "sn2">4\n
-- ```
-- Examples:
-- **a single paradiddle**
-- ```language=Werckmeister\n
--using "lua/mods/drumRudiments.lua";\n
--tempo: 150;\n
--device: MyDevice  midi 0;\n
--instrumentDef:piano  MyDevice  _ch=0 _pc=0;\n
--[\n
--instrument: piano;\n
--{\n
--   /mod: drumRudiments/\n
--   -- a paradiddle (RLRR) with c' for R and c for L\n
--   "paradiddle"@<c' c>1\n
--}\n
--]\n
-- ```
-- **repeating**
-- ```language=Werckmeister\n
--using "lua/mods/drumRudiments.lua";\n
--tempo: 120;\n
--device: MyDevice  midi 0;\n
--instrumentDef:piano  MyDevice  _ch=0 _pc=0;\n
--[\n
--instrument: piano;\n
--{\n
--   /mod: drumRudiments/\n
--   -- performs 4 paradiddles (RLRR) with c' for R and c for L\n
--   "4x paradiddle"@<c' c>1\n
--}\n
--]\n
-- ```
-- **orchestration**
-- If you want to orchestrate a rudiment over a drum set,
-- you are able to define more R & L notes. *(The total number of the notes has to be even.)*
-- ```language=Werckmeister\n
--using "lua/mods/drumRudiments.lua";\n
--tempo: 120;\n
--device: MyDevice  midi 0;\n
--instrumentDef:piano  MyDevice  _ch=0 _pc=0;\n
--[\n
--instrument: piano;\n
--{\n
--   /mod: drumRudiments/\n
--   -- performs a paradiddlediddle (RLRRLL) with \n
--   --      c' for R1 and c for L1\n
--   -- and  d' for R2 and d for L2.\n
--   -- The final sequence: c'(R1) c(L1) d'(R2) c(R1) d(L2) c(L1) \n
--   "paradiddlediddle"@<c' c d' d>1\n
--}\n
--]\n
-- ```
-- **Supported Rudiments**
--<ul style="font-size: 0.9em;">
--<li>singleStrokeRoll</li>
--<li>doubleStrokeRoll</li>
--<li>trippleStrokeRoll</li>
--<li>fiveStrokeRoll</li>
--<li>singleStrokeFour</li>
--<li>singleStrokeSeven</li>
--<li>multipleBounceRoll</li>
--<li>sixStrokeRoll</li>
--<li>sevenStrokeRoll</li>
--<li>nineStrokeRoll</li>
--<li>tenStrokeRoll</li>
--<li>elevenStrokeRoll</li>
--<li>thirteenStrokeRoll</li>
--<li>fifteenStrokeRoll</li>
--<li>seventeenStrokeRoll</li>
--<li>paradiddle</li>
--<li>doubleParadiddle</li>
--<li>trippleParadiddle</li>
--<li>paradiddlediddle</li>
--</ul>
-- ]]>
-- </command>



require "lua/com/com"
require "_drumRudimentsRepository"
require "_events"

parameters = {
}

SupportedRepeatTags = {
    ["2x"]  =  2,
    ["3x"]  =  3,
    ["4x"]  =  4,
    ["5x"]  =  5,
    ["6x"]  =  6,
    ["7x"]  =  7,
    ["8x"]  =  8,
    ["9x"]  =  9,
    ["11x"] = 11,
    ["12x"] = 12,
    ["13x"] = 13,
    ["14x"] = 14,
    ["15x"] = 15,
    ["16x"] = 16,
    ["17x"] = 17,
    ["18x"] = 18,
    ["19x"] = 19,
    ["20x"] = 20,
    ["21x"] = 21,
    ["22x"] = 22,
    ["23x"] = 23,
    ["24x"] = 24,
    ["25x"] = 25,
    ["26x"] = 26,
    ["27x"] = 27,
    ["28x"] = 28,
    ["29x"] = 29,
    ["30x"] = 30,
    ["31x"] = 31,
    ["32x"] = 32,
}

local tiedEventCache = {
    ["offset"] = 0,
    ["duration"] = 0,
    ["originEvent"] = nil
}

local function newTiedEventCache(event)
    tiedEventCache.duration = event.duration
    tiedEventCache.originEvent = event
end

local function isTiedEventCacheEmpty()
    return tiedEventCache.originEvent == nil
end

local function clearTiedEventCache()
    tiedEventCache.originEvent = nil
    tiedEventCache.duration = 0
    tiedEventCache.offset = 0
end

local function getPossibleRudimentEvent(events)
    local firstEvent = events[1]
    if not isTiedEventCacheEmpty() then
        tiedEventCache.duration = tiedEventCache.duration + firstEvent.duration
        tiedEventCache.offset = tiedEventCache.offset - firstEvent.duration
        if firstEvent.isTied then
            return nil
        end
    end
    if not isTiedEventCacheEmpty() and not firstEvent.isTied then
        firstEvent = tiedEventCache.originEvent
        firstEvent.isTied = false
        firstEvent.offset = tiedEventCache.offset
        firstEvent.duration = tiedEventCache.duration
        clearTiedEventCache()
    end
    return firstEvent
end


function perform(events, params, timeinfo)
    local firstEvent = getPossibleRudimentEvent(events)
    if (firstEvent == nil) then
        return {}
    end
    if #events == 0 or #firstEvent.tags == 0 then
        return events
    end
    if firstEvent.duration == 0 then
        return events
    end
    local rudimentPerformer = RudimentPerformer:new()
    rudimentPerformer:setSourceEvent(firstEvent)
    if rudimentPerformer.rudiment == nil then
        return events
    end
    if firstEvent.isTied then
        newTiedEventCache(firstEvent)
        return {}
    end
    local rudimentEvents = rudimentPerformer:perform();
    return rudimentEvents
end

RudimentPerformer = {}
function RudimentPerformer:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    self.idxL = 1
    self.idxR = 1
    self.offset = 0
    return o
end

function RudimentPerformer:findRudimentName(tags)
    for idx, tag in pairs(tags) do
        local val = Rudiments[tag]
        if val ~= nil then
            return tag
        end
    end
    return nil
end

function RudimentPerformer:findRepeatTagValue(tags)
    for idx, tag in pairs(tags) do
        local val = SupportedRepeatTags[tag]
        if val ~= nil then
            return val
        end
    end
    return nil
end

function RudimentPerformer:setSourceEvent(event)
    local rudimentName = self:findRudimentName(event.tags)
    local source       = event.pitches
    self.offset        = event.offset
    self.rudiment = Rudiments[rudimentName]
    if self.rudiment == nil then
        return
    end
    if #source % 2 ~= 0 then
        -- not enough events
        error("not enough chord pitches for rudiment " .. rudimentName)
    end
    local repeatCount = self:findRepeatTagValue(event.tags)
    if repeatCount ~= nil then
        self:repeat_(repeatCount)
    end
    self.duration = event.duration
    self.ls = {}
    self.rs = {}
    self.velocity = event.velocity
    for idx, pitch in pairs(source) do
        if idx % 2 == 0 then
            table.insert(self.ls, pitch)
        else
            table.insert(self.rs, pitch)
        end
    end
end

function RudimentPerformer:repeat_(count)
    local sequence = {}
    for i=1,count do
        local useAlternation = i % 2 == 1
        for idx, rudimentEvent in pairs(self.rudiment) do
            local event = rudimentEvent
            if (useAlternation) then
                event = AlternateEvent(event)
            end
            table.insert(sequence, event)
        end
    end
    self.rudiment = sequence
end

function RudimentPerformer:l()
    local l = self.ls[self.idxL]
    self.idxL = self.idxL + 1
    if self.idxL > #self.ls then
        self.idxL = 1
    end
    return l
end

function RudimentPerformer:r()
    local r = self.rs[self.idxR]
    self.idxR = self.idxR + 1
    if self.idxR > #self.ls then
        self.idxR = 1
    end
    return r
end

function RudimentPerformer:nextPitch(lOrR)
    if lOrR == nil then
        error("invalid L R value for rudiment: nil")
    end
    if lOrR == L then
        return self:l()
    elseif lOrR == R then
        return self:r()
    end
    error("invalid L R value for rudiment: " .. lOrR)
end

-- duration of the rudiment definition
function RudimentPerformer:defDuration()
    local duration = 0
    for idx, rudiment in pairs(self.rudiment) do
        duration = duration + rudiment.duration
    end
    return duration
end

function RudimentPerformer:perform()
    local events = {}
    local offset = self.offset
    local durationFactor = self.duration / self:defDuration()
    for idx, rudiment in pairs(self.rudiment) do
        local which          = rudiment.which
        local duration       = rudiment.duration
        local velocityFactor = rudiment.velocityFactor
        local note = Note:new()
        note.duration = duration * durationFactor
        note.velocity = self.velocity * velocityFactor
        note.offset = offset
        local pitch = self:nextPitch(which)
        note:addPitch(pitch.pitch, pitch.octave)
        table.insert(events, note)
        offset = offset + note.duration
    end
    return events
end

