// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GerdaSLa_H
#define GerdaSLa_H 1

namespace ge {

  // template
  class SLa {
    /// SLa shader scene mother class
    public:
    SLa(){}
    SLa(FrameBuffer * t, FrameShader * s) : target(t), shader(s) {}

    FrameBuffer * target = nullptr;
    FrameShader * shader = nullptr;
    bool on    = true;
    bool clean = false;
    string name;
    virtual void Tick(){};
  };

  // to blit texture + shader on FB
  class SLaTexture : public SLa {
    public:
    Texture * text;
    v2 text_pos = v2(), text_size = sys::FBV2;

    virtual void Tick(){
      if(shader){
        shader->Bind();
        shader->UpdateUniforms();
      }
      target->Target();
      text->Draw( text_pos, text_size );
      target->Untarget();
      if(shader) shader->Unbind(); 
    }
  };

  // to blit FB+shader on FB
  class SLaFB : public SLa {
    FrameBuffer * source;
    virtual void Tick(){
      draw_FB_to_FB( source, target, shader );
    }
  };

  // to blit FB+FB+shader on FB
  class SLa2FB : public SLa {
    FrameBuffer * source_1, * source_2;
    virtual void Tick(){
      draw_2FB_to_FB(source_1, source_2, target, shader);
    }
  };

  // to blit FB applying shader multiple times
  class SLaFBLoop : public SLa {
    public:
    FrameBuffer * source, * buffer;
    int loops = 1;
    virtual void Tick(){
      if(loops <= 1){
        draw_FB_to_FB( source, target, shader );
        return;
      }
      int counter = 0;
      draw_FB_to_FB( source, buffer, shader );
      for(int counter = 2; counter <= loops; ++counter){
        if(counter % 2) draw_FB_to_FB( target, buffer, shader );
        else            draw_FB_to_FB( buffer, target, shader ); 
      }
      if(loops%2) draw_FB_to_FB( buffer, target, nullptr ); 
    }
  };

  // 
  class SLaSystem : public BaseClass{
    public:
    map<string,SLa*> slas_map ;
    vector<SLa*> slas ;

    void Tick(){
      for(int i = 0, i_max = slas.size(); i < i_max; ++i){
        SLa* sla = slas[ i ];
        if(not sla->on) continue;
        if(not sla->target) continue;
        if(sla->clean) sla->target->Clean();
        sla->Tick();
      }
    }

    void Init(){
    }
  };
}


#endif


