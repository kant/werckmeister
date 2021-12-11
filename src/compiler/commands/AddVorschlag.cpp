#include "AddVorschlag.h"
#include <compiler/context/IContext.h>
#include <com/werckmeister.hpp>
#include <compiler/spielanweisung/spielanweisungen.h>
#include <compiler/spielanweisung/Vorschlag.h>


namespace sheet {
    namespace compiler {
        void AddVorschlag::execute(IContextPtr  context)
        {
            auto &wm = com::getWerckmeister();
			auto meta = context->voiceMetaData();
			meta->spielanweisungOnce = wm.getSpielanweisung(SHEET_SPIELANWEISUNG_VORSCHLAG);
			auto vorschlag = std::dynamic_pointer_cast<Vorschlag>(meta->spielanweisungOnce);
			vorschlag->vorschlagNote = AUsingAnEvent::event();
        }
    }
}