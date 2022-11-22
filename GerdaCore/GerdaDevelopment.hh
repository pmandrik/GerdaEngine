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
  //============================================================================================================= SLa shader control
  class ShaderStdin {
    /// class to update shaders variables from keyboard on the fly and dump shader states into console
    public:
    ShaderStdin( vector< shared_ptr<Shader> > shaders_ ){
      shaders_iter = 0;
      dir = 1;
      shader_prev = nullptr;
      shader_print_mode = "xml,cpp,vector";
      shaders = shaders_;
    }

    void ReloadShaders(){
      //for(auto shader : shaders) shader->Reload();      
    }

    void PrintShaders(){
      // for(auto shader : shaders) shader->Print( shader_print_mode );
    }
  
    void Tick(){
      if( shaders.size() == 0) return;
      if(sys::keyboard->Pressed(key::TAB) and sys::keyboard->Holded(key::LCTRL)) shaders_iter--;
      else if(sys::keyboard->Pressed(key::TAB)) shaders_iter++;
      if( shaders.size() <= shaders_iter ) shaders_iter = 0;
      if( shaders_iter < 0 ) shaders.size() - 1;
      shared_ptr<SLaShader> shader = std::dynamic_pointer_cast<SLaShader>( shaders.at( shaders_iter ) );
      if(not shader) MSG_WARNING(__PFN__, "NULL shader at index", shaders_iter);
      if(shader_prev != shader) MSG_INFO(__PFN__, ": switch to ", shader->name);
      if(sys::keyboard->Pressed(key::Q)) {dir *= -1; msg(dir);};
      float dvar = (0.1 + sys::keyboard->Holded(key::W) + 10*sys::keyboard->Holded(key::E)) * dir;
      if(sys::keyboard->Pressed(key::K_1)) { shader->vars[0] += dvar; msg(shader->vars[0]); }
      if(sys::keyboard->Pressed(key::K_2)) { shader->vars[1] += dvar; msg(shader->vars[1]); }
      if(sys::keyboard->Pressed(key::K_3)) { shader->vars[2] += dvar; msg(shader->vars[2]); }
      if(sys::keyboard->Pressed(key::K_4)) { shader->vars[3] += dvar; msg(shader->vars[3]); }
      if(sys::keyboard->Pressed(key::K_5)) { shader->vars[4] += dvar; msg(shader->vars[4]); }
      if(sys::keyboard->Pressed(key::K_6)) { shader->vars[5] += dvar; msg(shader->vars[5]); }
      if(sys::keyboard->Pressed(key::K_7)) { shader->vars[6] += dvar; msg(shader->vars[6]); }
      if(sys::keyboard->Pressed(key::K_8)) { shader->vars[7] += dvar; msg(shader->vars[7]); }
      if(sys::keyboard->Pressed(key::K_9)) { shader->vars[8] += dvar; msg(shader->vars[8]); }
      if(sys::keyboard->Pressed(key::K_0)) { shader->ResetVars(); }
      if(sys::keyboard->Pressed(key::SPACE)) PrintShaders();
      // if(sys::keyboard->Pressed(key::R))     ReloadShaders();
      shader_prev = shader;
    }
    int shaders_iter, dir;
    string shader_print_mode;
    vector< shared_ptr<Shader> > shaders;
    shared_ptr<Shader> shader_prev;
  };

};

#endif
