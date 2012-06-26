/*
 * Copyright 2010-2012 Esrille Inc.
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

#include "font/FontManager.h"
#include "font/FontManagerBackEndGL.h"
#include "font/FontDatabase.h"

FontManagerBackEndGL backend;

unsigned int point = 48;

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(1.0 / FontTexture::Width, 1.0 / FontTexture::Height, 0.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1.0, -1.0, 1.0);
    glTranslatef(0.0, -h, 0.0);
}

double renderStrings(FontTexture* font, double y)
{
    float scale = point * 96.0f / 72.0f;
    glPushMatrix();
    glTranslatef(24.0, y, 0.0);
    glScalef(48.0 / scale, 48.0 / scale, 1.0);
    font->renderText(u"ABCDEFGHIJKLMNOPQRSTUVWXYZ(+-*/=)", 33);
    glPopMatrix();
    y += 30.0;

    glPushMatrix();
    glTranslatef(24.0, y, 0.0);
    glScalef(24.0 / scale, 24.0 / scale, 1.0);
    font->renderText(u"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789(+-*/=)", 69);
    glPopMatrix();
    y += 24.0;

    glPushMatrix();
    glTranslatef(24.0, y, 0.0);
    glScalef(16.0 / scale, 16.0 / scale, 1.0);
    font->renderText(u"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789(+-*/=)", 69);
    glPopMatrix();
    y += 16.0;

    glPushMatrix();
    glTranslatef(24.0, y, 0.0);
    glScalef(12.0 / scale, 12.0 / scale, 1.0);
    font->renderText(u"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789(+-*/=)", 69);
    glPopMatrix();
    y += 16.0;

    glPushMatrix();
    glTranslatef(24.0, y, 0.0);
    glScalef(10.0 / scale, 10.0 / scale, 1.0);
    font->renderText(u"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789(+-*/=)", 69);
    glPopMatrix();
    y += 64.0;

    return y;
}

double renderCJKStrings(FontTexture* font, double y)
{
    float scale = point * 96.0f / 72.0f;

    glPushMatrix();
    glTranslatef(24.0, y, 0.0);
    glScalef(48.0 / scale, 48.0 / scale, 1.0);
    backend.renderString("こんにちは世界 コーヒー いろはにほへと");
    glPopMatrix();
    y += 30.0;

    glPushMatrix();
    glTranslatef(24.0, y, 0.0);
    glScalef(24.0 / scale, 24.0 / scale, 1.0);
    backend.renderString("こんにちは世界 コーヒー いろはにほへと");
    glPopMatrix();
    y += 24.0;

    glPushMatrix();
    glTranslatef(24.0, y, 0.0);
    glScalef(16.0 / scale, 16.0 / scale, 1.0);
    backend.renderString("こんにちは世界 コーヒー いろはにほへと");
    glPopMatrix();
    y += 16.0;

    glPushMatrix();
    glTranslatef(24.0, y, 0.0);
    glScalef(12.0 / scale, 12.0 / scale, 1.0);
    backend.renderString("こんにちは世界 コーヒー いろはにほへと");
    glPopMatrix();
    y += 16.0;

    glPushMatrix();
    glTranslatef(24.0, y, 0.0);
    glScalef(10.0 / scale, 10.0 / scale, 1.0);
    backend.renderString("こんにちは世界 コーヒー いろはにほへと");
    glPopMatrix();
    y += 64.0;

    return y;
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(0.0, 0.0, 0.0);

    double y = 64.0f;
    backend.getFontFace(u"Liberation Sans");
    FontTexture* sans = backend.getFontTexture(point);

    backend.getFontFace(u"Liberation Serif");
    FontTexture* serif = backend.getFontTexture(point);

    backend.getFontFace(u"Liberation Mono");
    FontTexture* mono = backend.getFontTexture(point);

    y = renderStrings(sans, y);
    y = renderStrings(serif, y);
    y = renderStrings(mono, y);

    backend.getFontFace(u"IPAMincho");
    y = renderCJKStrings(backend.getFontTexture(point), y);
    backend.getFontFace(u"IPAGothic");
    y = renderCJKStrings(backend.getFontTexture(point), y);

#if 0
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_QUADS);
            glTexCoord2i(0 , 0);
            glVertex2f(0, y);
            glTexCoord2i(1024, 0);
            glVertex2f(1024, y);
            glTexCoord2i(1024, 1024);
            glVertex2f(1024, y + 1024);
            glTexCoord2i(0 , 1024);
            glVertex2f(0, y + 1024);
    glEnd();
#endif

    glutSwapBuffers();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1024, 1024);
    glutCreateWindow(argv[0]);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (2 <= argc) {
        point = atoi(argv[1]);
        if (point == 0)
            point = 32;
    }

    FontManager* manager = backend.getFontManager();
    FontDatabase::loadBaseFonts(manager);

    glutMainLoop();
}
