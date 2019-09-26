#include "MidiContext.h"
#include <fm/midi.hpp>
#include <fm/config.hpp>
#include <math.h>
#include "error.hpp"
#include "sheet/tools.h"
#include <algorithm>
#include <fm/werckmeister.hpp>

#define SHEET_MASTER_TRACKNAME "master track"

namespace sheet {
	namespace compiler {

		namespace {
			const int DEFAULT_CHANNEL = 0;
			const int MidiSchluesselCOffset = 60;
			MidiContext::Base::VoiceId _voiceIds = 0;
			inline void _checkMidi(fm::midi::MidiPtr midi) 
			{
				if (!midi) {
					FM_THROW(Exception, "missing midi object");
				}
			}
		}


		int MidiContext::getAbsolutePitch(const PitchDef &pitch)
		{
			return MidiSchluesselCOffset + pitch.pitch + (pitch.octave * fm::NotesPerOctave);
		}

		int MidiContext::toMidiVelocity(double velocity)
		{
			return 127;
			int result = static_cast<int>(::ceil( velocity * 127.0 ));
			return std::min(127, std::max(0, result));
		} 

		std::vector<AInstrumentDef*> MidiContext::currentInstruments() const
		{
			auto midiInstruments = currentMidiInstruments();
			return std::vector<AInstrumentDef*>(midiInstruments.begin(), midiInstruments.end());
		}

		std::vector<MidiInstrumentDef*> MidiContext::currentMidiInstruments() const
		{
			auto voiceMeta = voiceMetaData<MidiContext::TrackMetaData>();
			std::vector<MidiInstrumentDef> *instruments = nullptr;
			if (voiceMeta) {
				instruments = &(voiceMeta->instruments);
			}
			auto trackMeta = trackMetaData<MidiContext::TrackMetaData>();
			instruments = &(trackMeta->instruments);
			if (!instruments) {
				FM_THROW(Exception, "instruments = null");
			}
			std::vector<MidiInstrumentDef*> result;
			result.reserve(instruments->size());
			for (auto &instrument : *instruments) {
				result.push_back(&instrument);
			}
			return result;
		}

		void MidiContext::renderPitch(const PitchDef &pitch, fm::Ticks absolutePosition, double velocity, fm::Ticks duration)
		{
			_checkMidi(midi_);
			auto event = fm::midi::Event::NoteOn(DEFAULT_CHANNEL, 
				absolutePosition, 
				getAbsolutePitch(pitch), 
				toMidiVelocity(velocity)
			);
			addEvent(event, currentMidiInstruments());
			event = fm::midi::Event::NoteOff(DEFAULT_CHANNEL, 
				absolutePosition + duration, 
				getAbsolutePitch(pitch)
			);
			addEvent(event, currentMidiInstruments());
		}

		void MidiContext::startEvent(const PitchDef &pitch, fm::Ticks absolutePosition, double velocity)
		{
			Base::startEvent(pitch, absolutePosition, velocity);
			_checkMidi(midi_);
			auto event = fm::midi::Event::NoteOn(DEFAULT_CHANNEL, 
				absolutePosition, 
				getAbsolutePitch(pitch), 
				toMidiVelocity(velocity)
			);
			addEvent(event, currentMidiInstruments());
		}

		
		void MidiContext::stopEvent(const PitchDef &pitch, fm::Ticks absolutePosition)
		{
			Base::stopEvent(pitch, absolutePosition);
			_checkMidi(midi_);
			auto event = fm::midi::Event::NoteOff(DEFAULT_CHANNEL, 
				absolutePosition, 
				getAbsolutePitch(pitch)
			);
			addEvent(event, currentMidiInstruments());
		}

		void MidiContext::addEvent(const fm::midi::Event &ev, TrackId trackId)
		{
			if (trackId == INVALID_TRACK_ID) {
				trackId = track(); // context's current track
			}
			if (trackId == INVALID_TRACK_ID) {
				FM_THROW(Exception, "failed to add an event without related track");
			}
			auto voiceConfig = voiceMetaData<MidiContext::VoiceMetaData>();
			if (voiceConfig) // && voiceConfig->tempoFactor != 1) 
			{
				auto evCopy = ev;
				evCopy.absPosition(evCopy.absPosition() * voiceConfig->tempoFactor + voiceConfig->positionOffset);
				midi_->tracks().at(trackId)->events().add(evCopy);
				return;
			}
			try {
				midi_->tracks().at(trackId)->events().add(ev);
			} catch(...) {
				FM_THROW(Exception, "failed to access track with id: " + std::to_string(trackId));
			}
		}

