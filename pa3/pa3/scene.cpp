/*
 * Copyright (c) 2007, 2011, 2012 University of Michigan, Ann Arbor.
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
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
using namespace std;

#ifndef _WIN32
#include <unistd.h>
#else
#include "wingetopt.h"
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "ltga.h"
#include "xvec.h"
#include "objects.h"
#include "shaders.h"


int wd;
GLdouble screen_width=800, screen_height=600;

typedef struct {
  char *vs;
  char *fs;
  //  GLuint pd;
} shader_t;
enum ShaderModeEnum {
  NONE,
  GOURAUD,
  PHONG,
  BLINN,
  NMAP, // BLINN
  PHONG_NMAP,
  NSHADERS
} shader_mode = NMAP; // TASK 5: replace NONE with NMAP
shader_t shaders[] = {
  { NULL, NULL },
  { (char *)"gouraud", (char *)"gouraud" },
  { (char *)"phong", (char *)"phong" },
  { (char *)"phong", (char *)"blinn" },
  { (char *)"nmap", (char *)"nmap" }, // Blinn
  { (char *)"nmap", (char *)"phong-nmap" },
};
GLuint spd=0;             /* shader program handles */

GLuint *tods;  // texture object descriptors
GLuint *nmods; // normal map object descriptors
int ntods, nnmods;
objType drawobject;
bool scene=false;

GLfloat position[] = { 0.0, 0.0, 2.0, 0.0 };  // directional
GLfloat diffuse[] = { 0.8, 0.5, 0.6, 1.0 };
//GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat specular[] = {0.0, 0.8, 0.0, 1.0 };
//GLfloat ambient[] = {0.4, 0.4, 0.2, 1.0};
GLfloat ambient[] = {0.5, 0.5, 0.5, 1.0};
GLfloat mat_specular[] = { .7, 1.0, 4.0, 1.0 };
GLfloat mat_shininess[] = { 50.0 };
GLfloat mat_ambient[] = { 0.2, 0.2, 0.3, 1.0 };

bool
init_lights(void)
{
  /* Lighting and material properties */
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);

  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

  glLightfv(GL_LIGHT0, GL_POSITION, position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);

  return true;
}

bool
load_texture(LTGA *texture)
{
  /* 
   * TASK 2: YOUR CODE HERE
   *
   * Set up autogeneration of mipmap:
   * - specify GL_TEXTURE_MAX_LEVEL using glTexParameteri()
   * - auto generate mipmap by setting GL_GENERATE_MIPMAP
   *   to GL_TRUE using glParameteri() BEFORE calling
   *   glTexImage2D().
  */
  // glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL, 30);
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

  /* TASK 1: YOUR CODE HERE
   *
   * Specify the texture and set its parameters. 
   * To actually map the texture onto the sphere,
   * go back to the init_sphere() functions and assign a pair of 
   * texture coordinates to each vertex forming the object.
   * You may want to use the GetAlphaDepth() and GetImageType()
   * methods of LTGA (see ltga.h and ltga.cpp ) to determine
   * whether the image is greyscale (GL_LUMINANCE or
   * GL_LUMINANCE_ALPHA) or color (GL_RGB or GL_RGBA).
   *
   * TASK 3: YOUR CODE HERE
   * Modify your call to glTexImage2D() to work with pixel 
   * buffer object
   */
  /* Specify texture */
  GLsizei width = texture->GetImageWidth();
  GLsizei height = texture->GetImageHeight();
  GLenum image_type = texture->GetImageType();
  // TODO handle image_type
  // glEnable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_2D);
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width,
                 height,0, GL_RGB, GL_UNSIGNED_BYTE, 0); // texture->GetPixels() for last argument 
  int err = glGetError(); assert(err == GL_NO_ERROR); 
  // int err = glGetError(); assert(err == GL_NO_ERROR); 
  /* Set texture parameters */
  // linearly interpolate between 4 nearest texel values while
  // shrinking or stretching
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  
  // clamp texture coordinates (s,t) to [0,1] each
  /* BGIN SOLUTION */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  /* END SOLUTION */

  // Simply copy the texture, don't modulate or blend etc
  // and enable texture

  // perspective correct interpolation
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


  return true;
}

