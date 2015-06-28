/*
 * Copyright (c) 2012, 2013 University of Michigan, Ann Arbor.
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
 * Authors: Sugih Jamin
 *
 * Per-vertex eye-to-tangent transformation of light and view 
 * vectors with per-fragment tangent-space lighting computation 
 * using normal map
*/
#version 120

varying vec4 position;
varying vec3 normal, view, light;

/* TASK 5:
 * Define vertex attribute variables
 * for position and normal.
 */
attribute vec4 va_Vertex;

attribute vec3 va_Normal;
/* TASK 6: YOUR CODE HERE
 * You need to input texture coordinates
 * from the application and pass it
 * along to the fragment shader.
*/
attribute vec2 va_Textcoord;
varying vec2 texcoord;

/* TASK 7: YOUR CODE HERE
 * Input tangent and pass tangent-space
 * view and light vectors to fragment shader
*/
attribute vec3 va_Tangent;
void 
main(void) 
{
  /* TASK 5: YOUR CODE HERE
   * Replace the use of gl_Vertex and gl_Normal
   * below with your own vertex attributes.
  */
  position = gl_ModelViewMatrix*va_Vertex;
  gl_Position = gl_ModelViewProjectionMatrix*va_Vertex;
  normal = gl_NormalMatrix*va_Normal;

  /* TASK 6: YOUR CODE HERE
   * 
   * pass texure coordinates from application
   * to the fragment shader.
  */
  texcoord = va_Textcoord;
  /* TASK 7: YOUR CODE HERE
   *
   * Compute orthonormal B', T', and [T'B'N]
   *
   * Replace the following two lines with your
   * eye and light vectors in tangent space
  */
  vec4 homo_tangent = vec4(va_Tangent.x, va_Tangent.y, va_Tangent.z, 0.0f);
  vec3 tangent = normalize(gl_ModelViewMatrix * homo_tangent).xyz; // eye space 
  vec3 bitTangent = cross(normal, tangent);
  mat3 TBNMatrix = mat3(tangent, bitTangent, normal);
  mat3 iTBNMatrix = transpose(TBNMatrix);


  // view = vec3(0.0, 0.0, 1.0);
  // light = vec3(0.0, 0.0, 1.0);
  view = normalize(iTBNMatrix * -position.xyz);
  light = normalize(iTBNMatrix * (gl_LightSource[0].position.xyz - position.xyz));



}
