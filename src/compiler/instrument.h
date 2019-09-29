#ifndef COMPILER_MIDI_INSTRUMENT_H
#define COMPILER_MIDI_INSTRUMENT_H

#include <fm/common.hpp>
#include <fm/units.hpp>
#include "compiler/voicings/VoicingStrategy.h"
#include "forward.hpp"
#include <unordered_map>

namespace sheet {
    namespace compiler {
		struct AInstrumentDef {
			fm::String uname;
			VoicingStrategyPtr voicingStrategy;
			typedef std::list<AModificationPtr> Modifications;
			Modifications modifications;
			typedef std::unordered_map<fm::Expression, int> VelocityOverride;
			VelocityOverride velocityOverride;
			virtual ~AInstrumentDef() = default;
		};
    }
}
#endif