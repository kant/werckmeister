#ifndef COMPILER_ANALYZERCONTEXT_H
#define COMPILER_ANALYZERCONTEXT_H

#include "MidiContext.h"
#include <list>

namespace sheet {
    namespace compiler {
        struct BarEvent: public Event {
            BarEvent(const Event &event) : Event(event) {} 
            fm::Ticks position = 0;
            fm::Ticks barLength = 0;
            int barCount = -1;
        };
        struct AnalyzerData {
            typedef std::list<BarEvent> BarEvents;
            BarEvents barEvents;
        };
        class AnalyzerContext : public MidiContext {
        public:
            typedef MidiContext Base;
            AnalyzerData *analyzerData = nullptr;
            virtual void newBar(const Event &newBarEvent) override;
        };
    }
}

#endif