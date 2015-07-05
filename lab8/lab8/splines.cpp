/*
 * Copyright (c) 2007, 2011 University of Michigan, Ann Arbor.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of Michigan, Ann Arbor. The name of the University 
 * may not be used to endorse or promote products derived from this 
 * software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Authors: Manoj Rajagopalan, Sugih Jamin
 *
*/
#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "xvec.h"

/* flags to show various interpolants/approximants
   and the functions that implement them */
int show_linear = 1;
extern void draw_linear(void);

int show_catmull_rom = 1;
extern void draw_catmull_rom(void);

int show_quadratic_endpieces = 0;
extern void draw_quadratic_endpieces(void);

int show_bezier = 0;
extern void draw_cubic_bezier(void);

int wd;
GLfloat screen_w = 1280.0;
GLfloat screen_h = 800.0;

#define NUM_POINTS 8
XVec2f points[NUM_POINTS]; /* Points forming the curve */

/* Currently selected point */
int curr_point = -1;

void
draw_points(void)
{
  int i;
        
  glColor4f(1.0, 0.0, 1.0, 1.0);

  /* Highlight points as filled squares */
  glBegin(GL_QUADS);
  for (i=0; i<NUM_POINTS; ++i) {
    glVertex2f(points[i](0)-10, points[i](1)-10);
    glVertex2f(points[i](0)-10, points[i](1)+10);
    glVertex2f(points[i](0)+10, points[i](1)+10);
    glVertex2f(points[i](0)+10, points[i](1)-10);
  }
  glEnd();

  return;
}

void
display(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  draw_points();
  if (show_linear) {
    draw_linear();
  }
  if (show_catmull_rom) {
    draw_catmull_rom();
  }
  if (show_quadratic_endpieces) {
    draw_quadratic_endpieces();
  }
  if (show_bezier) {
    draw_cubic_bezier();
  }
  glutSwapBuffers();

  return;
}

int
find_closest_point(int x, int y)
{
  int i;
  for (i=0; i<NUM_POINTS; ++i) {
    if (abs(points[i](0)-x) < 10 && abs(points[i](1)-y) < 10) {
      return i;
    }
  }

  return(-1);
}

void
passive_motion(int x, int y)
{
  static int cur_cursor = GLUT_CURSOR_LEFT_ARROW;

  /* curr_point contains the point that the mouse is closest to */
  /* -1 if not close to any point */
  curr_point = find_closest_point(x,screen_h-y);

  if (curr_point < 0 && cur_cursor == GLUT_CURSOR_CROSSHAIR) {

    glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
    cur_cursor = GLUT_CURSOR_LEFT_ARROW;

  } else if (curr_point >=0 && cur_cursor == GLUT_CURSOR_LEFT_ARROW) {

    glutSetCursor(GLUT_CURSOR_CROSSHAIR);
    cur_cursor = GLUT_CURSOR_CROSSHAIR;

  }

  return;
}

int mouse_prev_x;
int mouse_prev_y;

/* Mouse click callback */
void
mouse(int button, int state, int x, int y)
{
  /* left-click */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && curr_point>=0) {
    mouse_prev_x = x;
    mouse_prev_y = y;
  }

  return;
}

/* Mouse drag callback */
void
motion(int x, int y)
{
  if (curr_point >= 0) {
    points[curr_point](0) += (x - mouse_prev_x);
    points[curr_point](1) += (mouse_prev_y - y);
    mouse_prev_x = x;
    mouse_prev_y = y;
  }
  glutPostRedisplay();

  return;
}

void
reshape(int w, int h)
{
  screen_w = (GLfloat) w;
  screen_h = (GLfloat) h;
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, screen_w, 0, screen_h, -1.f, 1.f);

  return;
}

void
init(void)
{
  int i;

  GLfloat y = screen_h / 2;
  GLfloat x = 0.1 * screen_w;
  GLfloat dx = 0.8*screen_w / (GLfloat)(NUM_POINTS-1);
  points[0](0) = x;
  points[0](1) = y;
  for (i=1; i<NUM_POINTS; ++i) {
    x += dx;
    points[i](0) = x;
    points[i](1) = y;
  }

  return;
}

void
kbd(unsigned char key, int x, int y)
{
  switch ((char)key) {
  case '1':
    show_linear = !show_linear;
    if (show_linear) {
      printf("Displaying linear interpolant\n");
    } else {
      printf("Hiding linear interpolant\n");
    }
    break;

  case '2':
    show_catmull_rom = !show_catmull_rom;
    if (show_catmull_rom) {
      printf("Displaying Catmull-Rom interpolant\n");
    } else {
      printf("Hiding Catmull-Rom interpolant\n");
    }
    break;

  case '3':
    show_quadratic_endpieces = !show_quadratic_endpieces;
    if (show_quadratic_endpieces) {
      printf("Displaying quadratic endpieces\n");
    } else {
      printf("Hiding quadratic endpieces\n");
    }
    break;

  case '4':
    show_bezier = 1 - show_bezier;
    if (show_bezier) {
      printf("Displaying Bezier interpolant\n");
    } else {
      printf("Hiding Bezier interpolant\n");
    }
    break;

  case 'q':
  case 27:
    glutDestroyWindow(wd);
    exit(0);
    break;
  }

  glutPostRedisplay();

  return;
}

int
main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(screen_w, screen_h);
  wd = glutCreateWindow("Experiment with splines");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutPassiveMotionFunc(passive_motion);
  glutKeyboardFunc(kbd);
  //  glutIdleFunc(idle);

  init();

  glClearColor(1.0, 1.0, 1.0, 0.0);
        
  glutMainLoop();
  
  exit(0);
}
