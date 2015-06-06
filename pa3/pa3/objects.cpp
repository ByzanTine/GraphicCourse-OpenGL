/*
 * Copyright (c) 2012 University of Michigan, Ann Arbor.
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
 *
*/
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
using namespace std;

#ifdef __APPLE__
#include <GLUT/glut.h>
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "ltga.h"
#include "xvec.h"
#include "objects.h"

static GLenum gl_error;

#define SLICES 1000 /* number of azimuthal (longitudinal) partitions */
#define STACKS 1000 /* number of latitudinal partitions */

extern GLuint spd;
extern GLuint *tods;
extern int ntods;
extern GLuint *nmods;
extern int nnmods;

enum { OBJ, IDX, NOBS };

typedef struct {
  XVec3f position;
  XVec2f texcoords;
  /* TASK 7: YOUR CODE HERE
   * add a vertex attribute for tangent */
} sphere_vertex_t;

bool
init_sphere()
{
  GLuint vbods[NOBS];
  int i, j, k, k0, k1;
  double theta, phi;
  double fullcircle = M_PI*2.0;
  double inc_theta = M_PI/((double) STACKS);
  double laststack = M_PI-inc_theta;
  double inc_phi = fullcircle/((double) SLICES);
  double costheta, sintheta;
  double cosphi, sinphi;
  int midrift;
  sphere_vertex_t *vertices;
  GLuint *vertidx;

  glGenBuffers(NOBS, vbods);
  if ((gl_error = glGetError()) != GL_NO_ERROR) {
    cerr << "GenBuffers sphere: [Error " 
         << gl_error << "] " << gluGetString(gl_error) << endl;
    exit(1);
  }

  glBindBuffer(GL_ARRAY_BUFFER, vbods[OBJ]);
  if ((gl_error = glGetError()) != GL_NO_ERROR) {
    cerr << "BindBuffer sphere: [Error " 
         << gl_error << "] " << gluGetString(gl_error) << endl;
    exit(1);
  }
  /* Allocate enough space for the vertex array
   * (3-coords position element) (= normal array
   * for a smooth sphere) and the texture
   * coordinates array (2-coords element)
   * Total: 5*sizeof(float) = sizeof(sphere_vertex_t)
   */
  glBufferData(GL_ARRAY_BUFFER, (SLICES+1)*(STACKS+1)*sizeof(sphere_vertex_t), 
               0, GL_STATIC_DRAW);
  if ((gl_error = glGetError()) != GL_NO_ERROR) {
    cerr << "BufferData sphere: [Error " 
         << gl_error << "] " << gluGetString(gl_error) << endl;
    exit(1);
  }

  /*
   * TASK 1: replace the following line with your code from Lab6      
   *
   * Map the allocated buffer to an array of sphere_vertex_t,
   * which interleaves the position and texcoords of
   * each vertex.
  */
  vertices = (sphere_vertex_t *) malloc((SLICES+1)*(STACKS+1)*sizeof(sphere_vertex_t));
  
  /* Sphere */                          // we'll do the north pole together
  for (i = SLICES+1, theta = inc_theta; // with the south pole later
       theta <= laststack; theta += inc_theta) {
    // assume r = 1.0;
    costheta = cos(theta);
    sintheta = sin(theta);

    for (phi = 0.0; phi <= fullcircle; phi += inc_phi, i++) {
      cosphi = cos(phi);
      sinphi = sin(phi);
      vertices[i].position.x() = sintheta*cosphi;
      vertices[i].position.y() = sintheta*sinphi;
      vertices[i].position.z() = costheta;

      /* TASK 1: copy your code from Lab6 */
      /* assign texture coordinates per vertex */

    }
  }

  for (j = 0, phi = 0.0; j <= SLICES; j++, phi += inc_phi, i++) {
    // north pole, half of the triangles become lines, but
    // the result looks fine and it simplifies the code . . . .
    vertices[j].position = XVec3f(0.0, 0.0, 1.0);
    // south pole
    vertices[i].position = XVec3f(0.0, 0.0, -1.0);

    /* TASK 1: copy your code from Lab6 */
    /* assign texture coordinates to the pole vertices */

  }

  /* TASK 7:
   * Compute tangent of each triangle and assign
   * them as attributes of the three vertices
   * Don't forget to wrap around at the seam.
  */
  
  /*
   * TASK 1: replace the following three lines
   * with your code from Lab6
   *
   * Release the mapped buffer.
  */
  glBufferSubData(GL_ARRAY_BUFFER, 0,
               (SLICES+1)*(STACKS+1)*sizeof(sphere_vertex_t), vertices);
  free(vertices);

  /* TASK 5: YOUR CODE HERE
   * get shader vertex position and normal attribute locations
   */

  /* TASK 6: YOUR CODE HERE
   * get shader vertex texture coordinates locations
   */

  /* TASK 7: YOUR CODE HERE
   * get shader tangent attribute locations
   */

  /* Enable client-side vertex position and normal attributes
   * and set up pointer to the arrays.  Since we're interleaving
   * the vertex position (XVec3f) and texcoords (XVec2f), we need
   * to specify a stride of sizeof(XVec3f)+sizeof(XVec2f).
   */
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(sphere_vertex_t), 0);
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, sizeof(sphere_vertex_t), 0);
  // Automatic normalization of normals
  glEnable(GL_NORMALIZE);
  /* TASK 5: YOUR CODE HERE
   * Comment out the five function calls above
   * and replace them with shader version.
   */

  /* 
   * TASK 1: copy your code from Lab6
   *
   * Enable client-side vertex texture coordinates attribute
   * array and set up pointer to the array.  The first element
   * of the texture-coordinates array is after the first vertex
   * position coordinates.  Since we're interleaving
   * the vertex position (XVec3f) and texcoords (XVec2f),
   * subsequent texture coordinates are at stride
   * sizeof(XVec2f)+sizeof(XVec3f) bytes from the start
   * of the current texture coordinates.
   *
   * Caveat: make sure your texture coordinate pointer is
   * set to BYTE offset from the start of the mapped buffer
   * object.
  */

  /* TASK 6: YOUR CODE HERE
   *
   * Replace the client-side vertex texture coordinates attribute
   * array and pointer to it in TASK 1 above with shader version.
   */

  /* TASK 7: YOUR CODE HERE
   * Enable shader vertex tangent attribute
   * and set up pointer to the array.  The first element
   * of the tangent array is after the first vertex
   * position and texture coordinates.
   */

  /* Bind the GL_ELEMENT_ARRAY_BUFFER and allocate enough space
   * in graphics system memory to hold the element index
   * array to be used by glDrawElement().
  */
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[IDX]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, (SLICES+1)*STACKS*2*sizeof(GLuint),
               0, GL_STATIC_DRAW); 

  /*
   * TASK 1: replace the line below with your code from Lab6
   *
   * Map the allocated buffer to the "index" variable.
   * allocate enough space for the index array
   * for use by glDrawElement().
   */
  vertidx = (GLuint *) malloc((SLICES+1)*STACKS*2*sizeof(GLuint));

  // triangle strip
  //   - the poles cannot use triangle fan due to texcoords
  midrift = STACKS*(SLICES+1);  
  for (i=0, k=0, k0=0, k1=SLICES+1; 
       i < midrift;
       i++, k += 2, k0++, k1++) {
    vertidx[k] = k0;
    vertidx[k+1] = k1;
  }

  /* 
   * TASK 1: replace the following two calls with your code from Lab6
   *
   * Release the mapped index buffer.
  */
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                  (SLICES+1)*STACKS*2*sizeof(GLuint),
                  vertidx);
  free(vertidx);
  
  return true;
}

