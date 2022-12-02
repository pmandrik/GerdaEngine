#ifndef GERDA_DEVELOPMENT_HH 
#define GERDA_DEVELOPMENTE_HH 1 

namespace ge{

  using namespace pm;
  using namespace std;
  using namespace tinyxml2;

  // ======= screenshoot ====================================================================
  string get_screnshoot_name(){
    /// function return sys::screnshoot_prefix_YYMMDD_HHMMSS_{0..999}.png
    time_t t = time(0);
    struct tm * now = localtime( & t );
    string postfix = to_string(now->tm_year - 100);
    postfix += normalize_lenght(to_string(now->tm_mon + 1), 2, "0");
    postfix += normalize_lenght(to_string(now->tm_mday), 2, "0") + "_"; 
    postfix += normalize_lenght(to_string(now->tm_hour), 2, "0");
    postfix += normalize_lenght(to_string(now->tm_min),  2, "0");
    postfix += normalize_lenght(to_string(now->tm_sec),  2, "0");
    if(now->tm_sec != sys::screenshoot_sec) sys::screenshoot_counter = 0;
    sys::screenshoot_sec = now->tm_sec;
    postfix += to_string(sys::screenshoot_counter);
    sys::screenshoot_counter++;
    if(sys::screenshoot_counter >= 999) sys::screenshoot_counter = 0;

    return sys::screnshoot_prefix + "_" + postfix + ".png";
  }

  //============================================================================================================= Development helpers classes
  class KeyAction {
    bool off = true;
    int key = -1;
    char kind;

    std::function< void( void ) > func;

    public:
    KeyAction( char kind_, int key_, std::function< void( void ) > func_) : kind(kind_), key(key_), func(func_) {
    }

    void Tick(){
      if( kind == 'S' ){
        if( sys::keyboard->Pressed( key ) ) off = not off;
        if( not off ) return;
        func();
      }
      if( kind == 'P' ){
        if( not sys::keyboard->Pressed( key ) ) return;
        func();
      }
    }
  };

  class DevelopmentHelper {
    vector<KeyAction> swithers;

    void AddSwitcher( char kind, int key, std::function< void( void ) > f ){
      swithers.push_back( KeyAction( kind, key, f) );
    }

    /// class for development interaction 
    /// update shaders variables from keyboard on the fly and dump shader states into console
    public:
    DevelopmentHelper( vector< shared_ptr<Shader> > shaders_ ){
      shaders_iter = 0;
      dir = 1;
      shader_prev = nullptr;
      shader_print_mode = "xml,cpp,vector";
      shaders = shaders_;

      AddSwitcher( 'P', key::SPACE, [&](void) { PrintShaders(); } );
      AddSwitcher( 'S', key::L, [&](void) { SaveShaderHistory(); } );
      AddSwitcher( 'P', key::K_0, [&](void){ shader_active->ResetVars(); } );
      AddSwitcher( 'P', key::Q, [&](void){ dir *= -1; msg(dir); } );
      AddSwitcher( 'P', key::R, [&](void){ ReloadShaders(); });
      for(int i = key::K_1; i <= key::K_9; i++){
        AddSwitcher( 'P', i, [&, i](void) { UpdateShaderVar( i -  key::K_1); } );
      }
      AddSwitcher( 'P', key::K_1, [&](void) { PrintShaders(); } );
      AddSwitcher( 'P', key::K_1, [&](void) { PrintShaders(); } );
    }

    float GetShaderDVar(){
      return (0.1 + sys::keyboard->Holded(key::W) + 10*sys::keyboard->Holded(key::E)) * dir;
    }

    void UpdateShaderVar( int id ){
      shader_active->vars[id] += GetShaderDVar();
      msg(shader_active->vars[id]);
      shader_updated = true;
    }

    void ReloadShaders(){
      // for(auto shader : shaders) shader->Reload();      
    }

    void PrintShaders(){
      // for(auto shader : shaders) shader->Print( shader_print_mode );
    }

    void SaveShaderHistory(){
      if( not shader_updated ) return;
      shader_updated = false;
    }

    void DrawTrack( bool draw ){
      if( not draw ){
        if( draw ){
        } 
        return;
      }
      /// add points to track time to time
      // start new track
      // Track
      // add point new track
      sys::mouse;

      
    }
  
    void Tick(){
      /// Get input from mouse drawer
      

      /// Update variable in Shaders from keyboard
      if( shaders.size() == 0) return;
      if(sys::keyboard->Pressed(key::TAB) and sys::keyboard->Holded(key::LCTRL)) shaders_iter--;
      else if(sys::keyboard->Pressed(key::TAB)) shaders_iter++;
      if( shaders.size() <= shaders_iter ) shaders_iter = 0;
      if( shaders_iter < 0 ) shaders.size() - 1;
      shader_active = std::dynamic_pointer_cast<SLaShader>( shaders.at( shaders_iter ) );
      if(not shader_active) MSG_WARNING(__PFN__, "NULL shader at index", shaders_iter);
      if(shader_prev != shader_active) MSG_INFO(__PFN__, ": switch to ", shader_active->name);

      for(auto swither : swithers) swither.Tick();

      shader_prev = shader_active;

      msg(shader_active->vars[0]);
    }

    bool shader_updated = false;
    int shaders_iter, dir;
    string shader_print_mode;
    vector< shared_ptr<Shader> > shaders;
    shared_ptr<SLaShader> shader_active, shader_prev;
    Carta dev_carta;
  };

};

#endif
