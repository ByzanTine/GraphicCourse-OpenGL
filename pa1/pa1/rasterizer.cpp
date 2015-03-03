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
#include <algorithm>
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
  // fprintf(stderr, "Draw Point Raw coordinates: %d, %d\n", x, y);
  if (swapped)
    swap(x, y);
  if (mirrored) {
    x = (centerX << 1) - x;
  }
  XVec2f drawPlace(x, y);
  drawPoint(drawPlace, pointColor, clipView);
  // fprintf(stderr, "Draw Point: %d, %d\n", x, y);
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

// return code 0 inside,
// return code 1 outside
// return code 2 need clipping
int Line::
Cohen_Sutherland_Preprocess(const XVec4f &clipWin) {
  // first few bytes 
  int point0 = 0;
  int point1 = 0;

  if (vertex0.x()  < clipWin(0))
    point0 += 1;
  else if (vertex0.x() > clipWin(0) + clipWin(2))
    point0 += (1 << 1);

  if (vertex0.y() < clipWin(1))
    point0 += (1 << 2);
  else if (vertex0.y() > clipWin(1) + clipWin(3))
    point0 += (1 << 3);

  // TODO duplicate anyway
  if (vertex1.x()  < clipWin(0))
    point1 += 1;
  else if (vertex1.x() > clipWin(0) + clipWin(2))
    point1 += (1 << 1);

  if (vertex1.y() < clipWin(1))
    point1 += (1 << 2);
  else if (vertex1.y() > clipWin(1) + clipWin(3))
    point1 += (1 << 3);
  fprintf(stderr, "clipWin: %f, %f, %f, %f\n", clipWin(0), clipWin(1), clipWin(2), clipWin(3));
  fprintf(stderr, "point0: %d, point1: %d\n", point0, point1);
  if ((point0 | point1) == 0)
    return 0;
  if ((point0 & point1) != 0)
    return 1;
  else 
    return 2;


}

