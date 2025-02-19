#ifndef COMPILER_MIDICONTEXT_H
#define COMPILER_MIDICONTEXT_H

#include "AContext.h"
#include "forward.hpp"
#include <fm/midi.hpp>
#include <map>
#include <vector>
#include <fm/config.hpp>
#include <fm/ILogger.h>
#include <compiler/ICompilerVisitor.h>

#define SHEET_MASTER_TRACKNAME "master track"

namespace sheet {
    namespace compiler {
        class MidiContext : public AContext {
			friend struct MidiInstrumentDef;
			friend struct InstrumentSectionDef;
		private:
			fm::midi::MidiPtr midi_;
			ICompilerVisitorPtr _compilerVisitor;
			fm::ILoggerPtr _logger;
		public:
			typedef AContext Base;
			typedef std::unordered_map<fm::String, AInstrumentDefPtr> InstrumentDefs;
			typedef std::vector<AInstrumentDefPtr> InstrumentDefContainer;
			MidiContext(fm::midi::MidiPtr midiFile,
				fm::IDefinitionsServerPtr definitionsServer,
				ICompilerVisitorPtr compilerVisitor,
				fm::ILoggerPtr logger);

			struct VoiceMetaData : sheet::compiler::VoiceMetaData {
				fm::Ticks positionOffset = 0;
			};
			struct TrackMetaData : sheet::compiler::TrackMetaData {
				TrackMetaData();
				AInstrumentDefPtr instrument;
			};
			fm::midi::MidiPtr midiFile() const { return midi_; }
			virtual TrackId createTrackImpl() override;
			virtual VoiceId createVoiceImpl() override;
			virtual void renderPitch(const PitchDef &pitch, fm::Ticks absolutePosition, double velocity, fm::Ticks duration) override;
			virtual void addEvent(const fm::midi::Event &ev);
			virtual void addEvent(const fm::midi::Event &ev, TrackId trackId);
			virtual void renderPitchbend(double value, fm::Ticks absolutePosition) override;
			virtual void startEvent(const PitchDef &pitch, fm::Ticks absolutePosition, double velocity) override;
			virtual void stopEvent(const PitchDef &pitch, fm::Ticks absolutePosition) override;
			virtual void selectMidiSound(int bankMsb, int bankLsb, int pc);
			virtual void defineInstrumentSection(const fm::String& uname, std::list<fm::String> &sectionInstrumentUNames);
			virtual void defineMidiInstrument(const fm::String &uname, int channel, int bankMsb, int bankLsb, int pc);
			virtual void defineMidiInstrument(const fm::String &uname, const fm::String &deviceName, int channel, int bankMsb, int bankLsb, int pc);
			virtual void setInstrument(const fm::String &uname) override;
			virtual void setTempo(double bpm) override;
			virtual void setVolume(double volume, fm::Ticks relativePosition = 0) override;
			virtual void setPan(double val) override;
			virtual void addCue(const fm::String &text, fm::Ticks absolutePosition);
			virtual void setSignature(int upper, int lower) override;
			/**
			 * sends a custom meta event containing a device name
			 */
			virtual void addDeviceChangeEvent(const fm::String &deviceName, fm::Ticks position);
			virtual void addSetInstrumentEvent(const fm::String& instrumentName, fm::Ticks position);
			virtual AInstrumentDefPtr getInstrumentDef(const fm::String &uname) override;
			virtual AInstrumentDefPtr currentInstrumentDef() override;
			const InstrumentDefs & instrumentDefs() const { return this->instrumentDefs_; }
			virtual IContextPtr createNewContext() const;
			virtual void clear() override;
			static int toMidiPitch(const PitchDef &pitch);
		protected:
			void applyContextInfo(fm::midi::Event &ev) const;
			virtual Base::VoiceMetaDataPtr createVoiceMetaData() override;
			virtual Base::TrackMetaDataPtr createTrackMetaData() override;
			void addInstrumentDef(const fm::String &uname, AInstrumentDefPtr def);
			virtual TrackId createMasterTrack() override;
			int toMidiVelocity(double velocity);
			void setInstrument(std::shared_ptr<MidiInstrumentDef> def);
			void setInstrument(std::shared_ptr<InstrumentSectionDef> def);
		private:
			InstrumentDefs instrumentDefs_;
        };
		typedef std::shared_ptr<MidiContext> MidiContextPtr;
    }
}

#endif