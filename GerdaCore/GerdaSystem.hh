// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GERDA_SYSTEM_HH 
#define GERDA_SYSTEM_HH 1

namespace ge {

  class EventSystem;
  class Clocker;
  class Camera;
  class FrameBuffer;
  class Image;
  class Timer;
  class DataContainer;

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

    // ======= SOME GLOBAL SYSTEMS ====================================================================
    SDL_Window * window;
    SDL_GLContext glcontext;

    rgb fb_def_color;
    FrameBuffer * screen_final_frame; // Double buffer?

    Clocker     * clocker;
    Mouse       * mouse;
    Keyboard    * keyboard;
    EventSystem * event_sys;
    DataContainer * data_sys;

    int     screenshoot_counter = 0;
    int     screenshoot_sec = 0;
    Image * screenshoot_image;
    Timer * screenshoot_timer;
    string * screnshoot_prefix = new string("gs_");

    Camera * camera;

    // ======= SYSTEMS CONSTANTS ====================================================================
    // IO Image
    rgb alpha_color_1 = rgb(255,0,0); 
    rgb alpha_color_2 = rgb(0,255,0);     
    rgb alpha_color_3 = rgb(-1,-1,-1);

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
  }
  
};

#endif















