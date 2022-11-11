// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#include "GerdaEngine.hh"
#include "set"
using namespace ge;

int main(){
  // load config
  shared_ptr<XMLConfig> cfg = make_shared<XMLConfig>( "Data/sla_test03.xml" );

  // init gerda
  init_gerda( "sdl", cfg );

  // import cfg  
  sys::core_factory->verbose_lvl = pm::verbose::VERBOSE; 
  sys::core_factory->ImportConfig( cfg, sys::def_container );

  bool game = true;
  msg("Loop ...");
  while( game ){
    tick_gerda();
    game = not check_exit();
    // sys::keyboard->Print();

    auto text = sys::def_container->GetTexture("back");

    // text->Bind();
    // text->Draw( v2(), sys::SV2 );
    // text->Unbind();

    sys::camera->Tick();
    sys::camera->Resize(sys::mouse->wheel);
  }

  // TODO
  // DRAWER STEP:
  // load image -> load all images defined in cfg file
  // create texture drawer will use -> create all textures defined in cfg file
  // create drawer
  // create drawer implementation
  // SLA STEP
  // create shader
  // create frame buffer
  // create SLa to use drawer also ...
  // FACTORY STEP
  // create factory to use drawer implementation
  // OBJECT STEP
  // create object in this factory

  // 1. create game object
  // in factory class
  // where we provid how it is draw
  //
};


