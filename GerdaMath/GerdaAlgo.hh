// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#ifndef GerdaAlgo_H
#define GerdaAlgo_H 1

namespace ge {
  //------------------------------------------------------------------ INTERPOLATION ------------------------------------------------------------------
  template<class A> A interpolation_linear(const A & p1, const A & p2, const float & frac){
    return p1 + (p2 - p1) * frac;
  }

  template<class A> A interpolation_sin(const A & p1, const A & p2, const float & frac){
    return p1 + (p2 - p1) * sin(PI2 * frac);
  }

  template<class A> A interpolation_pow2(const A & p1, const A & p2, const float & frac, const int pow_n = 2){
    return p1 + (p2 - p1)  * pow(frac, pow_n);
  }

  struct VecInTime {
    vector<float> vals;
    float time;
    VecInTime(float time_, vector<float> & vals_){
      vals = vals_;
      time = time_;
    }
  };

  static int get_treshold_(const vector<VecInTime> & points, const float & time){
    int treshold = 0;
    for(;treshold < points.size(); treshold++)
      if( time < points[treshold].time ) break;
    return treshold;
  };

  vector<float> interpolation_algo(const vector<VecInTime> & points, float time, const string & algo){
    if( not points.size() ) return vector<float>();
    if( not points[0].vals.size() ) return vector<float>();
    if( points.size() == 1 ) return points[0].vals;

    //

    // basic algos without extrapolations
      if( time < points[0].time ) return points[0].vals;
      if( time > points[points.size()-1].time ) return points[points.size()-1].vals;
      int treshold = get_treshold_(points, time);
      const VecInTime & p1 = points.at(treshold-1);
      const VecInTime & p2 = points.at(treshold);
      time = (time - p1.time) / (abs(p2.time - p1.time) + OSML);
      vector<float> answer = vector<float>( p1.vals.size() );

      if( algo == "linear" )
        for(int i = 0, imax=p1.vals.size(); i < imax; ++i)
          answer[i] = interpolation_linear(p1.vals[i], p2.vals[i], time);
      if( algo == "sin" )
        for(int i = 0, imax=p1.vals.size(); i < imax; ++i)
          answer[i] = interpolation_sin(p1.vals[i], p2.vals[i], time);
      if( algo.substr(0,3) == "pow" ){
        string val = algo.substr(3,algo.size()-3);
        char * pEnd;
        int pow_n = max( (long int)1L, strtol(val.c_str(), &pEnd, 10) );  
        for(int i = 0, imax=p1.vals.size(); i < imax; ++i)
          answer[i] = interpolation_pow2(p1.vals[i], p2.vals[i], time, pow_n);
      }

      return answer;
  };

  //------------------------------------------------------------------ INTERSECTIONS ------------------------------------------------------------------
  bool is_point_in_box(const v2 & point, const v2 & box_center, const v2 & box_size){
    // dV.x = x_t - x_0 = A * x_1 + B * x_2
    // dV.y = y_t - y_0 = A * y_1 + B * y_2
    // A = dV.x/x_1 - B*x_2/x_1 = dV.y/y_1 - B*y_2/y_1
    // (x_1*y_2-y_1*x_2)*B = x_1*dV.y - y_1*dV.x
    v2 dV    = point - box_center;
    v2 sizer = v2(box_size.x, 0); // x_1
    v2 perpr = v2(0, box_size.y); // x_2

    float B = (sizer.x * dV.y - sizer.y * dV.x) / (sizer.x * perpr.y - sizer.y * perpr.x);
    if(abs(B) > 1) return false;
    float A;
    if( abs(sizer.x) > abs(sizer.y) ) A = (dV.x - B * perpr.x)/sizer.x;
    else                              A = (dV.y - B * perpr.y)/sizer.y;
    if(abs(A) > 1) return false;
    return true;
  }
  
  bool is_point_in_box(const v2 & point, const v2 & box_center, const v2 & box_size, const float & angle){
    // dV.x = x_t - x_0 = A * x_1 + B * x_2
    // dV.y = y_t - y_0 = A * y_1 + B * y_2
    // A = dV.x/x_1 - B*x_2/x_1 = dV.y/y_1 - B*y_2/y_1
    // (x_1*y_2-y_1*x_2)*B = x_1*dV.y - y_1*dV.x
    v2 dV    = point - box_center;
    v2 sizer = v2(box_size.x, 0).Rotated(angle); // x_1
    v2 perpr = v2(0, box_size.y).Rotated(angle); // x_2

    float B = (sizer.x * dV.y - sizer.y * dV.x) / (sizer.x * perpr.y - sizer.y * perpr.x);
    if(abs(B) > 1) return false;
    float A;
    if( abs(sizer.x) > abs(sizer.y) ) A = (dV.x - B * perpr.x)/sizer.x;
    else                              A = (dV.y - B * perpr.y)/sizer.y;
    if(abs(A) > 1) return false;
    return true;
  }

  //------------------------------------------------------------------ ANGLE MANIPULATION ------------------------------------------------------------------
  void angle_norm(float & angle){
    while(angle < 0) angle += 360.f;
    while(angle > 360.f) angle -= 360.f;
  }

  float angle_disstance(const float & a1, const float & a2){
    float d1 = max(a1, a2) - min(a1, a2);
    return min(d1, 360.f - d1);
  }

  float angle_interpolate(float angle, float target_angle, const float & frac){
    angle_norm(angle);
    angle_norm(target_angle);
    float d1 = max(angle, target_angle) - min(angle, target_angle);
    float d2 = 360.f - d1;
    if(d1 < d2) angle = angle + frac * (target_angle - angle);
    else        angle = angle - frac * d2 * check_sign(target_angle - angle);
    angle_norm(angle);
    return angle;
  }

};

#endif 
