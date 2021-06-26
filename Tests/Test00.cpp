// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#include "GerdaEngine.hh"
using namespace ge;

/// default workflow test - create screen, draw white quad
int main(){
  init_sdl();
  init_sdl_screen();
  init_opengl();
  init_gerda();

  Timer timer = Timer(90);
  bool game = true;
  while( game ){
    tick_gerda();
    game = not check_exit();
    
    timer.Tick();
    v2 pos  = v2(0.5, 0.5);
    v2 size = v2(0.1, 0.1);
    draw_quad(pos, size, timer.itime);
  }
};  
