/*
 * Copyright (c) 2008, 2011, 2014 University of Michigan, Ann Arbor.
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
*/
#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <algorithm>
using std::swap;

/* Where was the mouse last clicked? */
#define NVERTICES 2
int vertex[NVERTICES][2]; /* Array of 2D points */

/* Flag to indicate the number of vertices that the user has set by clicking */
int vvalid = 0;

/* screen width and height - screen refers to
   window within which we draw*/
#define DELTA 25
GLdouble screen_width=51*DELTA, screen_height=32*DELTA;

/* Handle to the GLUT window */
int wd;

void
drawpoint(GLint x, GLint y, GLclampf r, GLclampf g, GLclampf b)
{
  glColor3f(r,g,b);
  glRecti(x*DELTA, y*DELTA, (x+1)*DELTA, (y+1)*DELTA);
  /* NOTE: you cannot call glRect within a glBegin()...glEnd() block */

  return;
}


void 
drawpoint(GLint x, GLint y, GLclampf r, GLclampf g, GLclampf b, bool swapped, bool mirrored, int centerX) {
  fprintf(stderr, "Draw Point Raw coordinates: %d, %d\n", x, y);
  if (swapped)
    swap(x, y);
  if (mirrored) {
    x = (centerX << 1) - x;
  }
  
  drawpoint(x, y, r, g, b);
  fprintf(stderr, "Draw Point: %d, %d\n", x, y);
  return;
}

// ========================
// helper functions  
// use the information of vertex
float 
fmid(float x, float y) {

  int x0 = vertex[0][0];
  int x1 = vertex[1][0];
  int y0 = vertex[0][1];
  int y1 = vertex[1][1];

  int A = y0 - y1;
  int B = x1 - x0 ;
  int C = x0 * y1 - x1 * y0;

  return A * x + B * y + C;
}
//=========================

/* Display callback */
void
display(void)
{
  int i;
  
  /* Set background to white */
  glClear(GL_COLOR_BUFFER_BIT);

  /* Draw a grey grid */
  glColor3f(0.5, 0.5, 0.5);
  glLineWidth(1.0);
  glBegin(GL_LINES);
  /* vertical lines */
  for(i=0; i<screen_width; i+=DELTA) {
    glVertex2i(i,0);
    glVertex2i(i,(int)screen_height);
  }
  /* horizontal lines */
  for (i=0; i<screen_height; i+=DELTA) {
    glVertex2i(0,i);
    glVertex2i((int)screen_width, i);
  }
  glEnd();

  /* Draw the first point (square) in red, if value) */
  if (vertex[0][0]>=0 && vertex[0][1]>=0) {
    drawpoint(vertex[0][0], vertex[0][1], 1.0, 0.0, 0.0);
  }
  
  /* Draw the second point (square) in blue, if value) */
  if (vertex[1][0]>=0 && vertex[1][1]>=0) {
    drawpoint(vertex[1][0], vertex[1][1], 0.0, 0.0, 1.0);
  }
  
  /* Draw straight line connecting end points if both are valid*/
  if (vvalid == 2) {
    glLineWidth(3.0);
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glVertex2i(vertex[0][0]*DELTA+DELTA/2, vertex[0][1]*DELTA+DELTA/2);
    glVertex2i(vertex[1][0]*DELTA+DELTA/2, vertex[1][1]*DELTA+DELTA/2);
    glEnd();
    
    /* TODO - implement the line drawing algorithm here
       int vertex[0], vertex[1] store integer coordinates of the 
       'endpoint pixels' The support code does the task of trapping 
       mouse clicks and setting these
       NOTE: Interpolate the color between the endpoints to get a 
       gradual transition
    */

    fprintf(stderr, "vertex info: vertex0: %d, %d, vertex1: %d, %d\n", vertex[0][0], vertex[0][1], vertex[1][0], vertex[1][1]);
    // resolve the going toward smaller x
   
    int center[2];
    float m = (float)(vertex[1][1] - vertex[0][1]) / (float)(vertex[1][0] - vertex[0][0]);
    fprintf(stderr, "slope: %f\n", m);
    bool mirrored = false; // simply mirror according to x0
    if (m < 0) {
      fprintf(stderr, "mirror x\n");
      mirrored = true;
      vertex[1][0] = (vertex[0][0] << 1) - vertex[1][0];
      m = (float)(vertex[1][1] - vertex[0][1]) / (float)(vertex[1][0] - vertex[0][0]);
      // record mirror center
      center[0] = vertex[0][0];
      center[1] = vertex[0][1];

    }
    
    // no change on slope
    bool exchanged = false;
    if (vertex[0][0] > vertex[1][0]) {
      exchanged = true;
      swap(vertex[0], vertex[1]);
    }
    // swap x y
    bool swapped = false;
    if (m > 1) {
      fprintf(stderr, "swap x, y marked\n");
      swapped = true;
      swap(vertex[0][0], vertex[0][1]);
      swap(vertex[1][0], vertex[1][1]);
    }
    fprintf(stderr, "After preprocessing vertex info: vertex0: %d, %d, vertex1: %d, %d\n", vertex[0][0], vertex[0][1], vertex[1][0], vertex[1][1]);
    
    fprintf(stderr, "final slope: %f\n", (float)(vertex[1][1] - vertex[0][1]) / (float)(vertex[1][0] - vertex[0][0]));

    int x0 = vertex[0][0];
    int x1 = vertex[1][0];
    int y0 = vertex[0][1];  
    // int y1 = vertex[1][1]; 
    int y = y0;

    int dx = vertex[1][0] - vertex[0][0];
    int dy = vertex[1][1] - vertex[0][1];

    float l_fmid = fmid(x0 + 1, y0 + 0.5); // TODO
    for (int x = x0; x <= x1; ++x)
    { 
      // interpolate color
      if (exchanged) {
        float lerpR = 1.0 * (float)(x - x0)/(float)(x1 - x0);
        float lerpB = 1.0 * (float)(x1 - x)/(float)(x1 - x0);
        drawpoint(x, y, lerpR, 0.0, lerpB, swapped, mirrored, center[0]);
      }
      else {
        float lerpR = 1.0 * (float)(x1 - x)/(float)(x1 - x0);
        float lerpB = 1.0 * (float)(x - x0)/(float)(x1 - x0);
        drawpoint(x, y, lerpR, 0.0, lerpB, swapped, mirrored, center[0]);
      }

      
      
      if (l_fmid < 0) {
       y++;
       l_fmid += dx - dy;
      }
      else {
       l_fmid -= dy;
      }
     } 
       
  }
  
  /* Force OpenGL to start drawing */
  // glFlush();
  glutSwapBuffers();

  return;
}

