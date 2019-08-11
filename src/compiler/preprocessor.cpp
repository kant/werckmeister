#include "preprocessor.h"
#include <sheet/Track.h>
#include <compiler/metaData.h>

namespace sheet {
	namespace compiler {

		namespace {
			void handleTiedEvent(Event &ev, Voice::Events::iterator it, Voice::Events::iterator end)
			{
				auto _checkTie = [&ev](const Event &toCheck) {
					std::vector<sheet::PitchDef> diff;
					std::set_difference(ev.pitches.begin(), ev.pitches.end(), 
						toCheck.pitches.begin(), toCheck.pitches.end(), std::inserter(diff, diff.begin()))
					;
					if (diff.size() > 0) {
						FM_THROW(fm::Exception, "try to tying different pitches"); 
					}
				};
				auto lastDuration = ev.duration;
				for (++it; it!=end; ++it) {
					if (!it->isTimeConsuming()) {
						continue;
					}
					_checkTie(*it);
					if (it->duration == 0) {
						it->duration = lastDuration;
					} else {
						lastDuration = it->duration;
					}
					ev.duration += it->duration;
					if (it->type != Event::TiedNote) {
						it->type = Event::IgnoreAfterTying;
						break;
					}
					it->type = Event::IgnoreAfterTying;
				}
				ev.type = ev.type == Event::TiedNote ? Event::Note : Event::Degree;
			}
		}

        void Preprocessor::process(Track &track)
        {
            for (auto &voice : track.voices)
				{
					auto it = voice.events.begin();
					auto end = voice.events.end();
					fm::Ticks lastDuration = VoiceMetaData::DefaultDuration;
					for (; it!=end; ++it)
					{
						auto &ev = *it;
						if (ev.isTimeConsuming() || ev.type == Event::IgnoreAfterTying) {
							if (ev.duration == 0) {
								ev.duration = lastDuration;
							} else {
								lastDuration = ev.duration;
							}
						}
						if (ev.type == Event::TiedNote || ev.type == Event::TiedDegree) {
							handleTiedEvent(ev, it, end);
							continue;
						}
					}
				}
        }

    }
}