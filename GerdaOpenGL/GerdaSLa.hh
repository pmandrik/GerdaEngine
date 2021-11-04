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
    FrameBuffer * fb;
    virtual void Tick(){
      draw_FB_to_FB( fb, target, shader );
    }
  };

  // to blit FB+FB+shader on FB
  class SLa2FB : public SLa {
    FrameBuffer *fb1, *fb2;
    virtual void Tick(){
      draw_2FB_to_FB(fb1, fb2, target, shader);
    }
  };

  // to blit FB applying shader multiple times
  class SLaFBLoop : public SLa {
    FrameBuffer *fb, *fb_buffer;
    int N_iters = 1;
    virtual void Tick(){
      if(N_iters <= 1){
        draw_FB_to_FB( fb, target, shader );
        return;
      }
      int counter = 0;
      draw_FB_to_FB( fb, fb_buffer, shader );
      for(int counter = 2; counter <= N_iters; ++counter){
        if(counter % 2) draw_FB_to_FB( target, fb_buffer, shader );
        else            draw_FB_to_FB( fb_buffer, target, shader ); 
      }
      if(N_iters%2) draw_FB_to_FB( fb_buffer, target, nullptr ); 
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

    /// we can define workflow in terms of SLa in XML:
    /// 1. define all shaders to load:
    ///   a. name, path vert, path frag
    ///   b. default values of variables
    /// 2. define all sla
    ///   a. kind of SLa
    ///   b. shader + default values for shader variables (overriding)
    ///   c. SLa attributes: clean
    ///   d. SLa specific attributes: 
    ///     !. SLaTexture - texture name, text_pos, text_size
    ///     @. 
    bool LoadJSON(string filename){
      MSG_DEBUG(__PFN__, "load", filename);
      XMLDocument doc;
      XMLError error = doc.LoadFile( filename.c_str() );
      if( error ){
        check_tinyxml_error(error, filename);
        return false;
      }

      /// Load shaders from XML
      for(XMLElement* shaders_el = doc.FirstChildElement( "shader" ); shaders_el; shaders_el = shaders_el->NextSiblingElement("shader")){
        // name
        const char * name = shaders_el->Attribute( "name" );
        if(not name) {
          msg_err(__PFN__, "skip shader without \"name\" Attribute");
          continue;
        }
        MSG_DEBUG(__PFN__, "find shader", name);

        // shader vert, frag paths
        const char * vert = shaders_el->Attribute( "vert" );
        const char * frag = shaders_el->Attribute( "frag" );
        if(not vert or not frag){
          msg_err(__PFN__, "skip shader without \"vert\" or \"frag\" Attributes");
          continue;
        } else MSG_DEBUG(__PFN__, "vert / frag path = ", vert, "/", frag);

        string vert_str = parse_shader_path(vert);
        string frag_str = parse_shader_path(frag); // replace_all_map

        // shader vars, time, n_textures
        vector<float> vars;
        for(XMLElement* var_el = shaders_el->FirstChildElement( "var" ); var_el; var_el = var_el->NextSiblingElement("var")){
          MSG_DEBUG(__PFN__, "var", vars.size(), var_el->GetText() );
          vars.push_back( atof(var_el->GetText()) );
        }

        float time = 0;
        XMLElement* time_el = shaders_el->FirstChildElement( "time" );
        if(time_el) time = atof(time_el->GetText());

        int n_textures = 1;
        XMLElement* textures_el = shaders_el->FirstChildElement( "n_textures" );
        if(textures_el) n_textures = atof(textures_el->GetText());

        FrameShader * shader = load_frame_shader(name, vert_str, frag_str, vars.size(), n_textures);
        shader->SetDefVars( vars );
        shader->SetDefTime( time );
      }

      /// Load SLaTexture from XML
      for(XMLElement* el = doc.FirstChildElement( "SLaTexture" ); el; el = el->NextSiblingElement("SLaTexture")){
        // name
        const char * name = el->Attribute( "name" );
        if(not name) {
          msg_err(__PFN__, "skip SLaTexture without \"name\" Attribute");
          continue;
        } else MSG_DEBUG(__PFN__, "find shader", name);

        
      }
    }

    void Init(){
    }
  };
}


#endif