		void MidiContext::addEvent(const fm::midi::Event &ev, const MidiInstrumentReferences &instruments)
		{
			if (instruments.empty()) {
				addEvent(ev, INVALID_TRACK_ID);
				return;
			}
			for (auto instrument : instruments) {
				auto evCopy = ev;
				evCopy.channel(instrument->channel);
				addEvent(evCopy, instrument->trackId);
			}
		}

		void MidiContext::renderPitchbend(double value, fm::Ticks absolutePosition) 
		{
			_checkMidi(midi_);		
			auto event = fm::midi::Event::PitchBend(DEFAULT_CHANNEL, absolutePosition, value);
			addEvent(event, currentMidiInstruments());
		}

		MidiContext::Base::TrackId MidiContext::createTrackImpl()
		{
			_checkMidi(midi_);
			auto track = midi_->createTrack();
			midi_->addTrack(track);
			return midi_->tracks().size() - 1;
		}
		MidiContext::Base::VoiceId MidiContext::createVoiceImpl()
		{
			auto id = _voiceIds++;
			return id;
		}

		MidiContext::Base::VoiceMetaDataPtr MidiContext::createVoiceMetaData() 
		{
			return std::make_shared<MidiContext::VoiceMetaData>();
		}

		MidiContext::Base::TrackMetaDataPtr MidiContext::createTrackMetaData() 
		{
			return std::make_shared<MidiContext::TrackMetaData>();
		}

		void MidiContext::metaSetTempo(double bpm)
		{
			Base::metaSetTempo(bpm);
			auto meta = voiceMetaData<MidiContext::VoiceMetaData>();
			bool isMasterTempoValue = !meta;
			if (isMasterTempoValue) {
				// set global tempo
				auto tempoEvent = fm::midi::Event::MetaTempo(bpm);
				tempoEvent.absPosition(currentPosition());
				masterTempo(bpm);
				addEvent(tempoEvent, masterTrackId());
			} else {
				auto oldFactor = meta->tempoFactor;
				meta->tempoFactor = masterTempo() / bpm;
				// update offset, to fix position after tempo change
				auto fixQuotient = meta->tempoFactor / oldFactor;
				meta->position /= fixQuotient;
				meta->barPosition /= fixQuotient;
			}

		}

		void MidiContext::metaSetSignature(int upper, int lower)
		{
			Base::metaSetSignature(upper, lower);
			auto meta = voiceMetaData<MidiContext::VoiceMetaData>();
			auto sigEvent = fm::midi::Event::MetaSignature(upper, lower);
			sigEvent.absPosition(currentPosition());
			addEvent(sigEvent, currentMidiInstruments());
		}

		void MidiContext::metaSetChannel(int channel)
		{
			auto meta = trackMetaData<MidiContext::TrackMetaData>();
			if (!meta) {
				FM_THROW(Exception, "meta data = null");
			}			
			for(auto &instrument : currentMidiInstruments())  {
				instrument->channel = channel;
			}
		}

		void MidiContext::metaSoundSelect(const MidiInstrumentDef &instrument)
		{
			metaSoundSelect(instrument.cc, instrument.pc, instrument.channel, instrument.trackId);
		}

		void MidiContext::metaSoundSelect(int cc, int pc, int channel, TrackId trackId)
		{			
			auto ev = fm::midi::Event();
			ev.channel(channel);
			ev.eventType(fm::midi::Controller);
			ev.parameter1(0);
			ev.parameter2(cc);
			ev.absPosition(currentPosition());
			addEvent(ev, trackId);
			ev.eventType(fm::midi::ProgramChange);
			ev.parameter1(pc);
			ev.parameter2(0);
			addEvent(ev, trackId);
		}

		MidiInstrumentDef * MidiContext::getMidiInstrumentDef(const fm::String &uname)
		{
			auto it = midiInstrumentDefs_.find(uname);
			if (it == midiInstrumentDefs_.end()) {
				return nullptr;
			}
			return &it->second;
		}

		AInstrumentDef * MidiContext::getInstrumentDef(const fm::String &uname) 
		{
			return getMidiInstrumentDef(uname);
		}

