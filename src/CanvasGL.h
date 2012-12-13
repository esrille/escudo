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

#ifndef ES_CANVAS_GL_H
#define ES_CANVAS_GL_H

#include "Canvas.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

class Canvas::Impl
{
    int width;
    int height;
    GLuint frameBuffer;
    GLuint renderBuffer;
    GLuint texture;

    GLuint savedFrameBuffer;
    static GLuint currentFrameBuffer;

public:
    Impl() :
        width(0),
        height(0),
        frameBuffer(0),
        renderBuffer(0),
        texture(0)
    {}

    ~Impl() {
        if (0 < width && 0 < height)
            shutdown();
    }

    void setup(int width, int height);
    void shutdown();

    void beginRender(unsigned backgroundColor);
    void endRender();

    int getWidth() const {
        return width;
    }
    int getHeight() const {
        return height;
    }
    GLuint getFrameBuffer() const {
        return frameBuffer;
    }
    GLuint getRenderBuffer() const {
        return renderBuffer;
    }
    GLuint getTexture() const {
        return texture;
    }

    void render(int width, int height);
};

#endif  // ES_CANVAS_GL_H
