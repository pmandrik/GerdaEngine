// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#ifndef GERDA_SDL_HH 
#define GERDA_SDL_HH 1

namespace ge {

  using namespace pm;
  using namespace std;
  using namespace tinyxml2;

  // ======= Functions for reading text files ====================================================================
  string read_text_files_sdl(const string & path){
    SDL_RWops *file = SDL_RWFromFile(path.c_str(), "r");
    if( file == NULL ) {
      msg_err("read_text_files(): cant open file", path, "return empty string");
      return "";
    }

    Sint64 res_size = SDL_RWsize(file);
    char* res = (char*)malloc(res_size + 1);

    Sint64 nb_read_total = 0, nb_read = 1;
    char* buf = res;
    while (nb_read_total < res_size && nb_read != 0) {
      nb_read = SDL_RWread(file, buf, 1, (res_size - nb_read_total));
      nb_read_total += nb_read;
      buf += nb_read;
    }
    SDL_RWclose(file);
    if (nb_read_total != res_size) {
      free(res);
      msg_err("read_text_files(): error during reading the file", path);
    }

    res[nb_read_total] = '\0';
    string str = string(res);
    free(res);
    return str;
  }

  class ReadTxtFileImpSDL : public ReadTxtFileImp {
    public:
    string read_text_files(const string & path){ return read_text_files_sdl( path ); };
  };

  // ======= MOUSE ====================================================================
  class MouseSDL {
      Mouse * mouse;
      bool locker;

      public:
      MouseSDL(){};
      MouseSDL( Mouse * mouse_ ){
        mouse = mouse_;
      }

      void Release(){ locker = false; }

      void ApplyWheel(float y){
        mouse->wheel = y;
      }

      void ApplyMovement(float x, float y){
        mouse->x = x;
        mouse->y = y;
      }

      void ApplyEvent(const SDL_Event & game_event){
        if(locker) return;
        // msg(SDL_BUTTON(SDL_BUTTON_LEFT), SDL_BUTTON(SDL_BUTTON_MIDDLE), SDL_BUTTON(SDL_BUTTON_RIGHT), SDL_BUTTON(game_event.button.button));
        if( game_event.type == SDL_MOUSEBUTTONDOWN ){
          if(game_event.button.button == SDL_BUTTON_LEFT  ) {mouse->left_down   = true; mouse->left_hold = true; }
          if(game_event.button.button == SDL_BUTTON_MIDDLE) {mouse->middle_down = true; mouse->middle_hold = true; }
          if(game_event.button.button == SDL_BUTTON_RIGHT ) {mouse->right_down  = true; mouse->right_hold = true; }
        }
        if( game_event.type == SDL_MOUSEBUTTONUP ){
          if(game_event.button.button == SDL_BUTTON_LEFT  ) {mouse->left_up   = true; mouse->left_hold = false; }
          if(game_event.button.button == SDL_BUTTON_MIDDLE) {mouse->middle_up = true; mouse->middle_hold = false; }
          if(game_event.button.button == SDL_BUTTON_RIGHT ) {mouse->right_up  = true; mouse->right_hold = false; }
        }
        locker = true; // lock mouse for any other mouse events in this tick
      } 
  };

  // ======= KEYBOARD ====================================================================
  class KeyboardSDL {
      Keyboard * keyboard;
      bool * keyboard_pressed, * keyboard_holded, * keyboard_raised;
      const Uint8 * kstate;

      public:
      KeyboardSDL(){}
      KeyboardSDL( Keyboard * keyboard_ ){
        keyboard = keyboard_;
        keyboard_pressed = keyboard->keyboard_pressed;
        keyboard_holded  = keyboard->keyboard_holded;
        keyboard_raised  = keyboard->keyboard_raised;
      }
      
