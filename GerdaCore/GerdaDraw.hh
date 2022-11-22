// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#ifndef GERDA_DRAW_HH 
#define GERDA_DRAW_HH 1

namespace ge {

  using namespace pm;
  using namespace std;
  using namespace tinyxml2;

  // ==================================================================== Realms  
  class Realm { };

  class DrawRealm : public Realm {
    public:
    DrawRealm(){}

    vector< std::shared_ptr<SLaChain> > chains;
    void AddChain( std::shared_ptr<SLaChain> chain ){
      chains.push_back( chain );
    }

    void ExecuteChain( std::shared_ptr<SLaChain> chain ){
      std::vector< std::shared_ptr<SLa> > & slas = chain->slas;
      for(int i = 0, i_max = slas.size(); i < i_max; ++i){
        std::shared_ptr<SLa> sla = slas[ i ];
        
        if(not sla->target ) sys::camera->ReTick();
        if(    sla->target ) sys::camera->LoadFBDefault();
        
        if(not sla->on) continue;
        if(sla->clean) sla->Clean();
        sla->Tick();
        if(sla->once) sla->on = false;
      }
    }

    void Tick(){
      /// Execute chains
      for( auto chain : chains ) ExecuteChain( chain );
    }
  };

/*
  At game step:
    iterate over game objects and update drawable objects variables

  At draw step:
    pass drawable objects variable to drawer and reserve for drawing

    in defined order following may happend
      - drawer draw (Texture, other) 
      - 
*/
}
#endif







