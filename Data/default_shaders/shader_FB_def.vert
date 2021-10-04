#version 130 

smooth out vec4 inColor;
smooth out vec2 inPos;

uniform sampler2D text;

void main(){
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  inPos = gl_Position.xy;
  gl_TexCoord[0] = gl_MultiTexCoord0;
}