      void Tick(){
        kstate = SDL_GetKeyboardState(NULL);
        for(int key = key::KSTART; key < key::KEND; key++){
          if( kstate[key] ){
            if( not keyboard_holded[key] ){
              keyboard_pressed[key] = true;
              keyboard_holded[key] = true;
             }
             else keyboard_pressed[key] = false;
           }
           else{
            if( keyboard_holded[key] ){
              keyboard_holded[key] = false;
              keyboard_raised[key] = true;
              keyboard_pressed[key] = false;
            }
            else keyboard_raised[key] = false;
          }
        }
      }
  };

  // ======= CLOCKER ====================================================================
  class ClockerSDL {
    public:
    ClockerSDL(){}
    ClockerSDL(int fps){ delay = 1000/fps; ntime = SDL_GetTicks(); };
    float Tick(void){
      if(delay > SDL_GetTicks() - ntime) SDL_Delay( delay - SDL_GetTicks() + ntime );
      answer = SDL_GetTicks() - ntime;
      ntime = SDL_GetTicks();
      return 1./answer;
    }
    Uint32 delay, ntime, dtime, answer;
  };

  // ======= OPENGL FUNCTION LOAD VIA SDL ====================================================================
  #define SDL_PROC(ret,func,params) typedef ret (APIENTRY * GL_ ## func ## _Func) params;
  #include "SDL_glfuncs.h"
  #undef SDL_PROC

  #define SDL_PROC(ret,func,params) GL_ ## func ## _Func func = nullptr;
  #include "SDL_glfuncs.h"
  #undef SDL_PROC

  bool load_opengl_context(){
    msg("load_opengl_context() ... ");
    #if SDL_VIDEO_DRIVER_UIKIT
    #define __SDL_NOGETPROCADDR__
    #elif SDL_VIDEO_DRIVER_ANDROID
    #define __SDL_NOGETPROCADDR__
    #elif SDL_VIDEO_DRIVER_PANDORA
    #define __SDL_NOGETPROCADDR__
    #endif

    #if defined __SDL_NOGETPROCADDR__
    #define SDL_PROC(ret,func,params) ge::##func=func;
    #else
    #define SDL_PROC(ret,func,params) \
        do { \
            ge::func = (GL_ ## func ## _Func) SDL_GL_GetProcAddress(#func); \
            msg(#func, ge::func); \
            if ( ge::func == nullptr ) { \
                msg_err("Couldn't load GL function", #func, SDL_GetError());\
                return 0;\
            } \
        } while ( 0 );
    #endif /* __SDL_NOGETPROCADDR__ */

    #include "SDL_glfuncs.h"
    #undef SDL_PROC
    return 1;
  }

  // ======= Core ====================================================================
  class CoreSDL : public Core {
    MouseSDL mouse_sdl;
    KeyboardSDL keyboad_sdl;
    SDL_Event game_event;
    ClockerSDL clocker_sdl;

    public:
    SDL_Window * window;
    SDL_GLContext glcontext;

    CoreSDL(Mouse * mouse, Keyboard * keyboard, int fps){
      mouse_sdl = MouseSDL( mouse );
      keyboad_sdl = KeyboardSDL( keyboard );
      clocker_sdl = ClockerSDL( fps );
    }

    void Tick(){
      // sleep a litle bit
      clocker_sdl.Tick();

      /// check SDL events & update mouse state
      while(SDL_PollEvent(&game_event)){
        if(game_event.type == SDL_MOUSEMOTION) mouse_sdl.ApplyMovement( game_event.motion.x, game_event.motion.y );
        else if(game_event.type == SDL_MOUSEBUTTONDOWN or game_event.type == SDL_MOUSEBUTTONUP) mouse_sdl.ApplyEvent( game_event );
        else if(game_event.type == SDL_MOUSEWHEEL) mouse_sdl.ApplyWheel( game_event.wheel.y );
      }

      /// update keyboard state
      keyboad_sdl.Tick();

      SDL_GL_SwapWindow(window);
      glClearColor(sys::fb_def_color.r,sys::fb_def_color.g,sys::fb_def_color.b,sys::fb_def_color.a);
      glClear(GL_COLOR_BUFFER_BIT);
      glClear(GL_DEPTH_BUFFER_BIT);
    }
  };

}//namespace
#endif
