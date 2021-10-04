/*

*/

#version 130

smooth in vec4 inColor;
smooth in vec2 inPos;

uniform sampler2D text_0;
uniform sampler2D text_1;

uniform float time;
uniform float var_0;
uniform float var_1;
uniform float var_2;
uniform float var_3;
uniform float var_4;
uniform float var_5;
uniform float var_6;

void main(){
  vec2 pos  = gl_TexCoord[0].st;
  vec4 c_a  = texture2D(text_0, pos);
  vec4 c_b  = texture2D(text_1, pos);

  float val_r = c_a.x * var_0 + c_b.x * var_1;
  float val_g = c_a.y * var_0 + c_b.y * var_1;
  float val_b = c_a.z * var_0 + c_b.z * var_1;

  vec4 color = vec4(val_r, val_g, val_b, 1.0);
  gl_FragColor = color;
}






