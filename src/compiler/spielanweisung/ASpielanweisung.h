#ifndef ASPIELANWEISUNG_HPP
#define ASPIELANWEISUNG_HPP

#include <com/units.hpp>
#include <sheet/objects/Event.h>
#include <compiler/modification/AModification.h>

namespace sheet {
    namespace compiler {
        class AContext;
        class ASpielanweisung : public AModification {
        protected:
            ASpielanweisung() = default;
        public:
            virtual ~ASpielanweisung() = default;
        };
    }
}

#endif