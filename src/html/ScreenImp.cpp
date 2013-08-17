/*
 * Copyright 2013 Esrille Inc.
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

#include "ScreenImp.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "MediaListImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

unsigned int ScreenImp::getDPI() const
{
    return 96;  // TODO: assume 96dpi for now
/*
    float in = glutGet(GLUT_SCREEN_WIDTH_MM) / 25.4f;
    if (in == 0.0f)
        return 96;
    return glutGet(GLUT_SCREEN_WIDTH) / (glutGet(GLUT_SCREEN_WIDTH_MM) / 25.4f);
 */
}

unsigned int ScreenImp::getDPPX() const
{
    return 1;   // TODO: assume one for now
}

unsigned int ScreenImp::getColor() const
{
    return std::min(glutGet(GLUT_WINDOW_RED_SIZE), std::min(glutGet(GLUT_WINDOW_GREEN_SIZE), glutGet(GLUT_WINDOW_BLUE_SIZE)));
}

unsigned int ScreenImp::getColorIndex() const
{
    return glutGet(GLUT_WINDOW_COLORMAP_SIZE);
}

unsigned int ScreenImp::getMonochrome() const
{
    return 0;   // TODO: assume a color device for now
}

unsigned int ScreenImp::getScan() const
{
    return MediaListImp::Progressive;
}

unsigned int ScreenImp::getGrid() const
{
    return 0;   // TODO: assume a bitmap device for now
}

unsigned int ScreenImp::getAvailWidth()
{
    return glutGet(GLUT_SCREEN_WIDTH);
}

unsigned int ScreenImp::getAvailHeight()
{
    return glutGet(GLUT_SCREEN_HEIGHT);
}

unsigned int ScreenImp::getWidth()
{
    return glutGet(GLUT_SCREEN_WIDTH);
}

unsigned int ScreenImp::getHeight()
{
    return glutGet(GLUT_SCREEN_HEIGHT);
}

unsigned int ScreenImp::getColorDepth()
{
    return glutGet(GLUT_WINDOW_RED_SIZE) + glutGet(GLUT_WINDOW_GREEN_SIZE) + glutGet(GLUT_WINDOW_BLUE_SIZE);
}

unsigned int ScreenImp::getPixelDepth()
{
    return glutGet(GLUT_WINDOW_RED_SIZE) + glutGet(GLUT_WINDOW_GREEN_SIZE) + glutGet(GLUT_WINDOW_BLUE_SIZE);
}

}
}
}
}
