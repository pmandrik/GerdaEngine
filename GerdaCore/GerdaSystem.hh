// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GERDA_SYSTEM_HH 
#define GERDA_SYSTEM_HH 1

namespace ge {

  class EventParser;
  class Clocker;
  class Camera;
  class FrameBuffer;
  class Image;
  class Timer;

  namespace sys{
    // ======= SYSTEM CFG ====================================================================
    // SCREEN
    const bool FULLSCREEN = false;
    const int fps = 60;
    const int WW = 800; const int WH = 600;
    // const int WW = 2*640; const int WH = 2*480;
    const int WW2 = WW / 2;
    const int WH2 = WH / 2;
    const v2 WV  = v2(WW,  WH );
    const v2 WV2 = v2(WW2, WH2);
    const float z_buffer = 100;

    int SW, SW2, SH, SH2;
    int FBW, FBW2, FBH, FBH2;
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
    EventParser * event_parser;

    int     screenshoot_counter = 0;
    int     screenshoot_sec = 0;
    Image * screenshoot_image;
    Timer * screenshoot_timer;
    string * screnshoot_prefix = new string("gs_");

    Camera * camera;
  }
  
};

#endif















