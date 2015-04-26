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
 * Authors: Igor Guskov, Sugih Jamin
 *
*/
#version 120

varying vec3 normal_eye;

void 
main(void) 
{
  /* YOUR CODE HERE: you might want to consult Example 4: Ivory
   * from the Progammable Shaders lecture slides 
   * Replace this whole function with your own interesting shader.
  */
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  normal_eye = normalize(gl_NormalMatrix*gl_Normal);

}
