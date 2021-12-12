#pragma once

#include "AModification.h"
#include <lua/ALuaScript.h>
#include <com/common.hpp>
#include <forward.hpp>
#include <lua/ALuaWithParameter.h>

#define LUA_MODIFICATION_FENTRY "perform"

namespace compiler
{
    class LuaModification : public AModification,
                            public lua::ALuaScript,
                            public lua::ALuaWithParameter
    {
    public:
        typedef VoicingStrategy Base;
        typedef lua::ALuaScript LuaBase;
        LuaModification(const com::String &path);
        virtual ~LuaModification() = default;
        virtual bool canExecute() const override;
        virtual void assertCanExecute() const override;
        virtual void perform(IContextPtr ctx, Events &events) override;
        virtual ParametersByNames &getParameters() override;

    protected:
        void pushEvents(IContextPtr ctx, const Events &events);
        Events popEvents();
        void popNoteEvent(documentModel::Event &event);
        void popPitchBendEvent(documentModel::Event &event);

    private:
        documentModel::Event::Args args_;
    };
}
