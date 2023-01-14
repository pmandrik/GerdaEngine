// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#include "GerdaEngine.hh"
#include "set"
using namespace ge;

int main(){  
  // load config
  shared_ptr<XMLConfig> cfg = make_shared<XMLConfig>( "Data/sla_test03.xml" );
  // shared_ptr<XMLConfig> cfg = make_shared<XMLConfig>( "Data/sla_test03_a.xml" );

  // init gerda
  init_gerda( cfg );

  // import cfg  
  sys::core_factory->verbose_lvl = pm::verbose::VERBOSE; 
  sys::core_factory->ImportConfig( cfg, sys::def_container );

  auto dr = new DrawRealm();
  auto ch = sys::def_container->GetSLaChain("chain");
  dr->AddChain( sys::def_container->GetSLaChain("chain") );
  
  DevelopmentHelper std ( sys::def_container->GetShaders() );

  bool game = true;
  msg("Loop ...", ch );
  while( game ){
    tick_gerda();
    game = not check_exit();
    // sys::keyboard->Print();
    // sys::mouse->Print();sys::screnshoot_prefix

    auto text = sys::def_container->GetTexture("back");

    // text->Bind();
    // text->Draw( v2(), sys::SV2 );
    // text->Unbind();
    // msg( sys::camera->zoom, sys::mouse->wheel );
    
    sys::camera->Move( -1*v2(2 * sys::keyboard->hor, 2 * sys::keyboard->ver) );
    sys::camera->Resize(sys::mouse->wheel);
    sys::camera->Tick();

    dr->Tick();
    std.Tick();
    
    ge::glReadPixels(0, 0, sys::WW, sys::WH, GL_RGBA, GL_UNSIGNED_BYTE, sys::screenshoot_image->data);
    // for(int i = 0 ; i < 100; i++) msg( int(((unsigned char*)sys::screenshoot_image->data)[ i ]) ); 
    
    sys::camera->Tick();
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


