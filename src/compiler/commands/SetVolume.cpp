#include "SetVolume.h"
#include <compiler/context/IContext.h>

namespace documentModel {
    namespace compiler {
        void SetVolume::execute(IContextPtr  context)
        {
            auto setTo = parameters[argumentNames.SetVolume.SetTo].value<double>();
            context->setVolume(setTo);
        }

    }
}