		void MidiContext::metaSetVolume(int volume)
		{
			Base::metaSetVolume(volume);
			auto meta = voiceMetaData<MidiContext::VoiceMetaData>();
			auto trackMeta = trackMetaData<MidiContext::TrackMetaData>();
			if (!meta || !trackMeta) {
				FM_THROW(Exception, "meta data = null");
			}				
			auto midiVol = meta->volume * fm::midi::MaxMidiValue / MAX_VOLUME;
			auto channel = DEFAULT_CHANNEL;
			auto ev = fm::midi::Event::CCVolume(channel, midiVol);
			ev.absPosition(currentPosition());
			addEvent(ev, currentMidiInstruments()); 
		}

		void MidiContext::metaSetPan(int val)
		{
			Base::metaSetPan(val);
			auto meta = voiceMetaData<MidiContext::VoiceMetaData>();
			auto trackMeta = trackMetaData<MidiContext::TrackMetaData>();
			if (!meta || !trackMeta) {
				FM_THROW(Exception, "meta data = null");
			}				
			auto midiVal = meta->pan * fm::midi::MaxMidiValue / MAX_VOLUME;
			auto channel = DEFAULT_CHANNEL;
			auto ev = fm::midi::Event::CCPan(channel, midiVal);
			ev.absPosition(currentPosition());
			addEvent(ev, currentMidiInstruments()); 
		}
		namespace {
			fm::midi::Event __createVolumeEvent(MidiInstrumentDef &def, fm::Ticks pos)
			{
				auto midiVol = def.volume * fm::midi::MaxMidiValue / MidiContext::MAX_VOLUME;
				auto ev = fm::midi::Event::CCVolume(def.channel, midiVol);
				ev.absPosition(pos);
				return ev;
			}
			fm::midi::Event __createPanEvent(MidiInstrumentDef &def, fm::Ticks pos)
			{
				auto midiVal = def.pan * fm::midi::MaxMidiValue / MidiContext::MAX_PAN;
				auto ev = fm::midi::Event::CCPan(def.channel, midiVal);
				ev.absPosition(pos);
				return ev;
			}			
		}

		void MidiContext::metaSetInstrumentConfig(const fm::String &uname, const Event::Args &args)
		{
			if (args.size() < 3 || args.size() % 2 == 0) {
				FM_THROW(Exception, "not enough values for " + fm::String(SHEET_META__SET_INSTRUMENT_CONFIG) +  ": " + uname);
			}
			auto instrumentDef = getMidiInstrumentDef(uname);
			if (instrumentDef == nullptr) {
				FM_THROW(Exception, "instrumentDef not found: " + uname);
			}
			auto argsBegin = args.begin() + 1;
			auto argsEnd = args.end();
			auto argsExceptFirst = Event::Args(argsBegin, argsEnd);
			instrumentDef->volume = sheet::getArgValueFor<int>(SHEET_META__SET_INSTRUMENT_CONFIG_VOLUME, argsExceptFirst, instrumentDef->volume);
			instrumentDef->pan = sheet::getArgValueFor<int>(SHEET_META__SET_INSTRUMENT_CONFIG_PAN, argsExceptFirst, instrumentDef->pan);
			auto argIt = std::find(argsBegin, argsEnd, SHEET_META__SET_VOICING_STRATEGY);
			// assign voicingStrategy
			if (argIt != argsEnd) {
				auto &wm = fm::getWerckmeister();
				auto itName = argIt + 1;
				if (itName == argsEnd) {
					FM_THROW(Exception, fm::String("missing argument for ") + SHEET_META__SET_VOICING_STRATEGY);
				}
				instrumentDef->voicingStrategy = wm.getVoicingStrategy(*itName);
				instrumentDef->voicingStrategy->setArguments(Event::Args(itName, argsEnd));
			}
			// velocity overrides
			auto assignIfSet = [&argsExceptFirst, instrumentDef, this](const fm::String &expression){
				auto foundValue = sheet::getArgValueFor<int>(expression, argsExceptFirst);
				if (!foundValue.first) {
					return;
				}
				if (foundValue.second < 0 || foundValue.second > 100) {
					FM_THROW(Exception, "invalid value for: " + expression);
				}
				auto exprValue = getExpression(expression);
				instrumentDef->velocityOverride[exprValue] = foundValue.second;
			};
			for(const auto &keyValue : expressionMap_) {
				assignIfSet(keyValue.first);
			}
		}