void
toggle_mipmapping()
{
  static bool mipmapping=false;

  mipmapping = !mipmapping;

  if (mipmapping) {
    /* 
     * TASK 1: copy from Lab6
     * Turn on mipmapping using glTexParameteri().
     * Assume mipmap has been generated in load_texture().
     */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  } else {
    // linearly interpolate between 4 nearest texel values while 
    // shrinking or stretching
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }
  cerr << "Mipmapping " << (mipmapping ? "ON" : "OFF") << endl;

  return;
}

/*
 * called by LTGA::LoadFromFile()
 *
 * Allocate graphics memory of given size, map it to client
 * address space, and return a pointer to the mapped memory.
 *
 * Assume texture object has been generated by glGenBuffers().
 * and that it has been bound to PBO.
 */
void *
pbo_alloc(GLuint size)
{
  /* TASK 1: Replace the call to malloc with your code from lab6 */
  glBufferData(GL_PIXEL_UNPACK_BUFFER, size, NULL, GL_STREAM_DRAW);
  return glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_WRITE);
}

void
read_texture(LTGA *texture, const char *texture_file)
{
  texture->LoadFromFile(texture_file);
  if (!texture->IsLoaded()) {
    cerr << texture_file << ": image not loaded" << endl;
    exit(1);
  }
  cout << "Texture: " << endl;
  cout << "    Image type: " << LImageTypeString[texture->GetImageType()] 
       << " (" << texture->GetImageType() << ")" << endl;
  cout << "   Image width: " << texture->GetImageWidth() << endl;
  cout << "  Image height: " << texture->GetImageHeight() << endl;
  cout << "   Pixel depth: " << texture->GetPixelDepth() << endl;
  cout << "   Alpha depth: " << texture->GetAlphaDepth() << endl;
  cout << "   RL encoding: " << (texture->GetImageType() > (LImageType) 8) << endl;
  /* use texture->GetPixels()  to obtain the pixel array */

  return;
}

bool
init_textures(int ntexs, char *texfiles[])
{
  LTGA texture;

  
  if (!ntexs) {
    return true;
  }
  
  /* 
   * TASK 1: copy from Lab6
   * Generate a pixel buffer object and
   * bind it to the pixel unpack buffer
   */
  GLuint pixelBuffer;
  glGenBuffers(1, &pixelBuffer);
  
  /* TASK 4: YOUR CODE HERE
   * Generate ntods texture objects (tods) to load ntexs textures,
   * instead of just loading one texture into tod.  Both "ntods"
   * and "tods" are global variables.
  */
  ntods = ntexs;
  tods = (GLuint *) malloc(sizeof(GLuint)*ntods);
  glGenTextures(ntods, tods);
  for (int i = 0; i < ntexs; ++i)
  {
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixelBuffer);
    read_texture(&texture, texfiles[i]);

    /* 
     * TASK 1: copy from Lab6
     * Release the mapped pixel buffer object,
     * otherwise glTexImage2D() won't have
     * access to the buffer.
     */
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    /* 
     * TASK 1: *adapt* from Lab6
     * Generate a texture object, put the descriptor
     * in tods[0] and bind it to the 2D texture object
     */
    glBindTexture(GL_TEXTURE_2D, tods[i]);
    /* TASK 4: YOUR CODE HERE
     * Replace the above TASK 1 by binding each
     * of the ntods elements of tods to the
     * 2D texture object
     */
    
    load_texture(&texture);

  }
  


  /*
   * TASK 1: copy from Lab6
   * Now that the texture has been unpacked from the
   * pbo to the texture object, delete the pixel
   * buffer object, which also automatically unbinds
   * the pixel unpack buffer.
   */
  glDeleteBuffers(1, &pixelBuffer);
  /* TASK 6:
   * Pass the default texture unit (GL_TEXTURE0)
   * as a uniform variable to the shader.
   */
  if (shader_mode == NMAP)
  {
    GLint tex0_id = glGetUniformLocation(spd, "texture");
    glUniform1i(tex0_id, 0);
  }
  
  return true;
}

/* TASK 7: YOUR CODE HERE
 * Load nnmaps normal map files into the nnmods texture objects
 * (nmods) OF TEXTURE UNIT 1, instead of texture unit 0.
 * [There is a better way to structure the code to avoid
 * having two functions (init_textures() and init_nmaps()
 * that are practically identical, but this way is simplest,
 * from a task separation point of view . . . .]
 */
