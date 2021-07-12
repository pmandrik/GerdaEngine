
#ifndef GERDA_OTHER
#define GERDA_OTHER_HH 1

namespace ge {

  // ======= Timer ====================================================================
  class Timer{
    public:
    Timer(){};
    Timer(int max_time) : mtime(max_time) { dtime = 1./float(mtime); Reset(); }
    inline void Set(int time){itime = time; ftime = itime * dtime;}
    inline void End(){Set(mtime);}
    inline void Reset(){ Set(0); }
    inline void ResetBack(){ Set(mtime-1); }
    bool Tick(int val=1){ 
      itime += val; ftime += dtime*val;
      if(itime >= mtime){ Reset(); return true; }
      if(itime < 0){ ResetBack(); return true; }
      return false; 
    }
    bool TickIf(int val=1){
      if(itime) return Tick(val);
      return true;
    }

    int mtime, itime;
    float dtime, ftime;
  };

  // ======= Clocker ====================================================================
  class Clocker{
    public:
    Clocker(int fps){ delay = 1000/fps; ntime = SDL_GetTicks(); };
    float Tick(void){ // FIXME time override
      if(delay > SDL_GetTicks() - ntime) SDL_Delay( delay - SDL_GetTicks() + ntime );
      answer = SDL_GetTicks() - ntime;
      ntime = SDL_GetTicks();
      return 1./answer;
    }
    Uint32 delay, ntime, dtime, answer;
  };

  // ======= Base Class ====================================================================
  class BaseClass{
    public:
      BaseClass(){ verbose_lvl = sys::def_verbose_lvl; }
      int verbose_lvl;
  };

  // ======= string manipulation algos ====================================================================
  vector<string> * split_string(string str, const string & sep = " "){
    int sep_size = sep.size();
    vector<string> * answer = new vector<string>();

    for(int i = 0; i + sep_size < str.size(); i++){
      if( sep == str.substr(i, sep_size) ){
        answer->push_back( str.substr(0, i) ); 
        str = str.substr(i + sep_size, str.size() - i - sep_size);
        i = 0;
      }
    }

    answer->push_back(str);
    return answer;
  }

  string normalize_lenght(string s, const int & lenght, const string & symbol){ // add 'symbol'
    while(s.size() < lenght) s = symbol + s;
    return s;
  }

  // utf8 & strings ====================================================================
  string utf8_to_string(const uint32_t & code){
    string result;
    utf8::append(code, back_inserter(result));
    return result;
  }

  string utf8_to_string(const vector<uint32_t> & str){
    string result;
    utf8::utf32to8(result.begin(), result.end(), back_inserter(result));
    return result;
  }

  vector<uint32_t> string_to_utf8(const string & str){
    auto beg_it = str.begin();
    auto end_it = utf8::find_invalid(beg_it, str.end());
    int length = utf8::distance(beg_it, end_it);
    vector<uint32_t> answer;
    for(int i = 0; i < length; i++)
      answer.push_back( utf8::next(beg_it, end_it) );
    return answer;
  }

  vector<string> string_to_utf8_string(const string & str){
    const vector<uint32_t> & codes = string_to_utf8(str);
    vector<string> answer;
    for(auto code : codes) answer.push_back( to_string(code) );
    return answer;
  }

  string char_to_utf8_string(const char symbol){
    string nstring = " ";
    nstring[0] = symbol;
    const vector<uint32_t> & codes = string_to_utf8(nstring);
    return to_string(codes.at(0));
  }

  // ======= screenshoot ====================================================================
  string get_screnshoot_name(){
    time_t t = time(0);
    struct tm * now = localtime( & t );
    string postfix = to_string(now->tm_year - 100);
    postfix += normalize_lenght(to_string(now->tm_mon + 1), 2, "0");
    postfix += normalize_lenght(to_string(now->tm_mday), 2, "0") + "_"; 
    postfix += normalize_lenght(to_string(now->tm_hour), 2, "0");
    postfix += normalize_lenght(to_string(now->tm_min),  2, "0");
    postfix += normalize_lenght(to_string(now->tm_sec),  2, "0");
    if(now->tm_sec != sys::screenshoot_sec) sys::screenshoot_counter = 100;
    sys::screenshoot_sec = now->tm_sec;
    postfix += to_string(sys::screenshoot_counter);
    sys::screenshoot_counter++;
    if(sys::screenshoot_counter >= 995) sys::screenshoot_counter = 100;

    return *(sys::screnshoot_prefix) + "_" + postfix + ".png";
  }

  // ======= rect ====================================================================
  struct Rect{
    /// rectangular box
    Rect(float x_min_, float y_min_, float x_max_, float y_max_){
      x_min = x_min_; x_max = x_max_; y_min = y_min_; y_max = y_max_;
    }
    bool Contain(const v2 & pos){ return check_in(pos.x, x_min, x_max) and check_in(pos.y, y_min, y_max); }
    float x_min, x_max, y_min, y_max;
  };
}

#endif 















