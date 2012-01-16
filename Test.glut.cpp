/*
 * Copyright 2011, 2012 Esrille Inc.
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

#include <GL/freeglut.h>

#include "WindowImp.h"
#include "Test.util.h"

using namespace org::w3c::dom::bootstrap;
using namespace org::w3c::dom;

extern html::Window window;

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1000.0, 1.0);

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (WindowImp* imp = static_cast<WindowImp*>(window.self()))
        imp->render();
    glutSwapBuffers();  // This would block until the sync happens
}

unsigned getCharKeyCode(int key)
{
    // US Qwerty
    static unsigned map[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 8, 9, 0, 0, 12, 13, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 8, 9, 0, 27, 0, 0, 0, 0,
        32, 49 /* ! */, 222 /* " */, 51 /* # */, 52 /* $ */, 53 /* % */, 55 /* & */, 222 /* ' */,
        57 /* ( */, 48 /* ) */, 56 /* * */, 187 /* + */, 188 /* , */, 189 /* - */, 190 /* . */, 191 /* / */,
        48 /* 0 */, 49 /* 1 */, 50 /* 2 */, 51 /* 3 */, 52 /* 4 */, 53 /* 5 */, 54 /* 6 */, 55 /* 7 */,
        56 /* 8 */, 57 /* 9 */, 186 /* : */, 186 /* ; */, 188 /* < */, 187 /* = */, 190 /* > */, 191 /* ? */,
        50 /* @ */, 65 /* A */, 66 /* B */, 67 /* C */, 68 /* D */, 69 /* E */, 70 /* F */, 71 /* G */,
        72 /* H */, 73 /* I */, 74 /* J */, 75 /* K */, 76 /* L */, 77 /* M */, 78 /* N */, 79 /* O */,
        80 /* P */, 81 /* Q */, 82 /* R */, 83 /* S */, 84 /* T */, 85 /* U */, 86 /* V */, 87 /* W */,
        88 /* X */, 89 /* Y */, 90 /* Z */, 219 /* [ */, 220 /* \ */, 221 /* ] */, 54 /* ^ */, 189 /* _ */,
        192 /* ` */, 65 /* a */, 66 /* b */, 67 /* c */, 68 /* d */, 69 /* e */, 70 /* f */, 71 /* g */,
        72 /* h */, 73 /* i */, 74 /* j */, 75 /* k */, 76 /* l */, 77 /* m */, 78 /* n */, 79 /* o */,
        80 /* p */, 81 /* q */, 82 /* r */, 83 /* s */, 84 /* t */, 85 /* u */, 86 /* v */, 87 /* w */,
        88 /* x */, 89 /* y */, 90 /* z */, 219 /* { */, 220 /* | */, 221 /* } */, 192 /* ~ */, 46 /* DEL */
    };
    static_assert(sizeof map /sizeof map[0] == 128, "invalid map");
    return (0 <= key && key <= 127) ? map[key] : 0;
}

void keyboard(unsigned char key, int x, int y)
{
    if (WindowImp* imp = static_cast<WindowImp*>(window.self()))
        imp->keydown(key, getCharKeyCode(key), glutGetModifiers());
}

void keyboardUp(unsigned char key, int x, int y)
{
    if (WindowImp* imp = static_cast<WindowImp*>(window.self()))
        imp->keyup(key, getCharKeyCode(key), glutGetModifiers());
}

unsigned getSpecialKeyCode(int key)
{
    switch (key) {
    case GLUT_KEY_F1:
        return 112;
    case GLUT_KEY_F2:
        return 113;
    case GLUT_KEY_F3:
        return 114;
    case GLUT_KEY_F4:
        return 115;
    case GLUT_KEY_F5:
        return 116;
    case GLUT_KEY_F6:
        return 117;
    case GLUT_KEY_F7:
        return 118;
    case GLUT_KEY_F8:
        return 119;
    case GLUT_KEY_F9:
        return 120;
    case GLUT_KEY_F10:
        return 121;
    case GLUT_KEY_F11:
        return 122;
    case GLUT_KEY_F12:
        return 123;
    case GLUT_KEY_LEFT:
        return 37;
    case GLUT_KEY_UP:
        return 38;
    case GLUT_KEY_RIGHT:
        return 39;
    case GLUT_KEY_DOWN:
        return 40;
    case GLUT_KEY_PAGE_UP:
        return 33;
    case GLUT_KEY_PAGE_DOWN:
        return 34;
    case GLUT_KEY_HOME:
        return 36;
    case GLUT_KEY_END:
        return 35;
    case GLUT_KEY_INSERT:
        return 45;
    case 109:  // Num Lock
        return 144;
    default:
        return 0;
    }
}

void special(int key, int x, int y)
{
    unsigned keycode = getSpecialKeyCode(key);
    if (keycode) {
        if (WindowImp* imp = static_cast<WindowImp*>(window.self()))
            imp->keydown(0, keycode, glutGetModifiers());
    }
}

void specialUp(int key, int x, int y)
{
    unsigned keycode = getSpecialKeyCode(key);
    if (keycode) {
        if (WindowImp* imp = static_cast<WindowImp*>(window.self()))
            imp->keyup(0, keycode, glutGetModifiers());
    }
}

void mouse(int button, int state, int x, int y)
{
    if (WindowImp* imp = static_cast<WindowImp*>(window.self()))
        imp->mouse(button, state, x, y, glutGetModifiers());
}

void mouseMove(int x, int y)
{
    if (WindowImp* imp = static_cast<WindowImp*>(window.self()))
        imp->mouseMove(x, y, glutGetModifiers());
}

void timer(int value)
{
    if (WindowImp* imp = static_cast<WindowImp*>(window.self())) {
        if (imp->poll())
            glutPostRedisplay();
    }
    glutTimerFunc(50, timer, 0);
    // TODO: do GC here or maybe in the idle proc
 }

void init(int* argc, char* argv[])
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(816, 1056);
    glutCreateWindow(argv[0]);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMove);
    glutPassiveMotionFunc(mouseMove);
    glutTimerFunc(50, timer, 0);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
}
