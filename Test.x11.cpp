/*
 * Copyright 2012 Esrille Inc.
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

#include <GL/glx.h>
#include <X11/Xatom.h>
#include <X11/Xmu/Atoms.h>

#include <string>

// Since glutSetWindowTitle() only accpets a string in ASCII code,
// we have to implement our own version for now.
void setWindowTitle(const std::string& title)
{
    XTextProperty text;
    text.value = (unsigned char*) title.c_str();
    text.encoding = XA_UTF8_STRING(glXGetCurrentDisplay());
    text.format = 8;
    text.nitems = title.length();
    XSetWMName(glXGetCurrentDisplay(), glXGetCurrentDrawable(), &text);
}
