#include "AnalyzerContext.h"
#include <compiler/metaCommands.h>

namespace sheet {
    namespace compiler {
        void AnalyzerContext::newBar(const Event &newBarEvent)
        {
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
            barEvent.position = this->currentPosition();
            this->analyzerData->barEvents.push_back(barEvent);
        }
    }
}