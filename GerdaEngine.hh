// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#ifndef GerdaEngine_H
#define GerdaEngine_H 1

// ======= SDL ====================================================================
#include "SDL.h"
#include "SDL_opengl.h"

// ======= OPENGL =================================================================

// ======= STL ====================================================================
#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <string>
#include <random>
#include <math.h>
#include <stdlib.h>
#include <ctime>
#include <functional>
using namespace std;

// ======= PMANDRIK_LIBRARY ============================================================
#include "pmlib_v2d.hh"
#include "pmlib_msg.hh"
using namespace pm;

// ======= EXTERNAL ====================================================================
#include "tinyxml2.h"
using namespace tinyxml2;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "utf8.h"

// ======= GERDA ====================================================================
#include "GerdaMath.hh"
#include "GerdaCore.hh"
#include "GerdaOpenGL.hh"
#include "GerdaHLT.hh"

namespace ge {

  bool init_sdl(){
    msg("init_sdl() ... ");
    SDL_Init(SDL_INIT_VIDEO);
    msg("SDL_GetPlatform = ", SDL_GetPlatform());
    msg("SDL_GetNumVideoDisplays = ", SDL_GetNumVideoDisplays());
  }

  bool init_sdl_screen(){
    SDL_DisplayMode desktop_mode;
    bool not_ok = SDL_GetDesktopDisplayMode(0, &desktop_mode);

    if(not_ok){
      msg("SDL_GetDesktopDisplayMode() error ... ", SDL_GetError());
      msg(" will use screen width and height from ge.cfg");
    }

    if(not_ok or not sys::FULLSCREEN){
      sys::set_screen_size(sys::WW, sys::WH);
      sys::window = SDL_CreateWindow("GerdaWindow", 0, 0, sys::WW, sys::WH, SDL_WINDOW_OPENGL);
    }
    else{
      msg("SDL_GetDesktopDisplayMode() =", desktop_mode.w, desktop_mode.h, desktop_mode.refresh_rate);
      sys::set_screen_size(desktop_mode.w, desktop_mode.h);
      sys::window = SDL_CreateWindow("GerdaWindow", 0, 0, sys::SW, sys::SH, SDL_WINDOW_OPENGL|SDL_WINDOW_FULLSCREEN);
    }

    msg("init_sdl() ... done");
    return true;
  }

  bool init_opengl(){
    msg("init_opengl() ... ");
    sys::glcontext = SDL_GL_CreateContext(sys::window);
    load_opengl_context();

    // Alpha
	  glEnable(GL_BLEND);
	  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	  //glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA );
	  glEnable(GL_ALPHA_TEST);
	  glAlphaFunc(GL_GREATER, 0);

    // Depth
    glEnable(GL_DEPTH_TEST);
	  glDepthMask(GL_TRUE);
	  glDepthFunc(GL_LEQUAL);
	  glDepthRange(0.0, 1.0);
    msg("init_opengl() ... done");
  }

  bool init_gerda(){
    msg("init_gerda() ... ");
    sys::clocker      = new Clocker(sys::fps);
    sys::mouse        = new Mouse();
    sys::keyboard     = new Keyboard();
    sys::event_parser = new EventParser();

    sys::screenshoot_counter     = 100;
    sys::screenshoot_image       = new Image(sys::WW, sys::WH);
    sys::screenshoot_image->data = new float[4 * sys::WW * sys::WH];
    sys::screenshoot_timer       = new Timer(1);

    sys::camera       = new Camera();

    sys::screen_final_frame = new FrameBuffer();

    msg("init_gerda() ... done");
    return true;
  }

  bool tick_gerda(){
    if(sys::keyboard->screenshoot and not sys::screenshoot_timer->itime){
      glReadPixels(0, 0, sys::WW, sys::WH, GL_RGBA, GL_UNSIGNED_BYTE, sys::screenshoot_image->data);
      // sys::screenshoot_image->Mirrow();
      sys::screenshoot_image->Flip();
      sys::screenshoot_image->WritePNG( get_screnshoot_name() );
      sys::screenshoot_timer->Tick();
    }
    if(sys::screenshoot_timer->itime) sys::screenshoot_timer->Tick();

    SDL_GL_SwapWindow(sys::window);
    glClearColor(sys::fb_def_color.r,sys::fb_def_color.g,sys::fb_def_color.b,sys::fb_def_color.a);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    // sleep a litle bit
    sys::clocker->Tick();

    // remove all single-time-used flags
    sys::mouse->Tick();

    // parse all SDL events: mouse
    sys::event_parser->Tick();

    // get keyboard state after all SDL events was parsed
    sys::keyboard->Tick();

    if(sys::keyboard->exit) return false; // FIXME menu, other
    return true;

    // after this call apply in main loop draw functions
  }

}

#endif
