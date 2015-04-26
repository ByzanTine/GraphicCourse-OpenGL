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
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "xvec.h"
#include "objects.h"
#include "viewing.h"
#include "transforms.h"

object_t object;
float sphere_radius;
enum { SPOB=0, SIDX, NOBS }; // NOBS: number of object buffers
unsigned int bods[NOBS];     // vertex buffer object handles
int countperstack;

/* draw world coordinate system's axis */
void
drawAxes(int w, int h, float alpha)
{
  glDisable(GL_LIGHTING);

  glLineWidth(2.0);
  glBegin(GL_LINES);
    glColor4f(1.0, 0.0, 0.0, alpha);
    glVertex3f(-w, 0.0, 0.0);
    glVertex3f(w, 0.0, 0.0);
    glColor4f(0.0, 1.0, 0.0, alpha);
    glVertex3f(0.0, -h, 0.0);
    glVertex3f(0.0, h, 0.0);
    glColor4f(0.0, 0.0, 1.0, alpha);
    glVertex3f(0.0, 0.0, -w);
    glVertex3f(0.0, 0.0, w);
  glEnd();

  glEnable(GL_LIGHTING);

  return;
}

#define SLICES 1000  // longitude slices
#define STACKS 1000  // latitude slices
#include <stdio.h>
using namespace std;
GLuint vbods[2];                 // variable to hold vbo descriptor(s)      

XVec3f vert[STACKS + 1][SLICES];
XVec3f normals[STACKS + 1][SLICES];
unsigned int ids[STACKS  * (2 * SLICES + 2)];
/*
 * Initialize the vertex buffer objects
 * containing the vertices and vertex attributes
 * of objects in the world.
 * For sphere, use the macros SLICES and STACKS
 * to define the sphere.
*/
void
initWorld()
{
  /*
   * YOUR CODE HERE
   * Compute the vertices of your sphere and store them
   * in a vertex array and store the normals in a corresponding normal
   * array.  Then compute the indices to access the vertex array
   * to draw the sphere using glDrawElement().  Don't forget to set
   * the normal of your vertices.  Make sure the normals are of unit
   * length, i.e., normalized. You can either do manually or by
   * enabling an OpenGL mode (find it!).
   */
  int ids_index = 0;
  for (unsigned int stackNumber = 0; stackNumber < STACKS; ++stackNumber)
  {
    float theta = stackNumber * M_PI / STACKS;
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);
    for (int sliceNumber = SLICES - 1; sliceNumber >= 0; --sliceNumber)
    {
        
        float phi = sliceNumber * 2 * M_PI / SLICES;
        float sinPhi = sin(phi);
        float cosPhi = cos(phi);
        vert[stackNumber][sliceNumber] = XVec3f(sinPhi * sinTheta, cosPhi * sinTheta, cosTheta);
        normals[stackNumber][sliceNumber] = XVec3f(sinPhi * sinTheta, cosPhi * sinTheta, cosTheta);
        // normals[stackNumber][sliceNumber] = XVec3f(cosPhi * sinTheta, sinPhi * sinTheta, cosTheta);
        ids[ids_index++] = stackNumber * (SLICES) + (sliceNumber % SLICES);
        ids[ids_index++] = (stackNumber + 1) * (SLICES) + (sliceNumber % SLICES);

    }
    // ids[ids_index++] = stackNumber * SLICES;

  }
  float theta = M_PI;
  float cosTheta = cos(theta);
  float sinTheta = sin(theta);
  for (unsigned int sliceNumber = 0; sliceNumber < SLICES; ++sliceNumber)
  {
      
      float phi = sliceNumber * 2 * M_PI / SLICES;
      float sinPhi = sin(phi);
      float cosPhi = cos(phi);
      vert[STACKS][sliceNumber] = XVec3f(cosPhi * sinTheta, sinPhi * sinTheta, cosTheta);
      normals[STACKS][sliceNumber] = XVec3f(sinPhi * sinTheta, cosPhi * sinTheta, cosTheta);
  }

  return;
}
  
/*
 * Draw a sphere of the given radius.
*/
void
drawSphere(double radius)
{
  /* YOUR CODE HERE: 
   * Draw your sphere using glDrawElements() and GL_TRIANGLE_STRIP
   *
   * Caveat: the last argument to glDrawElement()
   * is offset IN BYTES from the start of index[]
   * array, i.e., give the offset in terms of
   * element count * sizeof(unsigned int), not just in
   * terms of element count, or your GPU will lock
   * up and you'd have to reboot your machine.
  */
  static float old_radius = 1.0f;
  for (unsigned int stackNumber = 0; stackNumber <= STACKS; ++stackNumber)
  {

    for (unsigned int sliceNumber = 0; sliceNumber <= SLICES; ++sliceNumber)
    {
        vert[stackNumber][sliceNumber] *= radius/old_radius;

    }
  }
  
  old_radius = radius;
  
  glEnableClientState(GL_VERTEX_ARRAY);
    
  glVertexPointer(3, GL_FLOAT, 0, &vert[0]);
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, 0, &normals[0]);

  glDrawElements(GL_TRIANGLE_STRIP, sizeof(ids)/sizeof(unsigned int), GL_UNSIGNED_INT, &ids[0]);
  
  glEnd();

  glDisableClientState(GL_VERTEX_ARRAY);
  return;
}

void
drawWorld(int w, int h)
{
  static GLUquadric *q;
  
  drawAxes(1, 1, 0.4); // draw object axes

  if (!q) q = gluNewQuadric();
  gluQuadricOrientation(q, GLU_OUTSIDE);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(0.6, 0.0, 0.0);
  if (object == TORUS) {
    glutSolidTorus(0.2, 0.3, SLICES, STACKS);
  } else {
    /* YOUR CODE HERE
     * uncomment the gluSphere call to see the expected
     * look and behavior of your sphere.  Make sure
     * you comment out the call to gluSphere and uncomment
     * the call to drawSphere before you turn in your lab.
    */
    // gluSphere(q, sphere_radius, SLICES, STACKS);
    drawSphere(sphere_radius);
  }
  glTranslatef(-1.2, -0.5, 0.0);
  glRotatef(-90.0, 1.0, 0.0, 0.0);
  gluCylinder(q, 0.5, 0.0, 1.0, SLICES, STACKS);
  glPopMatrix();

  return;
}
