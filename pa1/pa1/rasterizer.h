/*
 * Copyright (c) 2010, 2011 University of Michigan, Ann Arbor.
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

#ifndef RASTERIZER_H
#define RASTERIZER_H

#include "xvec.h"

#define LINE            0
#define TRIANGLE        1

class Line {
 public: 
  Line();
  virtual ~Line() {}
  virtual void drawInRect(XVec4f &clipWin);
  virtual char type() { return LINE; }


  XVec2f vertex0, vertex1;
  XVec4f color0, color1;
  bool isAntialiased;
 private:
  float fmid(int vertex[2][2], float x, float y);
  int Cohen_Sutherland_Preprocess(XVec4f &clipWin);
};

class Triangle:public Line {    
 public:
  Triangle();
  virtual ~Triangle() {}
  virtual void drawInRect(XVec4f &clipWin);
  virtual char type() { return TRIANGLE; }
  
  bool containsPoint(XVec2f &point, XVec4f &pointColor);
  
  XVec2f vertex2;
  XVec4f color2;
  int init;
private:
  struct BoundingBox {
    int xmin;
    int ymin;
    int xmax;
    int ymax;
  };

};

#endif // RASTERIZER_H
