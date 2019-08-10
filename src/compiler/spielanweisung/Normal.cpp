#include "Normal.h"
#include "compiler/AContext.h"

namespace sheet {
    namespace compiler {

        void Normal::addEvent(AContext *ctx, const Event::Pitches &pitches, fm::Ticks duration)
        {
            auto meta = ctx->voiceMetaData();
			for (const auto &pitch : pitches)
			{
				ctx->addEvent(pitch, duration);
			}
			ctx->seek(duration);
        }

    }
}