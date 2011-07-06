/*
 * Copyright 2010, 2011 Esrille Inc.
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

#ifndef STYLESHEET_IMP_H
#define STYLESHEET_IMP_H

#include <Object.h>
#include <org/w3c/dom/stylesheets/StyleSheet.h>

#include <org/w3c/dom/stylesheets/MediaList.h>
#include <org/w3c/dom/Node.h>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class StyleSheetImp : public ObjectMixin<StyleSheetImp>
{
private:
    bool disabled;

public:
    StyleSheetImp();

    // StyleSheet
    virtual std::u16string getType() __attribute__((weak));
    virtual std::u16string getHref() __attribute__((weak));
    virtual Node getOwnerNode() __attribute__((weak));
    virtual stylesheets::StyleSheet getParentStyleSheet() __attribute__((weak));
    virtual std::u16string getTitle() __attribute__((weak));
    virtual stylesheets::MediaList getMedia() __attribute__((weak));
    virtual void setMedia(std::u16string media) __attribute__((weak));
    virtual bool getDisabled() __attribute__((weak));
    virtual void setDisabled(bool disabled) __attribute__((weak));
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return stylesheets::StyleSheet::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return stylesheets::StyleSheet::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // STYLESHEET_IMP_H
