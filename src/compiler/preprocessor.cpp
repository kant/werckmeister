#include "preprocessor.h"
#include <sheet/Track.h>
#include <compiler/metaData.h>

namespace sheet {
	namespace compiler {

		namespace {
			void handleTiedEvent(Event &_ev, Voice::Events::iterator it, Voice::Events::iterator end)
			{
				std::list<Event*> events;
				auto _checkTie = [&events](const Event &toCheck) {
					Event &ev = *(events.back());
                    // pitches event  = A
                    // picthes toCheck= B
                    // check if A==B => #(A\B u B\A) == 0
					Event::Pitches symdiff;
					std::set_symmetric_difference(ev.pitches.begin(), ev.pitches.end(), 
						toCheck.pitches.begin(), toCheck.pitches.end(), std::inserter(symdiff, symdiff.begin()))
					;
					if (symdiff.empty()) {
                        return symdiff;
					}
                    // check if all picthes in A are tied with pitches in B => A\B
                    Event::Pitches diff;
					std::set_difference(ev.pitches.begin(), ev.pitches.end(), 
						toCheck.pitches.begin(), toCheck.pitches.end(), std::inserter(diff, diff.begin()))
					;
                    if (!diff.empty()) {
						FM_THROW(fm::Exception, "try to tying different pitches"); 
					}
                    return symdiff;
				};
				auto _increaseDuration = [&events](fm::Ticks duration) {
					for(auto ev : events) {
						ev->duration += duration;
					}
				};
				auto _increaseOffset = [&events](fm::Ticks offset) {
					for(auto ev : events) {
						ev->offset += offset;
					}
				};				
				auto _addEvent = [&events](Event &ev) {
					ev.type = ev.type == Event::TiedNote ? Event::Note : Event::Degree;
					events.push_back(&ev);
				};
				auto lastDuration = _ev.duration;
				_addEvent(_ev);
				for (++it; it!=end; ++it) {
					bool nextIsTied = it->type == Event::TiedNote || it->type == Event::TiedDegree; 
					if (!it->isTimeConsuming()) {
						continue;
					}
					auto pitchesLeft = _checkTie(*it);
					if (it->duration == 0) {
						it->duration = lastDuration;
					} else {
						lastDuration = it->duration;
					}
					_increaseDuration(it->duration);
					if (pitchesLeft.empty()) {
                        it->type = Event::IgnoreAfterTying;
                        if (!nextIsTied) {
                            break;
                        }
                    } else {
                        it->pitches.swap(pitchesLeft);
						_increaseOffset(-it->duration);
                        if (!nextIsTied) {
                            break;
                        }
						_addEvent(*it);
                    }
				}
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