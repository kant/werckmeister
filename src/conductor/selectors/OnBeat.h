#ifndef SEL_ATBEAT_H
#define SEL_ATBEAT_H

#include "ISelector.h"

namespace sheet
{
    namespace conductor
    {
        /// <selector name="onBeat">
        /// Selects an event which appears on a given beat or a list of beats
        /// see also: [fromBeat](#fromBeat), [toBeat](#toBeat).
        /// ## example, select the events at beat 1 and 3: 
        /// ```
        /// onBeat(1 3) {...}
        /// ```
        /// </selector>
        /// <param name="beat" type="quarters+"></param>
        class OnBeat : public ISelector
        {
        public:
            virtual bool isMatch(const ConductionSelector::Arguments&, const EventWithMetaInfo&) const override;
            virtual ~OnBeat() = default;
        };
    }
}

#endif