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
 * Authors: Sugih Jamin and Igor Guskov
 *
*/
#version 120

varying vec4 position;
varying vec3 normal;
varying vec3 viewVec;

void 
main(void)
{
  /* YOUR CODE HERE: 
   * Replace this file with your phong.fs and then modify your
   * implementation of per-pixel shading to use the Blinn-Phong 
   * algorithm.  
  */
  vec4 finalColor = vec4(0.0, 0.0, 0.0, 0.0);
  gl_FragColor = gl_FrontLightModelProduct.sceneColor;
  
  
  vec3 N = normalize(normal);
  vec3 V = normalize(viewVec);
  
  for (int i = 0; i < gl_MaxLights; i++)
  {
      vec3 L = gl_LightSource[i].position.xyz - position.xyz;
      float distance = length(L);
      L = normalize(L);
     
      
      float NdotL = dot(N, L);
      
      vec3 H = normalize(L + V);
      float NdotH = max(0.0, dot(N, H));

      vec4 k_amb = gl_FrontLightProduct[i].ambient;
      vec4 k_diffuse = gl_FrontLightProduct[i].diffuse;
      vec4 k_specular = gl_FrontLightProduct[i].specular;
      vec4 I_amb = k_amb;
      vec4 I_diffuse = k_diffuse * max(NdotL, 0.0);
      vec4 I_specular = k_specular * pow(NdotH, gl_FrontMaterial.shininess);
      
      float attenuation = 1.0/(gl_LightSource[i].constantAttenuation
           + gl_LightSource[i].linearAttenuation * distance
           + gl_LightSource[i].quadraticAttenuation * distance * distance);
      
      vec4 I_sum = I_amb + I_diffuse + I_specular;
      finalColor += attenuation * I_sum;
  }
  
  gl_FragColor += finalColor;
  gl_FragColor.a = 1.0;
}
