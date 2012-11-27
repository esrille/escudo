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

#include "CanvasGL.h"

GLuint Canvas::Impl::currentFrameBuffer = 0;

void Canvas::Impl::setup(int w, int h)
{
    if (width != 0 || height != 0 || w < 0 || h < 0)
        return;

    width = w;
    height = h;

    // Setup texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // Setup renderBuffer
    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, width, height);

    // Setup frameBuffer
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Canvas::Impl::shutdown()
{
    if (width == 0 && height == 0)
        return;

    glDeleteFramebuffers(1, &frameBuffer);
    glDeleteRenderbuffers(1, &renderBuffer);
    glDeleteTextures(1, &texture);

    frameBuffer = 0;
    renderBuffer = 0;
    texture = 0;
    width = height = 0;
}

void Canvas::Impl::beginRender()
{
    GLint v[4];
    glGetIntegerv(GL_VIEWPORT, v);
    GLint viewportHeight = v[3];

    savedFrameBuffer = currentFrameBuffer;
    currentFrameBuffer = frameBuffer;
    glBindFramebuffer(GL_FRAMEBUFFER, currentFrameBuffer);

    GLfloat m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    float x = m[12];
    float y = m[13];

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glTranslatef(x, viewportHeight - height, 0.0f);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Canvas::Impl::endRender()
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    currentFrameBuffer = savedFrameBuffer;
    glBindFramebuffer(GL_FRAMEBUFFER, currentFrameBuffer);
}

void Canvas::Impl::render(int w, int h)
{
    if (texture == 0)
        return;

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(1.0f / width, 1.0f / height, 0.0f);
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBegin(GL_QUADS);
        glTexCoord2f(0, h);
        glVertex2f(0, 0);
        glTexCoord2f(w, h);
        glVertex2f(w, 0);
        glTexCoord2f(w, 0);
        glVertex2f(w, h);
        glTexCoord2f(0, 0);
        glVertex2f(0, h);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

Canvas::Canvas() :
    pimpl(new Impl())
{
}

Canvas::~Canvas()
{
}

void Canvas:: setup(int width, int height)
{
    pimpl->setup(width, height);
}

void Canvas::shutdown()
{
    pimpl->shutdown();
}

void Canvas::beginRender()
{
    pimpl->beginRender();
}

void Canvas::endRender()
{
    pimpl->endRender();
}

int Canvas::getWidth() const
{
    return pimpl->getWidth();
}

int Canvas::getHeight() const
{
    return pimpl->getHeight();
}

void Canvas::render(int width, int height)
{
    return pimpl->render(width, height);
}