// return code 1 outside
int Line::
Cyrus_Beck_Preprocess(const XVec4f &clipWin, int vertex[2][2], float& maxPE, float& minPL) {
  // store the clipped point in vertex
  XVec2f normals[4];
  normals[0] = XVec2f(0,1);
  normals[1] = XVec2f(0,-1);
  normals[2] = XVec2f(-1,0);
  normals[3] = XVec2f(1,0);

  // determine PE PL
  XVec2f u(vertex[1][0] - vertex[0][0], vertex[1][1] - vertex[0][1]); // line vector
  XVec2f point0(vertex[0][0] + 0.5, vertex[0][1] + 0.5);
  //XVec2f point1(vertex[1][0], vertex[1][1]);
  XVec2f clipWinLowerLeft(clipWin(0), clipWin(1));
  XVec2f clipWinUpperRight = clipWinLowerLeft + XVec2f(clipWin(2), clipWin(3));

  float PE[2] = {0.0f, 0.0f};
  float PL[2] = {1.0f, 1.0f};
  // if (u.x() == 0 || u.y() == 0)
  //   return 0;
  // if (u.x() > 0) {
  //   PE[0] = -normals[2].dot(point0 - clipWinLowerLeft) / normals[2].dot(u);
  //   PL[0] = -normals[3].dot(point0 - clipWinUpperRight) / normals[3].dot(u);
  // }

  // if (u.x() < 0) {
  //   PL[0] = -normals[2].dot(point0 - clipWinLowerLeft) / normals[2].dot(u);
  //   PE[0] = -normals[3].dot(point0 - clipWinUpperRight) / normals[3].dot(u);
  // }

  // if (u.y() > 0) {
  //   PL[1] = -normals[0].dot(point0 - clipWinUpperRight) / normals[0].dot(u);
  //   PE[1] = -normals[1].dot(point0 - clipWinLowerLeft) / normals[1].dot(u);
  // }

  // if (u.y() < 0) {
  //   PE[1] = -normals[0].dot(point0 - clipWinUpperRight) / normals[0].dot(u);
  //   PL[1] = -normals[1].dot(point0 - clipWinLowerLeft) / normals[1].dot(u);
  // }
  for (int i = 0; i < 4; ++i)
  {
    float dotProduct = u.dot(normals[i]);
    if (dotProduct == 0) 
      continue;
    float ratio;

    if(i == 0 || i == 3)
      ratio = normals[i].dot(clipWinUpperRight - point0)/dotProduct;
    else
      ratio = normals[i].dot(clipWinLowerLeft - point0)/dotProduct;
      
    if(dotProduct < 0)
      PE[i/2] = ratio;
    else
      PL[i/2] = ratio;
  }

  maxPE = max(PE[0], PE[1]);
  minPL = min(PL[0], PL[1]);
  if (minPL < maxPE) {
    return 1;
  }

  // record onto points
  maxPE = max(0.0f, maxPE);
  minPL = min(1.0f, minPL);
  fprintf(stderr, "maxPE: %f, minP:: %f\n", maxPE, minPL);
  // XVec2f newVertex0 = vertex0 + (vertex1 - vertex0) * maxPE;
  // XVec2f newVertex1 = vertex0 + (vertex1 - vertex0) * minPL;
  // vertex[0][0] = newVertex0.x();
  // vertex[0][1] = newVertex0.y();
  // vertex[1][0] = newVertex1.x();
  // vertex[1][1] = newVertex1.y();

  return 0;
}
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
  // do the preprocessing first
  int clipped = Cohen_Sutherland_Preprocess(clipWin);

  if (clipped == 1) 
    return;

  int vertex[2][2];
  vertex[0][0] = vertex0.x();
  vertex[0][1] = vertex0.y();
  vertex[1][0] = vertex1.x();
  vertex[1][1] = vertex1.y();
  fprintf(stderr, "vertex info: vertex0: %d, %d, vertex1: %d, %d\n", vertex[0][0], vertex[0][1], vertex[1][0], vertex[1][1]);
  
  
  float maxPE = 0.0f, minPL = 1.0f;
  if (clipped == 2) {
    if(Cyrus_Beck_Preprocess(clipWin, vertex, maxPE, minPL)) {
      return;
    }

  }
  fprintf(stderr, "vertex info After Cyrus_Beck_Preprocess: vertex0: %d, %d, vertex1: %d, %d\n", vertex[0][0], vertex[0][1], vertex[1][0], vertex[1][1]);

   
  bool swapped = false;
  int center[2];
  if (vertex[1][0] - vertex[0][0] == 0) {
      // m is infinity
      fprintf(stderr, "swap x, y marked\n");
      swapped = true;
      swap(vertex[0][0], vertex[0][1]);
      swap(vertex[1][0], vertex[1][1]);

  }

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
    fprintf(stderr, "exchanged!\n");
    exchanged = true;
    swap(vertex[0], vertex[1]);
    // do something with PE PL as well
    float tmp = minPL;
    minPL = 1.0f - maxPE;
    maxPE = 1.0f -tmp;
  }
  // swap x y
  
  if (m > 1) {
    fprintf(stderr, "swap x, y marked\n");
    swapped = true;
    swap(vertex[0][0], vertex[0][1]);
    swap(vertex[1][0], vertex[1][1]);
  }
  fprintf(stderr, "After preprocessing vertex info: vertex0: %d, %d, vertex1: %d, %d\n", vertex[0][0], vertex[0][1], vertex[1][0], vertex[1][1]);
  
  fprintf(stderr, "final slope: %f\n", (float)(vertex[1][1] - vertex[0][1]) / (float)(vertex[1][0] - vertex[0][0]));

  int clippedEndPoints[2][2];

  clippedEndPoints[0][0] = (vertex[0][0] + (vertex[1][0] - vertex[0][0]) * maxPE);
  clippedEndPoints[0][1] = (vertex[0][1] + (vertex[1][1] - vertex[0][1]) * maxPE);
  clippedEndPoints[1][0] = (vertex[0][0] + (vertex[1][0] - vertex[0][0]) * minPL);
  clippedEndPoints[1][1] = (vertex[0][1] + (vertex[1][1] - vertex[0][1]) * minPL);

  int x0 = clippedEndPoints[0][0];
  int x1 = clippedEndPoints[1][0];
  int y0 = clippedEndPoints[0][1];  
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
      
      XVec4f drawColor = color0 * (float)(x - vertex[0][0])/(float) dx + 
                         color1 * (float)(vertex[1][0] - x)/(float) dx;
      drawPoint(drawPlace, drawColor, clipWin, swapped, mirrored, center[0]);
    }
    else {
      XVec4f drawColor = color0 * (float)(vertex[1][0]  - x)/(float)dx + color1 * (float)(x - vertex[0][0] )/(float)dx;
      drawPoint(drawPlace, drawColor, clipWin, swapped, mirrored, center[0]);
    }
    // fprintf(stderr, "fid: %f\n", l_fmid);
    
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
  // fprintf(stderr, "DRAW Triangle\n");
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
