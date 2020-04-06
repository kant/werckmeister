#include "AnalyzerContext.h"
#include <compiler/metaCommands.h>

namespace sheet {
    namespace compiler {
        void AnalyzerContext::newBar(const Event &newBarEvent)
        {
            auto voiceMeta = voiceMetaData();
            int barCount = voiceMeta->barCount;
            fm::Ticks barlength = voiceMeta->barPosition; // bar position means the actual written ticks per bar
                                                         // voiceMeta->barLength means the length per definiton
            Base::newBar(newBarEvent);
            if (this->analyzerData == nullptr) {
                return;
            }
            auto trackData = this->trackMetaData(this->track());
            bool isRelevant = trackData->type == SHEET_META__TRACK_META_KEY_TRACK_TYPE_DEFAULT
                || trackData->type == SHEET_META__TRACK_META_VALUE_TYPE_ACCOMP;
            if (!isRelevant) {
                return;
            }
            BarEvent barEvent(newBarEvent);
            barEvent.position = voiceMeta->position;
            barEvent.barCount = barCount;
            barEvent.barLength = barlength;
            this->analyzerData->barEvents.push_back(barEvent);
        }
    }
}