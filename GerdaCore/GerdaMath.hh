// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#ifndef GerdaMath_H
#define GerdaMath_H 1

namespace ge {

  using namespace pm;
  using namespace std;
  using namespace tinyxml2;

  // ======= MATH CONSTS ====================================================================
  const float PI = 3.14159265358979323846;
  const float PI2 = 3.14159265358979323846/2;
  const float OSML = 0.000001;
  const float RADDIG = PI/180.;
  const float DIGRAD = 180./PI;

  // ======= RANDOM ENGINE ====================================================================
  static default_random_engine rengine;

  int randint(int start, int end){
    uniform_int_distribution<int> distribution(start, end);
    return distribution(rengine);
  };

  int randrange(int start, int end, int step = 1){
    uniform_int_distribution<int> distribution(0, (end-start)/step);
    return start+distribution(rengine)*step;
  };

  int randone(){ return randrange(-1, 2, 2);};

  float uniform(float start, float end){
    uniform_real_distribution<float> distribution(start,end);
    return distribution(rengine);
  };

  v2 randv2(int start, int end){
    return v2(randint(start, end), randint(start, end));
  };

  v2 randv2(int startX, int endX, int startY, int endY){
    return v2(randint(startX, endX), randint(startY, endY));
  };

  // ======= DIFFERENT ALGOS & FUNCTIONS ====================================================================
  template <class T> int check_sign(T a){ if(a>0.){return 1;} if(a<0.){return -1;} return 0; }
  template <class T> bool check_in(T a, T min, T max){ return (a<min or a>max) ? false : true;}

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

  //------------------------------------------------------------------ GEOMETRICAL ABSRACTS -----------------------------------------------------------------
  struct Rect { // TODO Resize, rotate?
    /// rectangular box
    Rect(float x_min_, float y_min_, float x_max_, float y_max_){
      x_min = x_min_; x_max = x_max_; y_min = y_min_; y_max = y_max_;
    }
    bool Contain(const v2 & pos){ return check_in(pos.x, x_min, x_max) and check_in(pos.y, y_min, y_max); }
    float x_min, x_max, y_min, y_max;
  };

  struct Point {
    /// x, y + string metadata
    Point(const v2 & pos_){ pos = pos_; }
    Point(const v2 & pos_, const vector<string> & meta_){
      pos = pos_;
      meta = meta_;
    }
    v2 pos;
    vector<string> meta;
  };

  struct Track {
    /// Track is a vector of 2d points
    Track(){}
    
    public:
    vector<v2> points;
    float lenght = -1;

    float GetLenght(){
      float L = 0;
      for(int i = 1; i < points.size(); ++i)
        L += (points[i] - points[i-1]).L();
      return L;
    }
    
    void Clear(){
      points.clear();
    }
    
    void Pop(){
      if( not points.size() ) return;
      points.pop_back();
    }
    
    void Add( v2 point ){
      points.push_back( point );
    }
    
    Track GetSlimmed( char strategy = 'I', float factor = 1. ){
      /// Get a copy of vector with reduced number of points
      /// strategy = 'I' -> keep every int(factor) point
      /// strategy = 'D' -> remove points with distance < factor
      Track answer;
      if( not points.size() ) return answer;
      if( strategy == 'I' ){
        int factori = int( factor + 0.5 );
        for(int i = 0, i_max = points.size(); i < i_max; i += factori ){
          answer.Add( points[i] );
        }
      } else if( strategy == 'D' ){
        answer.Add( points[0] );
        float factor2 = factor * factor;
        for(int i = 1, i_max = points.size(); i < i_max; ++i ){
          if( (points[i] - points[i-1]).L2() < factor2 ) continue;
          answer.Add( points[i] );
        }
      }
      return answer;
    }

    v2 GetPos(const float & step){
      if( not points.size() ) return v2();
      if( lenght < 0 ) lenght = GetLenght();
      if( points.size() == 1 or lenght <= 0) return points.at(0);

      float L = 0, dL = 0;
      int N_step = 1;
      for(; N_step < points.size(); ++N_step){
        dL = (points[N_step] - points[N_step-1]).L();
        if( step <= (L+dL) / lenght ) break;
        L += dL;
      }

      float frac = (step - L/lenght);
      return interpolation_linear( points[N_step-1], points[N_step], frac );
    }
  };

  class Carta {
    /// Carta is a union of Points and Tracks
    public:
    map<string, vector<Point> > coordinates;
    map<string, vector<Track> > tracks;

    void AddCoodinate(const string & id, const v2 & pos){
      auto it = coordinates.find(id);
      if( it != coordinates.end() ) it->second.emplace_back( Point(pos) );
      else coordinates[id] = { Point(pos) };
    }

    void AddCoodinate(const string & id, const v2 & pos, const vector<string> & meta){
      auto it = coordinates.find(id);
      if( it != coordinates.end() ) it->second.emplace_back( Point(pos, meta) );
      else coordinates[id] = { Point(pos, meta) };
    }
  };

};
#endif
