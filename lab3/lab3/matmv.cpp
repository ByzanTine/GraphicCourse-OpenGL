/*
 * Copyright (c) 2011 University of Michigan, Ann Arbor.
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
 * Author: Sugih Jamin
*/
#include "matmv.h"

/*
 * YOUR goal here is to set up the current transformation matrix
 * ((*this)) with the appropriate matrix to perform the transformation
 * you want.
*/
MatModView CTM;  // current transformation matrix
    
void MatModView::
translate(float tx, float ty, float tz)
{
  /* YOUR CODE HERE */
  Identity();
  XVec3f tmp = XVec3f(tx, ty, tz);
  setCol(3, tmp);
  return;
}
  
void MatModView::
scale(float sx, float sy, float sz)
{
  /* YOUR CODE HERE */
  Identity();

  operator()(0, 0) = sx;
  operator()(1, 1) = sy;
  operator()(2, 2) = sz;
  return;
}

void MatModView::
Xshearby(float hxy, float hxz)
{
  Identity();
  // XVec4f(sx, sy, sz, 1);
  operator()(0, 1) = hxy;
  operator()(0, 2) = hxz;

  return;
}

void MatModView::
Yshearby(float hyx, float hyz)
{
  /* YOUR CODE HERE */
  Identity();

  operator()(1, 0) = hyx;
  operator()(1, 2) = hyz;
  return;
}

void MatModView::
Zshearby(float hzx, float hzy)
{
  /* YOUR CODE HERE */
  Identity();

  operator()(2, 0) = hzx;
  operator()(2, 1) = hzy;
  return;
}

void MatModView::
rotateX(float theta)
{
  /* YOUR CODE HERE */
  Identity();
  operator()(1,1) = cosf(theta);
  operator()(1,2) = -sinf(theta);
    
  operator()(2,1) = sinf(theta);
  operator()(2,2) = cosf(theta);
  return;
}

void MatModView::
rotateY(float theta)
{
  /* YOUR CODE HERE */
  Identity();
  operator()(0,0) = cosf(theta);
  operator()(0,2) = sinf(theta);
  operator()(2,0) = -sinf(theta);
  operator()(2,2) = cosf(theta);
  return;
}

void MatModView::
rotateZ(float theta)
{
  /* YOUR CODE HERE */
  Identity();
  operator()(0,0) = cosf(theta);
  operator()(0,1) = -sinf(theta);
  operator()(1,0) = sinf(theta);
  operator()(1,1) = cosf(theta);
  return;
}