		void MidiContext::metaSetInstruments(const std::vector<fm::String> &unames)
		{
			std::vector<MidiInstrumentDef*> instruments;
			Base::metaSetInstruments(unames);
			auto voiceMeta = voiceMetaData<MidiContext::VoiceMetaData>();
			auto trackMeta = trackMetaData<MidiContext::TrackMetaData>();
			if (voiceMeta) {
				voiceMeta->instruments.clear();
			}
			if (trackMeta) {
				trackMeta->instruments.clear();
			}
			for (const auto &uname : unames)  {
				// find instrumentDef defs and assign them to the meta data of the voice
				auto instrumentDef = getMidiInstrumentDef(uname);
				if (instrumentDef == nullptr) {
					FM_THROW(Exception, "instrument " + uname + " not found");
				}
				instruments.push_back(instrumentDef);

				// send instrumentDef name meta event
				auto trackName = fm::midi::Event::MetaTrack(uname);
				trackName.absPosition(0);
				addEvent(trackName, instrumentDef->trackId);
				
				if (false) {
					voiceMeta->instruments.push_back(*instrumentDef);
				} else {
					trackMeta->instruments.push_back(*instrumentDef);
				}
				if (!instrumentDef->deviceName.empty()) {
					addDeviceChangeEvent(*instrumentDef);
				}
				metaSoundSelect(*instrumentDef);
				// volume
				addEvent(__createVolumeEvent(*instrumentDef, currentPosition()), instrumentDef->trackId);
				// pan
				addEvent(__createPanEvent(*instrumentDef, currentPosition()), instrumentDef->trackId);
			}
		}

		void MidiContext::addDeviceChangeEvent(const MidiInstrumentDef &instrument)
		{
			if (instrument.deviceName.empty()) {
				return;
			}
			fm::midi::CustomMetaData cdata;
			cdata.type = fm::midi::CustomMetaData::SetDevice;
			std::string ndeviceName = instrument.deviceName;
			cdata.data = fm::midi::CustomMetaData::Data(ndeviceName.begin(), ndeviceName.end());
			auto ev = fm::midi::Event::MetaCustom(cdata);
			ev.absPosition(0);
			addEvent(ev, instrument.trackId);
		}

		void MidiContext::setMidiInstrumentDef(const fm::String &uname, const MidiInstrumentDef &def)
		{
			if (midiInstrumentDefs_.find(uname) != midiInstrumentDefs_.end()) {
				FM_THROW(Exception, "instrument " + uname + " already defined");
			}
			midiInstrumentDefs_.insert({ uname, def });
			midiInstrumentDefs_[uname].id = midiInstrumentDefs_.size();
		}

		void MidiContext::metaInstrument(const fm::String &uname, int channel, int cc, int pc)
		{
			metaInstrument(uname, fm::String(), channel, cc, pc);
		}

		void MidiContext::metaInstrument(const fm::String &uname, const fm::String &deviceName, int channel, int cc, int pc)
		{
			MidiInstrumentDef def;
			def.channel = channel;
			def.cc = cc;
			def.pc = pc;
			def.deviceName = deviceName;
			def.trackId = createTrack();		
			setMidiInstrumentDef(uname, def);
		}

		void MidiContext::processMeta(const fm::String &command, const std::vector<fm::String> &args)
		{
			try {
				if (command == SHEET_META__MIDI_INSTRUMENT_DEF) {
					auto name = getArgument<fm::String>(args, 0);
					std::size_t numArgs = args.size();
					if (numArgs == 4) {
						metaInstrument(getArgument<fm::String>(args, 0), getArgument<int>(args, 1), getArgument<int>(args, 2), getArgument<int>(args, 3));
						return;
					}
					if (numArgs == 5) {
						metaInstrument(getArgument<fm::String>(args, 0), getArgument<fm::String>(args, 1),getArgument<int>(args, 2), getArgument<int>(args, 3), getArgument<int>(args, 4));					
						return;
					}
					FM_THROW(Exception, "invalid number of arguments for instrumentDef: " + name );
				}
				if (command == SHEET_META__SET_INSTRUMENT_CONFIG) {
					metaSetInstrumentConfig(getArgument<fm::String>(args, 0), args);
					return;
				}
			} catch(const std::exception &ex) {
				FM_THROW(Exception, "failed to process " + command
									+"\n" + ex.what());
			}	
			catch(...) {
				FM_THROW(Exception, "failed to process " + command);
			}
			Base::processMeta(command, args);	
		}
		AContext::TrackId MidiContext::createMasterTrack()
		{
			auto trackId = Base::createMasterTrack();
			// send name meta event
			auto trackName = fm::midi::Event::MetaTrack(SHEET_MASTER_TRACKNAME);
			trackName.absPosition(0);
			addEvent(trackName, trackId); 
			return trackId;
		}
	}
}