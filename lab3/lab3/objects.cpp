/*
 * Copyright (c) 2008, 2011, 2012 University of Michigan, Ann Arbor.
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
#define _USE_MATH_DEFINES
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#ifdef _WIN32
#include "windows.h"
#endif
#include <GL/gl.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "xvec.h"
#include "objects.h"
#include "matmv.h"
#include "transforms.h"

object_t object;

Cube cube;          /* cube used for modeling transforms */
Cone cone;          /* cone used for modeling transforms */

Cube::
Cube(void)
{
  // Initialize the center and 8 vertices of the cube
  center = XVec4f(0.0, 0.0, 0.0, 1.0);
  vert[0] = XVec4f(50.0, 50.0, 50.0, 1.0);
  vert[1] = XVec4f(50.0, 50.0, -50.0, 1.0);
  vert[2] = XVec4f(50.0, -50.0, 50.0, 1.0);
  vert[3] = XVec4f(50.0, -50.0, -50.0, 1.0);
  vert[4] = XVec4f(-50.0, -50.0, 50.0, 1.0);
  vert[5] = XVec4f(-50.0, -50.0, -50.0, 1.0);
  vert[6] = XVec4f(-50.0, 50.0, 50.0, 1.0);
  vert[7] = XVec4f(-50.0, 50.0, -50.0, 1.0);

  return;
}
    
void Cube::
draw(void)
{
  glColor3f(1.0, 1.0, 1.0); // draw in white
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  // yellow lines form the back and bottom faces
  
  // One of two square surfaces
  glBegin(GL_LINE_LOOP);
    glVertex3f(vert[0][0], vert[0][1], vert[0][2]);
    glVertex3f(vert[1][0], vert[1][1], vert[1][2]);
    glColor3f(1.0, 1.0, 0.0); // draw in yellow
    glVertex3f(vert[3][0], vert[3][1], vert[3][2]);
    glVertex3f(vert[2][0], vert[2][1], vert[2][2]);
  glEnd();
    
  // The other square surface not connected to the first
  glColor3f(1.0, 1.0, 1.0); // draw in white
  glBegin(GL_LINE_LOOP);
    glVertex3f(vert[6][0], vert[6][1], vert[6][2]);
    glVertex3f(vert[7][0], vert[7][1], vert[7][2]);
    glColor3f(1.0, 1.0, 0.0); // draw in yellow
    glVertex3f(vert[5][0], vert[5][1], vert[5][2]);
    glVertex3f(vert[4][0], vert[4][1], vert[4][2]);
  glEnd();
    
  // Connection between corresponding points on the two squares
  glColor3f(1.0, 1.0, 1.0); // draw in white
  glBegin(GL_LINES);
    glVertex3f(vert[0][0], vert[0][1], vert[0][2]);
    glVertex3f(vert[6][0], vert[6][1], vert[6][2]);
    glColor3f(1.0, 1.0, 0.0); // draw in yellow
    glVertex3f(vert[1][0], vert[1][1], vert[1][2]);
    glVertex3f(vert[7][0], vert[7][1], vert[7][2]);
    glColor3f(1.0, 1.0, 1.0); // draw in white
    glVertex3f(vert[2][0], vert[2][1], vert[2][2]);
    glVertex3f(vert[4][0], vert[4][1], vert[4][2]);
    glColor3f(1.0, 1.0, 0.0); // draw in yellow
    glVertex3f(vert[3][0], vert[3][1], vert[3][2]);
    glVertex3f(vert[5][0], vert[5][1], vert[5][2]);
  glEnd();
    
  return;
}

void Cube::
transform()
{
  /* YOUR CODE HERE 
   * Apply the transformation(s) set up
   * in the current transformation matrix
   * to the cube.
  */
  for (int i = 0; i < 8; ++i)
  {
    vert[i] = CTM * vert[i];
  }
  return;
}

Cone::
Cone(void)
{
  int i;
  float radperslice = 2.0*M_PI/SLICES;
  
  radius = 50.0;
  height = 100.0;
  center = XVec4f(0.0, 0.0, 0.0, 1.0);

  /* YOUR CODE HERE
   * Setup the vertex array in vert[] such that vert[0] is the base
   * center, vert[1] is the pole, and the rest of vert[] contains the
   * vertices approximating the base circle.  Setup base[] to contain
   * the indices of vert[] forming a triangle fan for the base, and
   * cone[] those of a triangle fan forming the cone.
  */

  return;
}

void Cone::
draw(void)
{
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glColor3f(0.8, 0.6, 0.0);

  /* YOUR CODE HERE
   * Use vert[] as a vertex array.  Draw the base as a triangle fan
   * with its vertices listed in base[].  Draw the cone itself as a
   * triangle fan with vertices listed in cone[].
  */
  
  return;
}

void Cone::
transform(void)
{
  /* YOUR CODE HERE 
   * Apply the transformation(s) set up
   * in the current transformation matrix
   * to the cone.
  */

  return;
}

/* draw world coordinate system's axis */
void
drawAxes(int w, int h)
{
  int islit;
  
  islit = glIsEnabled(GL_LIGHTING);
  if (islit) {
    glDisable(GL_LIGHTING);
  }

  glLineWidth(2.0);
  glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(-w/2.0, 0.0, 0.0);
    glVertex3f(w/2.0, 0.0, 0.0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, w/2.0, 0.0);
    glVertex3f(0.0, -w/2.0, 0.0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, w);
    glVertex3f(0.0, 0.0, -w);
  glEnd();

  if (islit) {
    glEnable(GL_LIGHTING);
  }

  return;
}

void
drawWorld(transform_t mode, int w, int h)
{
  glLineWidth(1.0);
  
  drawAxes(w, h);

  switch (object) {
  case CUBE:
    cube.draw();
    break;
  case CONE:
    cone.draw();
    break;
  default:
    printf("Unknown object\n");
    exit(1);
    break;
  }

  return;
}
    
void
resetWorld(transform_t mode)
{
  CTM.Identity();
  cube = Cube();
  cone = Cone();

  return;
}

void
setupWorld(transform_t mode)
{
  /* YOUR CODE HERE
     Do any initializations you need to do
     before setting the transformation matrix.
  */


  return;
}
  
void
transformWorld(transform_t mode)
{
  /* YOUR CODE HERE
   * Now that you've set up the transformation matrix,
   * do the modeling transform by calling the transform()
   * method of the objects.
   */
  if (object == CUBE)
    cube.transform();
  return;
}