void
draw_sphere()
{
  int i;
  static const int countperstack = 2*(SLICES+1); 

  for (i = 0; i < STACKS; i++) {
    glDrawElements(GL_TRIANGLE_STRIP, countperstack, GL_UNSIGNED_INT, 
                   (GLvoid *)(i*countperstack*sizeof(GLuint)));
  }

  return;
}


bool
init_cube()
{
  /*
   * TASK 2: YOUR CODE HERE
   * Initialize a cube.
   */
  
  /* TASK 5: YOUR CODE HERE
   * Replace your use of client-side vertex position 
   * and normal attribute pointers with shader version.
   *
   * TASK 6: YOUR CODE HERE
   * Replace your use of client-side texture coordinates
   * pointer with shader version.
   *
   * TASK 7: YOUR CODE HERE
   * Pass vertex tangent to the shader as a vertex attribute.
   */

  return true;
}

void
draw_cube()
{
  /*
   * TASK 2: YOUR CODE HERE
   * Draw your cube.
   */

  return;
}


bool
init_world(objType drawobject)
{
  extern bool scene;
  
  switch (drawobject) {
  case SPHERE:
    return(init_sphere());
    break;
  case CUBE:
    return(init_cube());
    break;
  default:
    scene = true;

    /* TASK 3: YOUR CODE HERE
     *
     * Generate NOBJS vertex-array objects, then
     * bind to the SPHERE handle and call init_sphere(),
     * and bind to the CUBE handle and call init_cube().
    */
    break;
  }
  
  return true;
}

void
draw_world(objType drawobject)
{
  switch (drawobject) {
  case SPHERE: 
    /* TASK 3: YOUR CODE HERE
     * Bind the vertex array object for sphere.
     *
     * TASK 4: YOUR CODE HERE
     * Bind the texture for sphere.
     *
     * TASK 7: YOUR CODE HERE
     * Bind the normal map for sphere.
     */
    draw_sphere();
    break;

  case CUBE: 
    /* TASK 3: YOUR CODE HERE
     * Bind the vertex array object for cube.
     *
     * TASK 4: YOUR CODE HERE
     * Bind the texture for cube.
     *
     * TASK 7: YOUR CODE HERE
     * Bind the normal map for cube.
     */
    draw_cube();
    break;

  default: // whole scene of multiple objects
    /* TASK 7: YOUR CODE HERE
     *
     * Display at least three objects, using BOTH sphere and cube.
     *
     * Texture each object with a different texture.
     * You'll need to acces the global variables
     * "tods" and "ntods".
     * Make a scene. Be creative!
     *
     * Don't forget to activate the correct texture unit
     * and bind the right normal map also.
     */
    break;
  }
  
  return;
}
