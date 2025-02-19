#ifndef SEL_INSTRUMENT_H
#define SEL_INSTRUMENT_H

#include "ISelector.h"

namespace sheet
{
    namespace conductor
    {
        /// <selector name="instrumentSelector">
        /// Selects any event which belongs to one of the given instruments.
        /// *there is a known [issue](https://github.com/werckme/werckmeister/issues/230) related to instrumentSection*
        /// ## example:
        /// ```
        /// instrument(bass) {...}\n
        /// instrument(bass drums) {...}\n
        /// ```
        /// </selector>
        /// <param name="instrument name" type="instrumentName+"></param>
        class Instrument : public ISelector
        {
        public:
            virtual bool isMatch(const ConductionSelector::Arguments&, const EventWithMetaInfo&) const override;
            virtual ~Instrument() = default;
        };
    }
}

#endif