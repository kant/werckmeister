#pragma once

#include "ASelectorBase.h"
#include <compiler/IEventInformationServer.h>

namespace conductor
{
    /// <selector name="toPosition">
    ///     Selects any event where its position is before a given position.
    /// see also: [toPosition](#toPosition).
    /// ## example, select the events position &lt; 8 quarters:
    /// ```
    /// toPosition(8) {...}
    /// ```
    /// ## using cue markers instead of quarter numbers
    /// ```\n
    /// c d e f | /cue: myMark/ g a b 'c\n
    /// ```\n
    /// ```\n
    /// toPosition(@myMark) {...}\n
    /// ```\n
    /// &gt; In order to work properly, a cue name must occur only once in a document.    
    /// </selector>
    /// <param name="beat" type="quarters"></param>
    class ToPosition : public ASelectorBase
    {
    public:
        typedef ASelectorBase Base;
        ToPosition(compiler::IEventInformationServerPtr eventInformationServer) : Base(eventInformationServer)
        {
        }
        virtual bool isMatch(const documentModel::ConductionSelector::Arguments &, const EventWithMetaInfo &) const override;
        virtual ~ToPosition() = default;
    };
}
