#include "Normal.h"
#include "compiler/AContext.h"

namespace sheet {
    namespace compiler {

        void Normal::addEvent(AContext *ctx, const Event &ev)
        {
            auto meta = ctx->voiceMetaData();
			for (const auto &pitch : ev.pitches)
			{
                NoteEvent note = ctx->createNote(pitch, ev.duration, ev.offset);
                ctx->addNote(note);
			}
			ctx->seek(ev.duration);
        }

    }
}