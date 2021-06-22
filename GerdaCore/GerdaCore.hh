// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#ifndef GERDA_CORE_HH 
#define GERDA_CORE_HH 1

#include <GerdaInputDev.hh>
#include <GerdaSystem.hh>
#include <GerdaOther.hh>
#include <GerdaInput.hh>
#include <GerdaFSM.hh>

namespace ge {

  class EventParser : public BaseClass {
    public:
      EventParser(){
        mouse    = sys::mouse;
        keyboard = sys::keyboard;
      }

      void Tick(){
        while(SDL_PollEvent(&game_event)){
          if(game_event.type == SDL_MOUSEMOTION){
            mouse->x = game_event.motion.x;
            mouse->y = game_event.motion.y;
          }
          else if(game_event.type == SDL_MOUSEBUTTONDOWN or game_event.type == SDL_MOUSEBUTTONUP) mouse->ApplyEvent(game_event);
          else if(game_event.type == SDL_MOUSEWHEEL) mouse->wheel = game_event.wheel.y;
          //else if(game_event.type == SDL_KEYDOWN or game_event.type == SDL_KEYUP) keyboard->ApplyEvent(game_event);
        }
      }

      SDL_Event game_event;

      Mouse * mouse;
      Keyboard * keyboard;
  };

}

#endif







