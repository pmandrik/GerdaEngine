// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#ifndef GERDA_CORE_HH 
#define GERDA_CORE_HH 1

#define __PFN__ __PRETTY_FUNCTION__

namespace ge {

  using namespace pm;
  using namespace std;
  using namespace tinyxml2;

  class Mouse;
  class Keyboard;
  class Core;
  class Image;
  class Camera;
  class FileInput;

  namespace sys{
    // ======= SYSTEM CFG ====================================================================
    // SCREEN
    float z_buffer = 100;
    float z_def_value = 0;

    bool FULLSCREEN = false;
    int fps = 60;
    int WW = 800; int WH = 600;
    int WW2 = WW / 2; int WH2 = WH / 2;
    v2 WV  = v2(WW,  WH ); v2 WV2 = v2(WW2, WH2);   
    void set_windows_size(int ww, int wh){
      WW = ww;
      WH = wh;
      WW2 = WW / 2;
      WH2 = WH / 2;
      WV  = v2(WW,  WH );
    }

    int SW, SW2, SH, SH2;
    int FBW, FBW2, FBH, FBH2;
    v2 FBV, FBV2;   
    v2 SV, SV2;
    float STW_scale;
    void set_screen_size(int sw, int sh){
      SW  = sw;
      SH  = sh;
      SW2 = SW/2;
      SH2 = SH/2;
      SV  = v2(SW, SH);
      SV2 = v2(SW2, SH2);

      STW_scale = float(SH)/WH;
      FBW  = WW * STW_scale;
      FBH  = WH * STW_scale;
      FBW2 = FBW / 2;
      FBH2 = FBH / 2;
      FBV = v2(FBW, FBH);
      FBV2 = v2(FBW2, FBH2);

      msg(SW, SW2, SH, SH2);
      msg(" FBW, FBW2, FBH, FBH2", FBW, FBW2, FBH, FBH2);
    }

    // HLT related
    v2 hide_position = v2(999999, 999999);

    // OTHER
    int def_verbose_lvl = verbose::VERBOSE;
    int verbose_lvl     = verbose::VERBOSE;

    rgb fb_def_color;
    rgb alpha_color_1 = rgb(255,0,0); 
    rgb alpha_color_2 = rgb(0,255,0);     
    rgb alpha_color_3 = rgb(0,0,255);     
    rgb alpha_color_4 = rgb(-1,-1,-1);
    bool is_rgb_alpha_mask( int r, int g, int b ){ 
      return sys::alpha_color_1.Equal_i(r,g,b) or sys::alpha_color_2.Equal_i(r,g,b) or sys::alpha_color_3.Equal_i(r,g,b) or sys::alpha_color_4.Equal_i(r,g,b);
    }

    int     screenshoot_counter = 0;
    int     screenshoot_sec = 0;
    Image * screenshoot_image;
    Timer * screenshoot_timer;
    string * screnshoot_prefix = new string("gs_");

    // ======= SHADERS
    string default_shader_vert_path = "Data/default_shaders/shader_FB_def.vert";
    string default_shader_frag_path = "Data/default_shaders/shader_FB_def.frag";

    // ======= LANGUAGE SUPPORT 
    // ENG
    const string ABC_en = string("QWERTYUIOPLKJHGFDSAZXCVBNM");
    const string abc_en = string("qwertyuiopasdfghjklzxcvbnm");

    // RUS
    const string ABC_ru = string("ЙЦУКЕНГШЩЗХЪЭЖДЛОРПАВЫФЯЧСМИТЬБЮЁ");
    const string abc_ru = string("йцукенгшщзхъфывапролджэячсмитьбюё");

    // SYMBOLS
    const string abc_numbers = string("1234567890");
    const string abc_symbols = string(" !@#$%^*()_+-=[];\',./{}:\"<>?");

    // ======= CORE ====================================================================
    // We always have 1 mouse, 1 keyboard, 1 core
    Mouse       * mouse;
    Keyboard    * keyboard;
    Core        * core;
    Camera      * camera;
    FileInput   * file_input;
  };
  