/* Mouse click event callback */
void
mouse(int button, int state, int x, int y)
{
  int i;

  /* Trap left-clicks */
  if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    if(vvalid==0) {
      vertex[0][0] = x/DELTA;
      vertex[0][1] = (((int) screen_height)-y)/DELTA;
      ++vvalid;
    } else if(vvalid==1) {
      vertex[1][0] = x/DELTA;
      vertex[1][1] = (((int) screen_height)-y)/DELTA;
      ++vvalid;
    } else {
      for (i = 0; i < NVERTICES; i++) {
        vertex[i][0] = -1;
        vertex[i][1] = -1;
      }
      vvalid = 0;
      vvalid = 0;
    }
    glutPostRedisplay();
  }

  return;
}

/* Keyboard callback */
void
kbd(unsigned char key, int x, int y)
{
  int i;

  switch((char)key) {
  case 'q':
    glutDestroyWindow(wd);
    exit(0);
    break;
                        
  case ' ':
  case 27:
    for (i = 0; i < NVERTICES; i++) {
      vertex[i][0] = -1;
      vertex[i][1] = -1;
    }
    vvalid = 0;
    glutPostRedisplay();
    break;
                        
  }

  return;
}

/* OpenGL resize callback */
void
reshape(int w, int h)
{
  /* Save new screen dimensions */
  screen_width = (GLfloat) w;
  screen_height = (GLfloat) h;
  
  /* Instruct Open GL to use the whole window for drawing */
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  
  /* No projection transformation */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  /* Set coordinate system to first quadrant and bound x and y */
  glOrtho(0.0, screen_width, 0.0, screen_height, -1.f, 1.f);

  return;
}

int
main(int argc, char *argv[])
{
  int i;

  /* Initialize GLUT, let it extract command-line GLUT options that
     you may provide 
     - NOTE THE & BEFORE argc
  */
  glutInit(&argc, argv);
  
  /* Want single window and color will be specified as RGBA values */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

  /* Set initial window size
     screen_width and screen_height have been above
  */
  glutInitWindowSize((int)screen_width, (int)screen_height);

  /* Create the window and store the handle to it */
  wd = glutCreateWindow("Experiment with line drawing" /* title */ );

  /* --- register callbacks for GUI events --- */
  /* NOTE prototypes for each, declared above */
  
  /* Register function that paints the window
     - Calling OpenGL functions or otherwise
  */
  glutDisplayFunc(display);
  
  /* Register function to handle window reshapes */
  glutReshapeFunc(reshape);
  
  /* Register keyboard event processing function */
  glutKeyboardFunc(kbd);
  
  /* Register mouse-click event callback */
  glutMouseFunc(mouse);
  
  /* -- init data -- */
  for (i = 0; i < NVERTICES; i++) {
    vertex[i][0] = -1;
    vertex[i][1] = -1;
  }
  vvalid = 0;

  /* -- init gl -- */
  glClearColor(1.0, 1.0, 1.0, 1.0);
  /* Fill the 'front' face of polygons - rectangles in our case */
  glPolygonMode(GL_FRONT, GL_FILL); 

  /* Start the GLUT event loop */
  glutMainLoop();

  exit(0);
}
