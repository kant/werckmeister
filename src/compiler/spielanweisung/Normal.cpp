#include "Normal.h"
#include "compiler/AContext.h"

namespace sheet {
    namespace compiler {

        void Normal::addEvent(AContext *ctx, const Event::Pitches &pitches, fm::Ticks duration)
        {
            auto meta = ctx->voiceMetaData();
			for (const auto &pitch : pitches)
			{
                NoteEvent note = ctx->createNote(pitch, duration);
                ctx->addNote(note, meta->position);
			}
			ctx->seek(duration);
        }

    }
}