  // ======= MOUSE ====================================================================
  class Mouse {
    public:
      void Tick(){
        left_up   = false;
        right_up  = false;
        middle_up = false;
        left_down   = false;
        right_down  = false;
        middle_down = false;
        wheel = 0;
      }

      int x, y, wheel;
      bool left_hold, right_hold, middle_hold;
      bool left_down, right_down, middle_down;
      bool left_up,   right_up,   middle_up;
  };

  ostream & operator << (ostream & out, const Mouse & m){
    cout << "ge::Mouse pos(" << m.x << ", " << m.y << ") ";
    cout << "hold(" << m.left_hold << ", " << m.right_hold << ", " << m.middle_hold << ") ";
    cout << "up(" << m.left_up << ", " << m.right_up << ", " << m.middle_up << ") ";
    cout << "down(" << m.left_down << ", " << m.right_down << ", " << m.middle_down << ") wheel(" << m.wheel << ")";
    return out;
  };

  // ======= Base Class ====================================================================
  class BaseClass{
    public:
      BaseClass(){ verbose_lvl = sys::def_verbose_lvl; }
      int verbose_lvl;
      std::string name;
  };

  // ======= KEYBOARD ====================================================================
  class Keyboard {
    public:
      Keyboard(){
        hor = 0; ver = 0;
        actz = false; actx = false; exit = false; screenshoot = false; hor_pressed = false; ver_pressed = false;
        for(int key = 0; key < 232; key++){
          keyboard_pressed[key] = 0;
          keyboard_holded[key]  = 0;
          keyboard_raised[key]  = 0;
        }
      }

      void Print(){
        msg(__PFN__);
        msg("hor, ver, actz, actx, hor_pressed, ver_pressed = ", hor, ver, actz, actx, hor_pressed, ver_pressed );
        msg("exit, screenshoot = ", exit, screenshoot );
      }

      void Tick(){
        // update keyboard in the implementation
        // some regular event - this should be configuradable ...
        hor = check_sign( Holded(key::D) + Holded(key::RIGHT) - Holded(key::A) - Holded(key::LEFT));
        ver = check_sign( Holded(key::W) + Holded(key::UP) - Holded(key::S) - Holded(key::DOWN));
        actz = Pressed(key::Z) + Pressed(key::K) + Pressed(key::RETURN) + Pressed(key::SPACE);
        actx = Pressed(key::X) + Pressed(key::L);
        hor_pressed = check_sign( Pressed(key::D) + Pressed(key::RIGHT) - Pressed(key::A) - Pressed(key::LEFT)  );
        ver_pressed = check_sign( Pressed(key::W) + Pressed(key::UP) - Pressed(key::S) - Pressed(key::DOWN));
        exit = Pressed(key::ESCAPE);
        screenshoot = Holded(key::KP_0);
      }

      inline bool Pressed(int key){ return keyboard_pressed[key]; }
      inline bool Holded(int key) { return keyboard_holded[key];  }
      inline bool Raised(int key) { return keyboard_raised[key];  }

      int hor, ver;
      bool actz, actx, exit, screenshoot, hor_pressed, ver_pressed;
      bool keyboard_pressed[232], keyboard_holded[232], keyboard_raised[232];
  };

  // ======= File Input ====================================================================
  class ReadTxtFileImp {
    public:
    virtual string read_text_files(const string & path) = 0;
  };

  class FileInput {
    public:
    std::shared_ptr<ReadTxtFileImp> read_text_files_imp;

    string read_text_files(const string & path){
      return read_text_files_imp->read_text_files( path );
    };
  };
  

  // ======= Core ====================================================================
  class Core : public BaseClass {
    public:
    virtual void Tick(){ msg("do nothing"); }
  };

}

#include "GerdaXML.hh"
#include "GerdaDataContainer.hh"
#include "GerdaImage.hh"
#include "GerdaDraw.hh"
#include "GerdaSLa.hh"
#include "GerdaConfig.hh"

#endif







