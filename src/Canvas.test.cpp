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

#include <iostream>

#include "CanvasGL.h"
#include "Test.util.h"

void reshape(int w, int h);

void render(int w, int h)
{
    glDisable(GL_TEXTURE_2D);

    reshape(w, h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    unsigned color = 0xffff0000;
    glColor4ub(color >> 16, color >> 8, color, color >> 24);
    glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(w, 0);
        glVertex2f(w, h);
        glVertex2f(0, h);
    glEnd();

    color = 0xff00ffff;
    glColor4ub(color >> 16, color >> 8, color, color >> 24);
    glLineWidth(5);
    glBegin(GL_LINES);
        glVertex2f(0, 0);
        glVertex2f(w, h / 2);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(w, h / 2);
        glVertex2f(0, h);
    glEnd();

    glFlush();
}

Canvas canvas;

void myDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    canvas.render(816, 1056);

    glutSwapBuffers();  // This would block until the sync happens
}

int main(int argc, char* argv[])
{
    init(&argc, argv);

    glutDisplayFunc(myDisplay);

    canvas.setup(816, 1056);
    canvas.beginRender(0xffffffff);
    render(816, 1056);
    canvas.endRender();

    glutMainLoop();

    canvas.shutdown();
}
