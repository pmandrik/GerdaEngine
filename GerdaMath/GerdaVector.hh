// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#ifndef GerdaVector_H
#define GerdaVector_H 1

#include "GerdaMathConst.hh"

using namespace std;

namespace ge {
  // ======= v2 ====================================================================
  class v2 {
    public:
    v2(float xx = 0., float yy = 0., float zz = 0.):x(xx),y(yy),z(zz){};
    v2(ldiv_t val):x((float)val.rem),y((float)val.quot),z(0){}; // ldiv()
    inline v2 rotate(float angle){return  v2( x * cos(angle) - y * sin(angle), x * sin(angle) + y * cos(angle));}
    inline v2 rotated(float angle){return rotate(angle * RADDIG);}
    inline float l(){return sqrt(x*x + y*y);}
    inline float l2(){return x*x + y*y;}
    inline float angle(){return (y > 0 ? acos(x / (l()+OSML) ) : PI - acos(x / (l()+OSML) ));}
    inline float angled(){return DIGRAD * (y > 0 ? acos(x / (l()+OSML) ) : 2*PI - acos(x / (l()+OSML) ));}
    inline v2 set(const v2 & v){x = v.x; y = v.y; return *this;}
    inline v2 apply_sign(){x = ge::check_sign(x); y = ge::check_sign(y); return *this;};

    float x, y, z;

    v2 & operator *= (float value){x *= value; y *= value; return *this;}
    v2 & operator /= (float value){x /= value; y /= value; return *this;}
    v2 & operator += (const v2 & v){x += v.x; y += v.y; return *this;}
    v2 & operator -= (const v2 & v){x -= v.x; y -= v.y; return *this;}
    v2 & operator *= (const v2 & v){x *= v.x; y *= v.y; return *this;}
    v2 & operator /= (const v2 & v){x /= v.x; y /= v.y; return *this;}
    v2 operator - () const {return v2(-x, -y);}
  };

  v2 operator + (v2 va, v2 vb){return v2(va.x + vb.x, va.y + vb.y);}
  v2 operator - (v2 va, v2 vb){return v2(va.x - vb.x, va.y - vb.y);}
  v2 operator * (float value, v2 v){return v2(v.x*value, v.y*value);}
  v2 operator * (v2 v, float value){return v2(v.x*value, v.y*value);}
  v2 operator * (v2 va, v2 vb){return v2(va.x * vb.x, va.y * vb.y);}
  v2 operator / (v2 v, float value){return v2(v.x/value, v.y/value);}
  v2 operator / (v2 va, v2 vb){return v2(va.x / vb.x, va.y / vb.y);}

  bool operator == (v2 va, v2 vb){return ((va.x == vb.x) and (va.y == vb.y));}
  bool operator != (v2 va, v2 vb){return ((va.x != vb.x) or (va.y != vb.y));}

  ostream & operator << (ostream & out, v2 v){return out << " v2( " << v.x << ", " << v.y << " ) ";};

  // ======= v3 ====================================================================
  class v3 {
    public:
    v3(float xx = 0., float yy = 0., float zz = 0.):x(xx),y(yy),z(zz){};
    v3(v2 v):x(v.x),y(v.y),z(v.z){};
    inline v3 rotate(float angle, int axis){
      if(axis==0){
        v2 yz = v2(y, z).rotate(angle);
        return v3(x, yz.x, yz.y);
      }
      if(axis==1){
        v2 xz = v2(x, z).rotate(angle);
        return v3(xz.x, y, xz.y);
      }
      if(axis==2){
        v2 xy = v2(x, y).rotate(angle);
        return v3(xy.x, xy.y, z);
      }
      return *this;
    }
    inline v3 rotated(const float & angle, int axis){return rotate(angle * RADDIG, axis);}
    inline v3 rotated(const float & angle_x, const float & angle_y, const float & angle_z){
      return rotate(angle_x * RADDIG, 0).rotate(angle_y * RADDIG, 1).rotate(angle_z * RADDIG, 2);
    }

    inline v3 perspective_scale(const v3 & start_pos, const float & end_plane_x, const float & end_plane_y){
      return v3(start_pos.x - (z - end_plane_x) / (start_pos.z - end_plane_x) * (start_pos.x - x), 
                start_pos.y - (z - end_plane_y) / (start_pos.z - end_plane_y) * (start_pos.y - y), 
                z);
    }

