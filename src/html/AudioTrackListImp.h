// Generated by esidl 0.3.0.
// This file is expected to be modified for the Web IDL interface
// implementation.  Permission to use, copy, modify and distribute
// this file in any software license is hereby granted.

#ifndef ORG_W3C_DOM_BOOTSTRAP_AUDIOTRACKLISTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_AUDIOTRACKLISTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/AudioTrackList.h>
#include "EventTargetImp.h"

#include <org/w3c/dom/events/EventTarget.h>
#include <org/w3c/dom/events/EventHandlerNonNull.h>
#include <org/w3c/dom/html/AudioTrack.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class AudioTrackListImp : public ObjectMixin<AudioTrackListImp, EventTargetImp>
{
public:
    // AudioTrackList
    unsigned int getLength();
    html::AudioTrack getElement(unsigned int index);
    html::AudioTrack getTrackById(const std::u16string& id);
    events::EventHandlerNonNull getOnchange();
    void setOnchange(events::EventHandlerNonNull onchange);
    events::EventHandlerNonNull getOnaddtrack();
    void setOnaddtrack(events::EventHandlerNonNull onaddtrack);
    events::EventHandlerNonNull getOnremovetrack();
    void setOnremovetrack(events::EventHandlerNonNull onremovetrack);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::AudioTrackList::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::AudioTrackList::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_AUDIOTRACKLISTIMP_H_INCLUDED