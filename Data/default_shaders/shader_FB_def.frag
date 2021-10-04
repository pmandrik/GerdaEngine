#version 130

smooth in vec4 inColor;
smooth in vec2 inPos;

uniform sampler2D text_0;

uniform float time;

void main(){
  vec2 pos  = gl_TexCoord[0].st;
  vec4 c_a  = texture2D(text_0, pos);

  float val_r = 1 - c_a.x;
  float val_g = 1 - c_a.y;
  float val_b = 1 - c_a.z;

  vec4 color = vec4(val_r, val_g, val_b, 1.0);
  gl_FragColor = color;
}






