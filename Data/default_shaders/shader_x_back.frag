#version 130

smooth in vec4 inColor;
smooth in vec2 inPos;

uniform sampler2D text_0;

uniform float time;
uniform float var_0;
uniform float var_1;
uniform float var_2;
uniform float var_3;
uniform float var_4;

const int size_x = 1600; // 4800;
const int size_y = 900;  // 2700;

float grad( vec4 a, vec4 b ){
  return ( abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z) ) * 0.33;
}

float pixelate(float val, float step, float shift){
  return step * (int((val + step*shift) / step));
}

float mix( float a, float b, float coff){
  return a * coff + b * (1. - coff);
}

vec2 get_v( float r, float g, float b, float x, float y ){
  vec2 v;
  v.x = cos(r + b); // - 0.5 * cos( 3.14 * b * r + var_1) * sin(r * y);
  v.x = abs(v.x) * ( 0.5 - x ) * var_0;// * tan(3.14 * r * x * y);
  v.y = abs(v.x) * cos(3.14 * r * abs(x)*10 * tan( y + r ) );
  
  // v.x = pixelate(v.x, 0.1, 0);
  // v.y = pixelate(v.y, 0.1, 0);
  return v;
}

void main(){
  vec2 pos  = gl_TexCoord[0].st;
  vec4 c_o  = texture2D(text_0, pos);

  pos.x = pixelate(pos.x, 0.001 + 0.01*c_o.r, 0.5);
  pos.y = pixelate(pos.y, 0.001 + 0.01*abs(pos.x), 0.5);

  vec2 v = get_v(c_o.x, c_o.y, c_o.z, pos.x, pos.y);
  vec4 cv = texture2D(text_0, pos + vec2(v.x/size_x, v.y/size_y));

  float dr = 100*(cv.x + cv.y + cv.z);
  float dx = dr * cos(2*3.14 * cv.x) + dr * sin(3.14 * cv.y);
  float dy = dr * cos(3.14 * cv.y) - dr * sin(2*3.14 * cv.x);
  pos.x += dx/size_x;
  pos.y += dy/size_y;

  vec4 u = texture2D(text_0, pos + vec2(0 + var_0/size_x, 1./size_y + var_1/size_y));
  vec4 l = texture2D(text_0, pos + vec2(1./size_x + var_0/size_x, 0 + var_1/size_y));
  vec4 r = texture2D(text_0, pos + vec2(-1./size_x + var_0/size_x, 0 - var_1/size_y));
  vec4 d = texture2D(text_0, pos + vec2(0 - var_0/size_x, -1./size_y + var_1/size_y));

  float gu = grad(c_o, u);
  float gl = grad(c_o, l);
  float gr = grad(c_o, r);
  float gd = grad(c_o, d);

  float ga = ( gu + gl + gr + gd ) / 4.;

  vec4 p = 1*cv * (1.-0.12*ga + 0.2*ga*ga);

  float val_r = p.x * abs(1. + 0.01*abs(v.x) / (gu+0.01 + var_1-gd) );
  float val_g = p.y * abs(1. + 0.01*abs(v.x) - 0.0001*val_r * (gu+0.01 + var_1-gd) ) * tan(3.14*5*val_r);
  float val_b = p.z * abs(1. - 0.01*abs(v.x) * abs( 0.5 - pos.x ) ) * tan(3.14*2*val_g);
  val_r = 1.3*val_r * cos(3.14*6*val_b * 3.14*6*val_g);;

  val_b= abs(val_b);
  val_g= abs(val_g);
  val_r = abs(val_r);
  
    float dec = 4*( 0.5 - pos.x )*( 0.5 - pos.x );
    if( dec < 0.5 ) dec = 1;
    else{
      dec = 2. - dec*2; // 0-1
      dec = (dec + 5.1)/6.;
      //dec *= val_r*1.2 + cos(3.14 * val_r * 2);
    }

  val_r = min(max(val_r*dec,0),1);
  val_g = min(max(val_g*dec,0),1);
  val_b = min(max(val_b*dec,0),1);

  float fr = mix(c_o.x, val_r, 0.99*(1. - abs( 0.5 - pos.x )) );
  float fg = mix(c_o.y, val_g, 0.99*(1. - abs( 0.5 - pos.x )) );
  float fb = mix(c_o.z, val_b, 0.99*(1. - abs( 0.5 - pos.x )) );

  vec4 color = vec4(fr, fg, fb, 1.0);
  gl_FragColor = color;
}