    inline float l(){return sqrt(x*x + y*y + z*z);}
    inline float l2(){return x*x + y*y + z*z;}

    float x, y, z;

    v3 & operator *= (float value){x *= value; y *= value; z *= value; return *this;}
    v3 & operator /= (float value){x /= value; y /= value; z /= value; return *this;}
    v3 & operator += (const v3 & v){x += v.x; y += v.y; z += v.z; return *this;}
    v3 & operator -= (const v3 & v){x -= v.x; y -= v.y; z -= v.z; return *this;}
    v3 & operator *= (const v3 & v){x *= v.x; y *= v.y; z *= v.z; return *this;}
    v3 & operator /= (const v3 & v){x /= v.x; y /= v.y; z /= v.z; return *this;}
    v3 operator - () const {return v3(-x, -y, -z);}
  };

  v3 operator + (v3 va, v3 vb){return v3(va.x + vb.x, va.y + vb.y, va.z + vb.z);}
  v3 operator - (v3 va, v3 vb){return v3(va.x - vb.x, va.y - vb.y, va.z - vb.z);}
  v3 operator * (float value, v3 v){return v3(v.x*value, v.y*value, v.z*value);}
  v3 operator * (v3 v, float value){return v3(v.x*value, v.y*value, v.z*value);}
  v3 operator * (v3 va, v3 vb){return v3(va.x * vb.x, va.y * vb.y, va.z * vb.z);}
  v3 operator / (v3 v, float value){return v3(v.x/value, v.y/value, v.z/value);}
  v3 operator / (v3 va, v3 vb){return v3(va.x / vb.x, va.y / vb.y, va.z / vb.z);}

  bool operator == (v3 va, v3 vb){return ((va.x == vb.x) and (va.y == vb.y) and (va.z == vb.z));}
  bool operator != (v3 va, v3 vb){return ((va.x != vb.x) or (va.y != vb.y) or (va.z != vb.z));}

  ostream & operator << (ostream & out, v3 v){return out << "v3(" << v.x << "," << v.y << "," << v.z << ")";};

  // ======= rgb ====================================================================
  class rgb {
    public:
    rgb(float xx = 0., float yy = 0., float zz = 0., float aa = 1.):r(xx),g(yy),b(zz),a(aa){};
    float r, g, b, a;

    rgb & operator *= (float value){r *= value; g *= value; b *= value; return *this;}
    rgb & operator /= (float value){r /= value; g /= value; b /= value; return *this;}
    rgb & operator += (const rgb & v){r += v.r; g += v.g; b += v.b; return *this;}
    rgb & operator -= (const rgb & v){r -= v.r; g -= v.g; b -= v.b; return *this;}
    rgb & operator *= (const rgb & v){r *= v.r; g *= v.g; b *= v.b; return *this;}
    rgb & operator /= (const rgb & v){r /= v.r; g /= v.g; b /= v.b; return *this;}
    rgb operator - () const {return rgb(-r, -g, -b);}

    rgb norm_255i(){ return rgb(int(r*255), int(g*255), int(b*255), int(a*255) ); }
    rgb norm_255f(){ return rgb(r*255, g*255, b*255, a*255 ); }
    rgb norm_1f()  { return rgb(r/255., g/255., b/255., a/255. ); }
  };

  rgb operator + (rgb va, rgb vb){return rgb(va.r + vb.r, va.g + vb.g, va.b + vb.b);}
  rgb operator - (rgb va, rgb vb){return rgb(va.r - vb.r, va.g - vb.g, va.b + vb.b);}
  rgb operator * (float value, rgb v){return rgb(v.r*value, v.g*value, v.b*value);}
  rgb operator * (rgb v, float value){return rgb(v.r*value, v.g*value, v.b*value);}
  rgb operator * (rgb va, rgb vb){return rgb(va.r * vb.r, va.g * vb.g, va.b * vb.b);}
  rgb operator / (rgb v, float value){return rgb(v.r/value, v.g/value, v.b/value);}
  rgb operator / (rgb va, rgb vb){return rgb(va.r / vb.r, va.g / vb.g, va.b / vb.b);}

  ostream & operator << (ostream & out, rgb v){return out << " rgb( " << v.r << ", " << v.g << ", " << v.b << ", " << v.a << " ) ";};
}

#endif