bool
init_nmaps(int nnmaps, char *nmapfiles[])
{
  LTGA normalMap;

  
  if (!nnmaps) {
    return true;
  }
  
  /* 
   * TASK 1: copy from Lab6
   * Generate a pixel buffer object and
   * bind it to the pixel unpack buffer
   */
  GLuint pixelBuffer;
  glGenBuffers(1, &pixelBuffer);
  
  /* TASK 4: YOUR CODE HERE
   * Generate ntods texture objects (tods) to load ntexs textures,
   * instead of just loading one texture into tod.  Both "ntods"
   * and "tods" are global variables.
  */
  nnmods = nnmaps;
  nmods = (GLuint *) malloc(sizeof(GLuint)*nnmods);
  glGenTextures(nnmods, nmods);
  for (int i = 0; i < nnmods; ++i)
  {
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixelBuffer);
    read_texture(&normalMap, nmapfiles[i]);

    /* 
     * TASK 1: copy from Lab6
     * Release the mapped pixel buffer object,
     * otherwise glTexImage2D() won't have
     * access to the buffer.
     */
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    /* 
     * TASK 1: *adapt* from Lab6
     * Generate a texture object, put the descriptor
     * in tods[0] and bind it to the 2D texture object
     */
    glBindTexture(GL_TEXTURE_2D, nmods[i]);
    /* TASK 4: YOUR CODE HERE
     * Replace the above TASK 1 by binding each
     * of the ntods elements of tods to the
     * 2D texture object
     */
    
    load_texture(&normalMap);

  }
  


  /*
   * TASK 1: copy from Lab6
   * Now that the texture has been unpacked from the
   * pbo to the texture object, delete the pixel
   * buffer object, which also automatically unbinds
   * the pixel unpack buffer.
   */
  glDeleteBuffers(1, &pixelBuffer);
  /* TASK 6:
   * Pass the default texture unit (GL_TEXTURE0)
   * as a uniform variable to the shader.
   */
  if (shader_mode == NMAP)
  {
    GLint tex0_id = glGetUniformLocation(spd, "normalMap");
    glUniform1i(tex0_id, 1);
  }
  
  return true;
}

void
display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  draw_world(drawobject);
  
  glutSwapBuffers();

  return;
}

void
reshape(int w, int h)
{
  /* Save new screen dimensions */
  screen_width = (GLdouble) w;
  screen_height = (GLdouble) h;

  /* Instruct Open GL to use the whole window for drawing */
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);

  /* Set the perspective - later in this course */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(55, screen_width/screen_height, 0.1, 25);
  glMatrixMode(GL_MODELVIEW);

  return;
}

void
kbd(unsigned char key, int x, int y)
{
  static bool in_texture_mode = true;

  // set the matrix mode to model-view
  glMatrixMode(GL_MODELVIEW);

  switch((char)key) {
  case 'q':
  case 27:
    glutDestroyWindow(wd);
    exit(0);
    break;

  case 'c':
    if (scene) {
      drawobject = CUBE;
    } else {
      cerr << "Scene not yet implemented" << endl;
    }
    break;
    
  case 'b':
    if (scene) {
      drawobject = SPHERE;
    } else {
      cerr << "Scene not yet implemented" << endl;
    }
    break;
    
  case 'a':
    drawobject = NOBJS;
    if (!scene) {
      init_world(drawobject);
    }
    break;
    
  case 'h':
  case 'z':
    glRotatef(5,0,0,1);
    break;

  case 'l':
  case 'Z':
    glRotatef(-5,0,0,1);
    break;

  case 'j':
  case 'x':
    glRotatef(-5,1,0,0);
    break;

  case 'k':
  case 'X':
    glRotatef(5,1,0,0);
    break;

  case 'w':
  case 'Y':
    glRotatef(5,0,1,0); 
    break;

  case 's':
  case 'y':
    glRotatef(-5,0,1,0); 
    break;

  case 'm':
    glScalef(0.9, 0.9, 0.9);
    break;

  case 'M':
    glScalef(1.1, 1.1, 1.1);
    break;

  case 'i':
    toggle_mipmapping();
    break;

  case 't':
    in_texture_mode = !in_texture_mode;
    if (in_texture_mode) {
      glEnable(GL_TEXTURE_2D); 
    } else {
      glDisable(GL_TEXTURE_2D);
    }
    break;

  default:
    return;
  }

  glutPostRedisplay();

  return;
}

