/*
 * Copyright 2010-2013 Esrille Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLMEDIAELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLMEDIAELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLMediaElement.h>
#include "HTMLElementImp.h"

#include <org/w3c/dom/html/HTMLElement.h>
#include <org/w3c/dom/html/MediaError.h>
#include <org/w3c/dom/html/AudioTrackList.h>
#include <org/w3c/dom/html/VideoTrackList.h>
#include <org/w3c/dom/html/MediaController.h>
#include <org/w3c/dom/html/TextTrackList.h>
#include <org/w3c/dom/html/TextTrack.h>
#include <org/w3c/dom/html/TimeRanges.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class HTMLMediaElementImp : public ObjectMixin<HTMLMediaElementImp, HTMLElementImp>
{
public:
    HTMLMediaElementImp(DocumentImp* ownerDocument, const std::u16string& localName) :
        ObjectMixin(ownerDocument, localName)
    {
    }

    // Node - override
    virtual Node cloneNode(bool deep = true) {
        auto node = std::make_shared<HTMLMediaElementImp>(*this);
        node->cloneAttributes(this);
        if (deep)
            node->cloneChildren(this);
        return node;
    }

    // HTMLMediaElement
    html::MediaError getError();
    std::u16string getSrc();
    void setSrc(const std::u16string& src);
    std::u16string getCurrentSrc();
    std::u16string getCrossOrigin();
    void setCrossOrigin(const std::u16string& crossOrigin);
    unsigned short getNetworkState();
    std::u16string getPreload();
    void setPreload(const std::u16string& preload);
    html::TimeRanges getBuffered();
    void load();
    std::u16string canPlayType(const std::u16string& type);
    unsigned short getReadyState();
    bool getSeeking();
    double getCurrentTime();
    void setCurrentTime(double currentTime);
    double getDuration();
    unsigned long long getStartDate();
    bool getPaused();
    double getDefaultPlaybackRate();
    void setDefaultPlaybackRate(double defaultPlaybackRate);
    double getPlaybackRate();
    void setPlaybackRate(double playbackRate);
    html::TimeRanges getPlayed();
    html::TimeRanges getSeekable();
    bool getEnded();
    bool getAutoplay();
    void setAutoplay(bool autoplay);
    bool getLoop();
    void setLoop(bool loop);
    void play();
    void pause();
    std::u16string getMediaGroup();
    void setMediaGroup(const std::u16string& mediaGroup);
    html::MediaController getController();
    void setController(html::MediaController controller);
    bool getControls();
    void setControls(bool controls);
    double getVolume();
    void setVolume(double volume);
    bool getMuted();
    void setMuted(bool muted);
    bool getDefaultMuted();
    void setDefaultMuted(bool defaultMuted);
    html::AudioTrackList getAudioTracks();
    html::VideoTrackList getVideoTracks();
    html::TextTrackList getTextTracks();
    html::TextTrack addTextTrack(const std::u16string& kind);
    html::TextTrack addTextTrack(const std::u16string& kind, const std::u16string& label);
    html::TextTrack addTextTrack(const std::u16string& kind, const std::u16string& label, const std::u16string& language);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLMediaElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLMediaElement::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLMEDIAELEMENTIMP_H_INCLUDED
