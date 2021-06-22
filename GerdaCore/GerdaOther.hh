
#ifndef GERDA_OTHER
#define GERDA_OTHER_HH 1

namespace ge {

  // ======= Timer ====================================================================
  class Timer{
    public:
    Timer(){};
    Timer(int max_time) : mtime(max_time) { dtime = 1./float(mtime); Reset(); }
    inline void Set(int time){itime = time; ftime = itime * dtime;}
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
      BaseClass(){
        verbose_lvl = sys::def_verbose_lvl;
      }
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
}

#endif 















