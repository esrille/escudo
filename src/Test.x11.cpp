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
#include <X11/Xutil.h>
#include <X11/Xmu/Atoms.h>

#include <string>

// Since glutSetWindowTitle() only accpets a string in ASCII code,
// we have to implement our own version for now.
void setWindowTitle(const std::string& title)
{
    Atom name = XInternAtom(glXGetCurrentDisplay(), "_NET_WM_NAME", False);
    Atom utf8 = XInternAtom(glXGetCurrentDisplay(), "UTF8_STRING", False);
    XChangeProperty(glXGetCurrentDisplay(), glXGetCurrentDrawable(), name, utf8,
                    8, PropModeReplace, (unsigned char*) title.c_str(),
                    title.length());
}

void setIcon(size_t n, size_t width, size_t height, uint32_t* image)
{
    Atom netWmIcon = XInternAtom(glXGetCurrentDisplay(), "_NET_WM_ICON", False);
    Atom cardinal = XInternAtom(glXGetCurrentDisplay(), "CARDINAL", False);
    size_t length = 2 + width * height;
    long buffer[length];
    buffer[0] = width;
    buffer[1] = height;
    for (size_t i = 0; i < width * height; ++i) {
        // Convert RGBA (OpenGL) to ARGB (X11); i.e., swap R and B.
        uint32_t argb = image[i] & 0xff00ff00;
        argb |= (image[i] >> 16) & 0x000000ff;
        argb |= (image[i] << 16) & 0x00ff0000;
        buffer[2 + i] = argb;
    }
    XChangeProperty(glXGetCurrentDisplay(), glXGetCurrentDrawable(), netWmIcon, cardinal, 32,
                    (n == 0) ? PropModeReplace : PropModeAppend,
                    reinterpret_cast<const unsigned char*>(buffer), length);
    XFlush(glXGetCurrentDisplay());
}
