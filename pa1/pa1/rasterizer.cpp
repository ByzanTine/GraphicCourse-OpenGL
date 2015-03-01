/*
 * Copyright (c) 2010, 2011, 2012 University of Michigan, Ann Arbor.
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
 * Authors:
 *        Ari Grant, grantaa@umich.edu
 *        Sugih Jamin, jamin@eecs.umich.edu
 */

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <stdio.h>
using namespace std;

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#else
#include <GL/glut.h>
#endif

#include "rasterizer.h"

void drawPoint(XVec2f &point, XVec4f &pointColor, XVec4f &clipView);

void 
drawPoint(XVec2f &point, XVec4f &pointColor, XVec4f &clipView, bool swapped, bool mirrored, int centerX) {
  int x = point(0);
  int y = point(1);
  fprintf(stderr, "Draw Point Raw coordinates: %d, %d\n", x, y);
  if (swapped)
    swap(x, y);
  if (mirrored) {
    x = (centerX << 1) - x;
  }
  XVec2f drawPlace(x, y);
  drawPoint(drawPlace, pointColor, clipView);
  fprintf(stderr, "Draw Point: %d, %d\n", x, y);
  return;
}


Line::
Line()
{
  vertex0 = XVec2f(-1,-1);
  vertex1 = XVec2f(-1,-1);
  
  color0 = XVec4f(1,0,0,1);
  color1 = XVec4f(0,1,0,1);
  
  isAntialiased = false;

  return;
}

// ========================
// helper functions  
// use the information of vertex
float  Line::
fmid(int vertex[2][2], float x, float y) {

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

/*
 * Line::drawInRect(XVec4f &clipWin)
 * Pre: assumes all class members have been initialized with valid values.
 *      clipWin contains the clipping window
 */
void Line::
drawInRect(XVec4f &clipWin)
{
  /* This function draws the line from vertex0 to vertex1 with endpoints
     colored color0 and color1 respectively. It should call drawPoint() 
     to set each pixel. For the third argument of drawPoint(), pass on 
     the variable clipWin. */
  int vertex[2][2];
  vertex[0][0] = vertex0.x();
  vertex[0][1] = vertex0.y();
  vertex[1][0] = vertex1.x();
  vertex[1][1] = vertex1.y();

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
  float l_fmid = fmid(vertex, x0 + 1, y0 + 0.5); // TODO
  for (int x = x0; x <= x1; ++x)
  { 
    // interpolate color
    XVec2f drawPlace(x, y);

    if (exchanged) {
      
      XVec4f drawColor = color0 * (float)(x - x0)/(float)(x1 - x0) + color1 * (float)(x1 - x)/(float)(x1 - x0);
      drawPoint(drawPlace, drawColor, clipWin, swapped, mirrored, center[0]);
    }
    else {
      XVec4f drawColor = color0 * (float)(x1 - x)/(float)(x1 - x0) + color1 * (float)(x - x0)/(float)(x1 - x0);
      drawPoint(drawPlace, drawColor, clipWin, swapped, mirrored, center[0]);
    }
    fprintf(stderr, "fid: %f\n", l_fmid);
    
    if (l_fmid < 0) {
     y++;
     l_fmid += dx - dy;
    }
    else {
     l_fmid -= dy;
    }
   } 
  /* The color should be interpolated across the line, it should be
     one pixel thick, and clipped against the given rect. */
  
  /* YOUR CODE HERE */

  return;
}

Triangle::
Triangle()
{
  Line();
  
  vertex2 = XVec2f(-1,-1);
  color2 = XVec4f(0,0,1,1);

  return;
}


bool Triangle::
containsPoint(XVec2f &point, XVec4f &pointColor)
{
  /* This function returns whether the given point is inside the given
     triangle. If it is, then pointColor is set to  the color of the point
     inside the triangle by interpolating the colors of the vertices. */
  // already have all vertex information 
  int vertex[3][2]; // three points
  vertex[0][0] = vertex0.x();
  vertex[0][1] = vertex0.y();
  vertex[1][0] = vertex1.x();
  vertex[1][1] = vertex1.y();
  vertex[2][0] = vertex2.x();
  vertex[2][1] = vertex2.y();
  int x = point.x();
  int y = point.y();
  int vPQ[] = {vertex[1][0] - vertex[0][0], vertex[1][1] - vertex[0][1]};
  int vPR[] = {vertex[2][0] - vertex[0][0], vertex[2][1] - vertex[0][1]};
  
  int vPM[] = {x - vertex[0][0], y - vertex[0][1]};

  int sPQR = vPQ[0]*vPR[1] - vPQ[1]*vPR[0];
  
  int sPQM = vPQ[0]*vPM[1] - vPQ[1]*vPM[0];

  int sPRM =  - vPR[0]*vPM[1] + vPR[1]*vPM[0];
  
  float beta = sPQM*1.0/sPQR*1.0;
  float gamma = sPRM*1.0/sPQR*1.0;
  if (beta >= 0 && gamma >= 0 && beta + gamma <= 1) {
    pointColor = (1 - beta - gamma) * color0 +
                  beta * color2 + 
                  gamma * color1;
    return true;
  }
    
  else 
    return false;

  /* YOUR CODE HERE */
  /* set the point's color to blue for now
     pointColor = XVec4f(0.0,0.0,1.0,1.0);
  */
  
}

void Triangle::
drawInRect(XVec4f &clipWin)
{
  /* This function draws the triangle t. It should call drawPoint()
     to set each pixel. */
  
  /* YOUR CODE HERE */
  BoundingBox bbox;
  bbox.xmin = 0;
  bbox.ymin = 0;
  bbox.xmax = 500;
  bbox.ymax = 800;
  fprintf(stderr, "DRAW Triangle\n");
  for (int i = bbox.xmin; i < bbox.xmax; i++) {
    for (int j = bbox.ymin; j < bbox.ymax; j++) {
      // if ()
      XVec4f pointColor;
      XVec2f pointPlace(i,j);
      if (containsPoint(pointPlace, pointColor)) {
        drawPoint(pointPlace, pointColor, clipWin);
      }
    }
  }
  return;
}
