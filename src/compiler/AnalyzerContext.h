#ifndef COMPILER_ANALYZERCONTEXT_H
#define COMPILER_ANALYZERCONTEXT_H

#include "MidiContext.h"
#include <list>

namespace sheet {
    namespace compiler {
        struct AnalyzerEvent: public Event {
            AnalyzerEvent(const Event &event) : Event(event) {} 
            fm::Ticks position = 0;
            fm::Ticks duration = 0;
        };
        struct BarEvent: public AnalyzerEvent {
            BarEvent(const Event &event) : AnalyzerEvent(event) {} 
            fm::Ticks barLength = 0;
            int barCount = -1;
        };
        struct AnalyzerData {
            typedef std::list<BarEvent> BarEvents;
            typedef std::list<AnalyzerEvent> AnalyzerEvents;
            BarEvents barEvents;
            AnalyzerEvents analyzerEvents;
        };
        class AnalyzerContext : public MidiContext {
        public:
            typedef MidiContext Base;
            AnalyzerData *analyzerData = nullptr;
            virtual void newBar(const Event &newBarEvent) override;
            virtual void setEvent(const Event &event) override;
            virtual void metaSetTempo(double bpm) override {}
            virtual void addEvent(const fm::midi::Event &ev, TrackId trackId) override {}
        protected:
            bool isCurrentTrackRelevant() const;
        };
    }
}

#endif