void
init_gl()
{
  /* Initial camera location and orientation */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0, 0, 3, 0, 0, 0, 0, 1, 0);

  glPolygonMode(GL_FRONT, GL_FILL);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_CULL_FACE);

  return;
}

#define NTEXFILES 6

int
main(int argc, char *argv[])
{
  char opt;
  int ntexs=-1;
  char *texfiles[NTEXFILES];
  int nnmaps=-1;
  char *nmapfiles[NTEXFILES];
  
  while ((opt = getopt(argc, argv, "gpbt:n:ve")) != -1) {
    switch (opt) {
    case 'g':
      if (shader_mode != NONE) {
        cerr << "-g: Multiple shader programs specified." << endl;
        exit(-1);
      }
      shader_mode = GOURAUD;
      break;
    case 'p':
      if (shader_mode == NMAP) {
        shader_mode = PHONG_NMAP;
      } else if (shader_mode == NONE) {
        shader_mode = PHONG;
      } else {
        cerr << "-p: Multiple shader programs specified." << endl;
        exit(-1);
      }
      break;
    case 'b':
      if (shader_mode == NONE) {
        shader_mode = BLINN;
      } else if (shader_mode != NMAP) {
        cerr << "-b: Multiple shader programs specified." << endl;
        exit(-1);
      }
      break;
    case 't':
      ntexs++;
      if (ntexs >= NTEXFILES) {
        cerr << "Doesn't support more than " << NTEXFILES
             << " texture files" << endl;
        exit(-1);
      }
      texfiles[ntexs] = optarg;
      break;
    case 'n':
      nnmaps++;
      if (nnmaps >= NTEXFILES) {
        cerr << "Doesn't support more than " << NTEXFILES
             << " normal maps" << endl;
        exit(-1);
      }
      nmapfiles[nnmaps] = optarg;
      if (shader_mode == PHONG) {
        shader_mode = PHONG_NMAP;
      } else if (shader_mode == BLINN || shader_mode == NONE) {
        shader_mode = NMAP;
      } else if (shader_mode != NMAP) {
        cerr << "-n: Multiple shader programs specified" << endl;
        exit(-1);
      }
      break;
    default:
      cerr << "Unknown command line option: " << opt << endl;
      exit(-1);
    }
  }
      
  if (ntexs < 0) {
    cerr << "Usage: scene -t <texture_file>.tga -n <normal_map>.tga [-t <texture_file>.tga . . . -n <normal_map>.tga . . .]" << endl;
    return -1;
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize((int)screen_width, (int)screen_height);
  wd = glutCreateWindow("PA3: Multiple Objects with Normal Mapping");
#ifndef __APPLE__
  GLenum err = glewInit();
  if (GLEW_OK != err)
    {
      /* Problem: glewInit failed, something is seriously wrong. */
      cerr << "Error: " << glewGetErrorString(err) << endl;
    }
  cout <<  "Status: Using GLEW " << glewGetString(GLEW_VERSION) << endl;
#endif

  if (shader_mode == NONE) {
    cout << "No shader program given, using fixed function pipeline." << endl;
  } else if (shader_mode != NMAP) {  
    // feel free to add support for the other shader modes
    cerr << "Shader mode not supported in this version of the code." << endl
         << "Using fixed function pipeline." << endl;
    shader_mode = NONE;
  } else {
    /* shader creation must come after context creation */
    if (!InitShaders(shaders[shader_mode].vs, shaders[shader_mode].fs, &spd)) {
      cerr << "Shaders " << shaders[shader_mode].vs << "; " <<
        shaders[shader_mode].fs << " init failed. Exiting." << endl;
      exit(-1);
    }
    glUseProgram(spd);
  }

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(kbd);

  init_gl();
  /* YOUR CODE HERE
   * TASK 2: drawobject = CUBE
   * TASK 3: drawobject = NOBJS;
   */

  // drawobject = SPHERE;
  drawobject = NOBJS;
  //END PROBLEM*/
  if (init_lights() && 
      init_textures(ntexs+1, texfiles) &&
      init_nmaps(nnmaps+1, nmapfiles) &&
      init_world(drawobject)) {
    glutMainLoop();
    return(0);
  } else {
    return(1);
  }
}
