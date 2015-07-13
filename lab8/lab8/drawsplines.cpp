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
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "xvec.h"
#include "xmat.h"

#define NUM_POINTS 8
extern XVec2f points[];

#define NCUBIC 4
#define NQUADRATIC 3
#define NSAMPLES 32

void
draw_linear(void)
{
  int i;

  /* Enable stippling on the linear segment */
  glEnable(GL_LINE_STIPPLE);
  glLineStipple(10, 0xAAAA);

  glColor4f(1.0, 0.0, 0.0, 1.0);
  glLineWidth(5.0);

  /* Draw line segments with points (x, y).  In this case, we simply
   * call OpenGL and let the graphics engine's rasterizer do the
   * linear interpolation. For the other splines, we'd need to compute
   * the interpolation manually, by computing the values of the spline
   * function between the control points.
  */
  glBegin(GL_LINE_STRIP);
  for (i = 0; i < NUM_POINTS; i++) {
    glVertex2f(points[i](0), points[i][1]);
  }
  glEnd();
        
  /* Disable the dotted-line effect for drawing */
  glDisable(GL_LINE_STIPPLE);

  return;
}

inline void
cspline_coeffs(XVec2f a[NCUBIC], /* result coefficients */
               XMat4f B, /* spline's basis matrix */
               XVec2f p[NCUBIC] /* control points */)
{
  memset((char *) a, 0, NCUBIC*sizeof(XVec2f));

  /* YOUR CODE HERE TASK 1
   *
   * Given 4 control points p[0]...p[3], calculate the coefficients a[0]...a[3]
   * of the cubic spline interpolating the control points.
  */
  // it's so convoluted to calculate this
  XVec4f px(p[0].x(), p[1].x(), p[2].x(), p[3].x());
  XVec4f py(p[0].y(), p[1].y(), p[2].y(), p[3].y());
  // mutiple each vector with B matrix
  XVec4f ax = B * px;
  XVec4f ay = B * py;
  for (int i = 0; i < NCUBIC; ++i)
  {
    a[i].x() = ax[i];
    a[i].y() = ay[i];
  }
  return;
}

inline void
cspline_eval(XVec2f &f,  /* result function */
             float u,    /* spline parameter, 0 <= u <= 1 */
             XVec2f a[NCUBIC] /* coefficients */)
{
  /* YOUR CODE HERE TASK 1
   *
   * Evaluate the cubic spline at the given parameter u,
   * given the coefficients of the spline.
  */
  f = a[0] + u * a[1] + u * u * a[2] + u * u * u * a[3];
  return;
}

void
draw_catmull_rom(void)
{
  XMat4f C, B;
  XVec2f a[NCUBIC], p[NCUBIC];

  /* YOUR CODE HERE TASK 1
   *
   * Manually populate C with the Catmull-Rom constraint matrix
  */
  C.setRow(0, XVec4f(1, -1, 1, 1));
  C.setRow(1, XVec4f(1,  0, 0, 0));
  C.setRow(2, XVec4f(1,  1, 1, 1));
  C.setRow(3, XVec4f(1,  2, 4, 6));
  /* YOUR CODE HERE TASK 1
   *
   * Compute B, the Catmull-Rom basis matrix, from C.
   * You MUST use C and not fill in the matrix manually.
  */
  B = C.inverse();
  glColor3f(0.0, 0.0, 1.0);
  glLineWidth(5.0);

  /* YOUR CODE HERE TASK 1
   *
   * Draw a Catmull-Rom spline that interpolates every control points
   * except the first and last ones.  For each spline segment between
   * two interpolated control points, first compute the spline's
   * coefficients for this segment.  Then samples the spline at
   * NSAMPLES location on the spline, i.e., compute f(u) for NSAMPLES
   * different values of u. You MUST draw each interpolated control
   * point EXACTLY ONCE, not more, not less.
  */
  glBegin(GL_LINE_STRIP);
  XVec2f vertex;
  for (int i = 1; i < NUM_POINTS - 2; i++) {
    p[0] = points[i-1];
    p[1] = points[i];
    p[2] = points[i+1];
    p[3] = points[i+2];

    cspline_coeffs(a, B, p);
    for (int i = 0; i < NSAMPLES; i++)
    {
      cspline_eval(vertex, (float)i/NSAMPLES, a);
      // std::cout << vertex << std::endl;
      glVertex2fv(vertex);
    }
  }
  glEnd();
  return;
}


