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
#include <OpenGL/glu.h>
#else
#ifdef _WIN32
#include "windows.h"
#endif
#include <GL/glu.h>
#endif

#include <stdio.h>

#include "xvec.h"
#include "xmat.h"

#include "viewing.h"

XMat4f CTM;  // current transformation matrix

const float DELTA = 10.0;
const float THETA = (float)(4.0*M_PI/180.0);
const float THETA_PER_PIXEL = (float)(0.5*M_PI/180.0);

Camera cam;

Camera::
Camera(XVec4f &eye_pos, XVec4f &gaze_dir, XVec4f &top_dir,
       float near_plane, float far_plane, float deg)
{
  e = eye_pos;
  g = gaze_dir;
  t = top_dir;

  zNear = near_plane;
  zFar = far_plane;
  fovy = deg;

  orient();

  return;
}

void Camera::
orient()
{
  /* YOUR CODE HERE
   * Compute the new basis vectors u, v, w based on
   * gaze direction (g) and top direction (t) already
   * stored in the Camera's class variables.
   * Store the results in the Camera's u, v, w variables.
  */
  w = -g;
  v = t;
  u = v.cross(w);

  return;
}

void Camera::
dolly(float delta)
{
  /* YOUR CODE HERE
   * Advance the eye position along
   * gaze direction by delta factor.
   * Work on this only after you've gotten
   * perspective projection up and running.
  */
  e(2) += delta;
  return;
}

void Camera::
crab(float delta)
{
  /* YOUR CODE HERE
   * Track the eye POSITION left and right
   * by delta factor, no panning.
  */
  e(0) += delta;
  return;
}

void Camera::
boom(float delta)
{
  /* YOUR CODE HERE
   * Track the eye POSITION up and down
   * by delta factor, no tilting.
  */
  e(1) += delta;
  return;
}

void Camera::
pan(float theta)
{
  /* YOUR CODE HERE
   * Rotate gaze direction by theta
   * about the camera's v axis
  */  
  g = cosf(theta) * g + (1 - cosf(theta)) * (g * v) * v + sinf(theta) * (v.cross(g));
  // std::cerr << "g: " << g << std::endl;
  return;
}
  
void Camera::
tilt(float theta)
{
  /* YOUR CODE HERE
   * Rotate gaze direction AND top direction
   * by theta about the camera's u axis
  */
  // here use Rodrigues Formula
  g = cosf(theta) * g + (1 - cosf(theta)) * (g * u) * u + sinf(theta) * (u.cross(g));
  t = cosf(theta) * t + (1 - cosf(theta)) * (t * u) * u + sinf(theta) * (u.cross(t));
  // std::cerr << "g: " << g << std::endl;
  return;
}
  
void Camera::
roll(float theta)
{
  /* YOUR CODE HERE
   * Manipulate top direction so as to get the effect of roll
  */
  t = cosf(theta) * t + (1 - cosf(theta)) * (t * g) * g + sinf(theta) * (g.cross(t));
  return;
}

void
movecam(unsigned char key, int x, int y)
{
  switch (key) {
  case 'l': // crab right
  case 'X': // crab right
    /* YOUR CODE HERE */
    cam.crab(DELTA);
    break;
  case 'h': // crab left
  case 'x': // crab left
    /* YOUR CODE HERE */
    cam.crab(-DELTA);
    break;
  case 'k': // boom up
  case 'Y': // boom up
    /* YOUR CODE HERE */
    cam.boom(DELTA);
    break;
  case 'j': // boom down
  case 'y': // boom down
    /* YOUR CODE HERE */
    cam.boom(-DELTA);
    break;
  case 'w': // dolly forward
  case 'z': // dolly forward
    /* YOUR CODE HERE */
    cam.dolly(DELTA);
    break;
  case 's': // dolly backward
  case 'Z': // dolly backward
    /* YOUR CODE HERE */
    cam.dolly(-DELTA);
    break;
  case 'd': // roll cw
    /* YOUR CODE HERE */
    cam.roll(THETA);
    break;
  case 'a': // roll ccw
    /* YOUR CODE HERE */
    cam.roll(-THETA);
    break;
  case MOUSE_DRAG:
    if (x) { // pan left or right, gaze follows mouse
      /* YOUR CODE HERE */
      cam.pan( - x * THETA_PER_PIXEL);
    }
    if (y) { // tilt left or right, gaze follows mouse
      /* YOUR CODE HERE */
      cam.tilt( - y * THETA_PER_PIXEL);
    }
    break;
  default:
    break;
  }

  /* re-orient camera's coordinate system */
  cam.orient();
  
  return;
}

/*
 * This function converts the world space representation of the cube-array and the pyramid
 * to camera space using viewing transformations. 
 *
 * Set up your viewing transformations in the current transformation matrix (CTM)
 *
 * At the end, after the application of perspective projection, only the first 
 * two coordinates will be significant since you have projected onto a 2D window.
 *
 * Pre:
 * - Camera coordinate system (u, v, w) already correctly set up
 *   (by calling Camera::orient())
*/
void
setup_view(transform_t mode)
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  CTM.Identity();

	if (mode == HARDWARE) {
    /* YOUR CODE HERE 
     * Use gluLookAt() to set up the eye coordinate system 
     * with the set up stored in the Camera "cam"
    */
    gluLookAt(cam.e(0), cam.e(1), cam.e(2), 
              cam.e(0) + cam.g(0), cam.e(1) + cam.g(1), cam.e(2) + cam.g(2),
              cam.t(0), cam.t(1), cam.t(2));
  } else {
    /* YOUR CODE HERE
     * 1. Apply a translation to move world origin to eye position
     *    - declare your translation matrix and compute its elements
     * 2. Rotate all points from {x,y,z} basis to this new {u,v,w} basis (world to eye)
     *    - calculate the rotation transformation matrix
     * 3. Multiply the rotation matrix with the translation matrix to form your viewing
     *    transformation matrix to be stored in the CTM.
     */
    XVec4f translationVector = -cam.e;
    XMat4f translationMatrix; // init as identity
    translationMatrix.setCol(3, translationVector);
    translationMatrix(3,3) = 1; // reset the last entry for correct homogeneus coord
    XMat4f rotationMatrix;
    rotationMatrix.setRow(0, cam.u);
    rotationMatrix.setRow(1, cam.v);
    rotationMatrix.setRow(2, cam.w);

    CTM = rotationMatrix * translationMatrix;

  }

  return;
}
