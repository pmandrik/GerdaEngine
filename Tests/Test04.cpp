// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
/// load Texture FB shader scene workflow from JSON

#include "GerdaEngine.hh"
#include "set"
using namespace ge;

int main(){
  sys::set_windows_size(600, 1000);
  init_sdl_screen_opengl_gerda();

  DataContainer dc;
  load_gerdadata_xml( "Data/sla_workflow.xml", & dc );
  load_sla_xml( "Data/sla_workflow.xml", & dc );
  load_sla_chain_xml( "Data/sla_workflow.xml", & dc );

  dc.GetSLaChain("chain 0")->Init();

  msg("Loop ...");
  Timer timer = Timer(900);
  bool game = true;
  while( game ){
    tick_gerda();
    game = not check_exit();

    dc.GetSLaChain("chain 0")->Tick();

    sys::camera->Tick();
    sys::camera->Resize(sys::mouse->wheel);
  }
};