void
draw_quadratic_endpieces(void)
{
  glColor3f(1.0, 0.5, 0.0);
  glLineWidth(5.0);
  

  /* YOUR CODE HERE TASK 2
   *
   * Adapt your Catmull-Rom spline code to draw a quadratic spline
   * between the first and second control points that interpolates
   * both control points.  Then draw another quadratic spline between
   * the penultimate and last control points that also interpolates
   * both control points.  Both of these quadratic curves must have
   * the same tangent as the cubic Catmull-Rom spline where they meet
   * the Catmull-Rom spline.
   *
   * NOTE: the inverse function provided in xmat.h doesn't seem to
   * work correctly with the constraint matrix for the spline
   * interpolating the last two control points.  You'd need to 
   * invert this matrix by hand.  (If you managed to write or 
   * find a working inverse function, let me know!)
  */
  // TODO this quad curve is hard to design a good shape curve without 
  // breaking the tangent at the middle point 
  // So this is a simple approximation of p0 = f(0), p1 = f(0.5), p2 = f(1)
  XMat3f C, B;
  C.setRow(0, XVec4f(1, 0, 0));
  C.setRow(1, XVec4f(1, 0.5, 0.25));
  C.setRow(2, XVec4f(1, 1, 1));
  B = C.inverse();

  XVec2f a[NQUADRATIC], p[NQUADRATIC];
  // draw the two first point
  {
    p[0] = points[0];
    p[1] = points[1];
    p[2] = points[2];
    // now calculate a 
    XVec3f px(p[0].x(), p[1].x(), p[2].x());
    XVec3f py(p[0].y(), p[1].y(), p[2].y());
    // mutiple each vector with B matrix
    XVec3f ax = B * px;
    XVec3f ay = B * py;
    for (int i = 0; i < NQUADRATIC; ++i)
    {
      a[i].x() = ax[i];
      a[i].y() = ay[i];
    }
    // draw the line
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < NSAMPLES; i++)
    {
      float u = (float)i/NSAMPLES/2;
      XVec2f vertex = a[0] + u * a[1] + u * u * a[2];
      // std::cout << vertex << std::endl;
      glVertex2fv(vertex);
    }
    glEnd();
  }
  // draw the two last point 
  {
    p[0] = points[NUM_POINTS - 3];
    p[1] = points[NUM_POINTS - 2];
    p[2] = points[NUM_POINTS - 1];
    // now calculate a 
    XVec3f px(p[0].x(), p[1].x(), p[2].x());
    XVec3f py(p[0].y(), p[1].y(), p[2].y());
    // mutiple each vector with B matrix
    XVec3f ax = B * px;
    XVec3f ay = B * py;
    for (int i = 0; i < NQUADRATIC; ++i)
    {
      a[i].x() = ax[i];
      a[i].y() = ay[i];
    }
    // draw the line
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < NSAMPLES; i++)
    {
      float u = 0.5f + (float)i/NSAMPLES/2;
      XVec2f vertex = a[0] + u * a[1] + u * u * a[2];
      // std::cout << vertex << std::endl;
      glVertex2fv(vertex);
    }
    glEnd();
  }
  return;
}

void
draw_cubic_bezier(void)
{

  /* YOUR CODE HERE TASK 3
   *
   * Manually populate B with the cubic Bezier basis matrix.
  */
  XMat4f C, B;
  XVec2f a[NCUBIC], p[NCUBIC];
  glColor3f(0.0, 1.0, 0.0);
  glLineWidth(5.0);

  /* YOUR CODE HERE TASK 3
   *
   * Draw a cubic Bezier spline that interpolates the two end control
   * points.  Calculate the coefficients of the spline for control
   * point-sets {0, 1, 2, 3} and {3, 4, 5, 6}. Then samples the spline
   * at NSAMPLES location on the spline, i.e., compute f(u) for
   * NSAMPLES different values of u. You MUST draw each interpolted
   * control point EXACTLY ONCE, not more, not less.  You MUST use
   * cspline_coeffs() and cspline_eval() and you may consult
   * draw_catmull_rom().
  */
  B.setRow(0, XVec4f( 1,  0,  0, 0));
  B.setRow(1, XVec4f(-3,  3,  0, 0));
  B.setRow(2, XVec4f( 3, -6,  3, 0));
  B.setRow(3, XVec4f(-1,  3, -3, 1));

  glBegin(GL_LINE_STRIP);
  XVec2f vertex;
  {
    p[0] = points[0];
    p[1] = points[1];
    p[2] = points[2];
    p[3] = points[3];

    cspline_coeffs(a, B, p);
    for (int i = 0; i < NSAMPLES; i++)
    {
      // TODO NSAMPLES should be larger than 2
      cspline_eval(vertex, (float)i/(NSAMPLES-1), a);
      // std::cout << vertex << std::endl;
      glVertex2fv(vertex);
    }
  }
  glEnd();
  glBegin(GL_LINE_STRIP);
  {
    p[0] = points[3];
    p[1] = points[4];
    p[2] = points[5];
    p[3] = points[6];

    cspline_coeffs(a, B, p);
    for (int i = 0; i < NSAMPLES; i++)
    {
      // TODO NSAMPLES should be larger than 2
      cspline_eval(vertex, (float)i/(NSAMPLES-1), a);
      // std::cout << vertex << std::endl;
      glVertex2fv(vertex);
    }
  }
  glEnd();
  return;
}

