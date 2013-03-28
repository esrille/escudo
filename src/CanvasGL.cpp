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

    if (GLEW_ARB_framebuffer_object) {
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
    } else {
        // Setup renderBuffer
        glGenRenderbuffersEXT(1, &renderBuffer);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderBuffer);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_STENCIL_EXT, width, height);

        // Setup frameBuffer
        glGenFramebuffersEXT(1, &frameBuffer);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBuffer);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture, 0);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, renderBuffer);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, renderBuffer);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }
}

void Canvas::Impl::shutdown()
{
    if (width == 0 && height == 0)
        return;

    if (GLEW_ARB_framebuffer_object) {
        glDeleteFramebuffers(1, &frameBuffer);
        glDeleteRenderbuffers(1, &renderBuffer);
    } else {
        glDeleteFramebuffersEXT(1, &frameBuffer);
        glDeleteRenderbuffersEXT(1, &renderBuffer);
    }
    if (texture) {
        glDeleteTextures(1, &texture);
        texture = 0;
    }
    if (translucent) {
        glDeleteTextures(1, &translucent);
        translucent = 0;
    }
    frameBuffer = 0;
    renderBuffer = 0;
    width = height = 0;
}

void Canvas::Impl::beginRender(unsigned backgroundColor)
{
    GLint v[4];
    glGetIntegerv(GL_VIEWPORT, v);
    GLint viewportHeight = v[3];

    savedFrameBuffer = currentFrameBuffer;
    currentFrameBuffer = frameBuffer;
    if (GLEW_ARB_framebuffer_object)
        glBindFramebuffer(GL_FRAMEBUFFER, currentFrameBuffer);
    else
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, currentFrameBuffer);

    GLfloat m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    float x = m[12];
    float y = m[13];

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glTranslatef(-x, viewportHeight - (y + height), 0.0f);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glClearColor(((backgroundColor >> 16) & 255) / 255.0f,
                 ((backgroundColor >> 8) & 255) / 255.0f,
                 (backgroundColor & 255) / 255.0f,
                 ((backgroundColor >> 24) & 255) / 255.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Canvas::Impl::endRender()
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    currentFrameBuffer = savedFrameBuffer;
    if (GLEW_ARB_framebuffer_object)
        glBindFramebuffer(GL_FRAMEBUFFER, currentFrameBuffer);
    else
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, currentFrameBuffer);
}

void Canvas::Impl::beginTranslucent()
{
    glPushMatrix();
    if (translucent == 0) {
        glGenTextures(1, &translucent);
        glBindTexture(GL_TEXTURE_2D, translucent);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    }
    glFlush();
    if (GLEW_ARB_framebuffer_object)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, translucent, 0);
    else
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, translucent, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Canvas::Impl::endTranslucent(float alpha)
{
    glPopMatrix();
    glFlush();
    if (GLEW_ARB_framebuffer_object)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    else
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture, 0);
    alphaBlend(width, height, alpha, translucent);
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

void Canvas::Impl::alphaBlend(int w, int h, float alpha, GLuint tex)
{
    if (tex == 0)
        return;

    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(1.0f / width, 1.0f / height, 0.0f);
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glColor4f(alpha, alpha, alpha, alpha);
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

    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
}

Canvas::Canvas() :
    pimpl(new Impl())
{
}

Canvas::~Canvas()
{
}

void Canvas::setup(int width, int height)
{
    pimpl->setup(width, height);
}

void Canvas::shutdown()
{
    pimpl->shutdown();
}

void Canvas::beginRender(unsigned backgroundColor)
{
    pimpl->beginRender(backgroundColor);
}

void Canvas::endRender()
{
    pimpl->endRender();
}

void Canvas::beginTranslucent()
{
    pimpl->beginTranslucent();
}

void Canvas::endTranslucent(float alpha)
{
    pimpl->endTranslucent(alpha);
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

void Canvas::alphaBlend(int width, int height, float alpha)
{
    return pimpl->alphaBlend(width, height, alpha, pimpl->getTexture());
}
