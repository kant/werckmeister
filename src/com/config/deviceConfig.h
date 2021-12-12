#pragma once

#include <string>

namespace com
{
    struct DeviceConfig
    {
        enum Type
        {
            Undefinded,
            Midi,
            FluidSynth
        };
        std::string name;
        int offsetMillis = 0;
        Type type = Undefinded;
        typedef std::string DeviceId;
        DeviceId deviceId;
    };
}
