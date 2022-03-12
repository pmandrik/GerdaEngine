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
    float angle = 0;

    virtual void Tick(){
      if(shader){
        shader->Bind();
        shader->UpdateUniforms();
      }
      target->Target();
      text->Draw( text_pos, text_size, v2(0., 0.),  v2(1., 1.),  0,  false,  false, angle );
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
  struct SLaChainLink{
    string name;
    vector<string> arguments;
    SLaChainLink(string n, vector<string> & a){ name = n; arguments = a; }
  };

  class SLaChain : public BaseClass{
    public:
    map<string,SLa*> slas_map ;
    vector<SLa*> slas ;
    DataContainer * dc ;
    vector< pair<SLaChainLink, SLaChainLink > > links;

    SLaChainLink ParceRelation(string item){
      string sla_name;
      vector<string> sla_arguments;
      parce_string_function(item, sla_name, sla_arguments);
      MSG_DEBUG(__PFN__, "ParceRelation", item, "->", sla_name);
      return SLaChainLink(sla_name, sla_arguments);
    }

    void AddRelation(string src, string tgt){
      MSG_DEBUG( __PFN__, src, "&", tgt );
      links.push_back( make_pair(ParceRelation(src), ParceRelation(tgt)));
    }

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

  /// we can define workflow in terms of SLa in XML:
  /// 1. define all shaders to load:
  ///   a. name, path vert, path frag
  ///   b. default values of variables
  /// 2. define all sla
  ///   a. kind of SLa
  ///   b. shader + default values for shader variables (overriding) TODO
  ///   c. SLa attributes: clean
  ///   d. SLa specific attributes: 
  ///     !. SLaTexture - texture name, text_pos, text_size
  bool get_common_sla_xml(XMLElement * el, DataContainer * dc, SLa * sla){
    /// Load SLa name 
    const char * name = el->Attribute( "name" );
    if(not name) {
      msg_err(__PFN__, "skip SLaTexture without \"name\" Attribute");
      return false;
    } else MSG_DEBUG(__PFN__, "find SLa", name);
    sla->name = name;

    /// shader 
    const char * shader_name = el->Attribute( "shader" );
    if(shader_name) {
      MSG_DEBUG(__PFN__, "with shader", shader_name);
      sla->shader = dc->GetFrameShader( shader_name );
    } else MSG_DEBUG(__PFN__, "without shader");

    /// bool clean
    XMLElement* textures_el = el->FirstChildElement( "clean" );
    if(textures_el) sla->clean = true;

    return true;
  }
  
  bool load_gerdadata_xml(string filename, DataContainer * dc){
    MSG_DEBUG(__PFN__, "load", filename);
    XMLDocument doc;
    XMLError error = doc.LoadFile( filename.c_str() );
    if( error ){
      check_tinyxml_error(error, filename);
      return false;
    }

    /// Load Images from XML
    for(XMLElement* shaders_el = doc.FirstChildElement( "image" ); shaders_el; shaders_el = shaders_el->NextSiblingElement("image")){
      /// name
      const char * name = shaders_el->Attribute( "name" );
      if(not name) {
        msg_err(__PFN__, "skip image without \"name\" Attribute");
        continue;
      } MSG_DEBUG(__PFN__, "find image", name);

      const char * path = shaders_el->Attribute( "path" );
      if(not path){
        msg_err(__PFN__, "skip image without \"path\" Attributes");
        continue;
      } else MSG_DEBUG(__PFN__, "with path = ", path);

      string path_str = parse_shader_path(path);
      if( not dc->CreateImagePNG( name, path_str ) ) msg_err(__PFN__, "not able to create image", name, path_str);
    }

    /// Load Textures from XML
    for(XMLElement* shaders_el = doc.FirstChildElement( "texture" ); shaders_el; shaders_el = shaders_el->NextSiblingElement("texture")){
      /// name
      const char * name = shaders_el->Attribute( "name" );
      if(not name) {
        msg_err(__PFN__, "skip texture without \"name\" Attribute");
        continue;
      } MSG_DEBUG(__PFN__, "find texture", name);

      const char * image = shaders_el->Attribute( "image" );
      if(not image){
        msg_err(__PFN__, "skip texture without \"image\" Attributes");
        continue;
      } else MSG_DEBUG(__PFN__, "with image = ", image);

      if( not dc->CreateTexture( name, image ) ) msg_err(__PFN__, "not able to create texture", name, image);
    }

    /// Load shaders from XML
    for(XMLElement* shaders_el = doc.FirstChildElement( "shader" ); shaders_el; shaders_el = shaders_el->NextSiblingElement("shader")){
      /// name
      const char * name = shaders_el->Attribute( "name" );
      if(not name) {
        msg_err(__PFN__, "skip shader without \"name\" Attribute");
        continue;
      } MSG_DEBUG(__PFN__, "find shader", name);

      /// shader vert, frag paths
      const char * vert = shaders_el->Attribute( "vert" );
      const char * frag = shaders_el->Attribute( "frag" );
      if(not vert or not frag){
        msg_err(__PFN__, "skip shader without \"vert\" or \"frag\" Attributes");
        continue;
      } else MSG_DEBUG(__PFN__, "vert / frag path = ", vert, "/", frag);

      string vert_str = parse_shader_path(vert);
      string frag_str = parse_shader_path(frag); // replace_all_map

      /// shader vars, time, n_textures
      vector<float> vars;
      for(XMLElement* var_el = shaders_el->FirstChildElement( "var" ); var_el; var_el = var_el->NextSiblingElement("var")){
        MSG_DEBUG(__PFN__, "var", get_xml_attribute_float(var_el, "value", 0.f) );
        vars.push_back( get_xml_attribute_float(var_el, "value", 0.f) );
      }

      float time     = get_xml_attribute_float( shaders_el, "time", "value", 0.f );
      int n_textures = get_xml_attribute_int( shaders_el, "texture", "N", 1.f );

      FrameShader * shader = load_frame_shader(name, vert_str, frag_str, vars.size(), n_textures);
      shader->SetDefVars( vars );
      shader->SetDefTime( time );
  
      dc->AddFrameShader(shader->shader_name, shader);
    }
  }

  bool load_sla_xml(string filename, DataContainer * dc){
    MSG_DEBUG(__PFN__, "load", filename);
    XMLDocument doc;
    XMLError error = doc.LoadFile( filename.c_str() );
    if( error ){
      check_tinyxml_error(error, filename);
      return false;
    }

    /// Load SLaTexture from XML
    for(XMLElement* el = doc.FirstChildElement( "SLaTexture" ); el; el = el->NextSiblingElement("SLaTexture")){
      SLaTexture * sla = new SLaTexture();
      if( not get_common_sla_xml( el, dc, (SLa*)sla ) ) { delete sla; continue; }
      MSG_DEBUG(__PFN__, "kind SLaTexture");

      /// with texture name, size, position
      const char * name_texture = el->Attribute( "texture" );
      if(not name_texture) {
        msg_err(__PFN__, "skip SLaTexture without \"texture\" Attribute");
        delete sla; continue;
      } MSG_DEBUG(__PFN__, "with texture", name_texture);

      sla->text = dc->GetTexture( name_texture );
      if(not sla->text){
        msg_err(__PFN__, "skip SLaTexture due unloaded \"texture\" Attribute");
        delete sla; continue;
      }

      float pos_x  = get_xml_attribute_float( el, "pos_x", "value", 0.f );
      float pos_y  = get_xml_attribute_float( el, "pos_y", "value", 0.f );
      float size_x = get_xml_attribute_float( el, "size_x", "value", 1.f );
      float size_y = get_xml_attribute_float( el, "size_y", "value", 1.f );
      sla->text_pos = v2(pos_x, pos_y);
      sla->text_size = v2(size_x, size_y);

      MSG_DEBUG(__PFN__, "with pos, size", sla->text_pos, sla->text_size);

      dc->AddSLa(sla->name, (SLa*)sla);
    }

    /// Load SLaFB
    for(XMLElement* el = doc.FirstChildElement( "SLaFB" ); el; el = el->NextSiblingElement("SLaFB")){
      SLaFB * sla = new SLaFB();
      if( not get_common_sla_xml( el, dc, (SLa*)sla ) ) { delete sla; continue; }
      MSG_DEBUG(__PFN__, "kind SLaFB");
      dc->AddSLa(sla->name, (SLa*)sla);
    }

    /// Load SLa2FB
    for(XMLElement* el = doc.FirstChildElement( "SLa2FB" ); el; el = el->NextSiblingElement("SLa2FB")){
      SLa2FB * sla = new SLa2FB();
      if( not get_common_sla_xml( el, dc, (SLa*)sla ) ) { delete sla; continue; }
      MSG_DEBUG(__PFN__, "kind SLa2FB");
      dc->AddSLa(sla->name, (SLa*)sla);
    }

    /// Load SLaFBLoop
    for(XMLElement* el = doc.FirstChildElement( "SLaFBLoop" ); el; el = el->NextSiblingElement("SLaFBLoop")){
      SLaFBLoop * sla = new SLaFBLoop();
      if( not get_common_sla_xml( el, dc, (SLa*)sla ) ) { delete sla; continue; }
      sla->loops = get_xml_attribute_int( el, "loops", 1 );
      MSG_DEBUG(__PFN__, "kind SLaFBLoop");
      dc->AddSLa(sla->name, (SLa*)sla);
    }
  }

  bool load_sla_chain_xml(string filename, DataContainer * dc){
    /// Chain
    for(XMLElement* el = doc.FirstChildElement( "chain" ); el; el = el->NextSiblingElement("chain")){
      const char * name = el->Attribute("name");
      if(not name) {
        msg_err(__PFN__, "skip chain without \"name\" Attribute");
        continue;
      } MSG_DEBUG(__PFN__, "find chain", name);

      SLaChain * chain = new SLaChain( dc );
      for(XMLElement* link_el = el->FirstChildElement( "relation" ); link_el; link_el = link_el->NextSiblingElement("relation")){
        const char * value = link_el->Attribute("value");
        if(not value) {
          msg_err(__PFN__, "skip link without \"value\" Attribute");
          continue;
        } MSG_DEBUG(__PFN__, "find link", value);

        string value_cpp = string(value);
        std::vector<std::string> value_cpp_parts;
        pm::split_string_strip(value_cpp, value_cpp_parts, ",");
        if( not value_cpp_parts.size() ){
          msg_err(__PFN__, "skip empty relation");
          continue;
        } MSG_DEBUG(__PFN__, "find relation with N parts = ", value_cpp_parts.size());

        for(auto value_cpp_part : value_cpp_parts){
          MSG_DEBUG("x");
          std::vector<std::string> conn_parts;
          pm::split_string_strip(value_cpp_part, conn_parts, "->");
          if( conn_parts.size() < 2 ){
            msg_err(__PFN__, "skip relation without \"->\" separated parts");
            continue;
          } MSG_DEBUG(__PFN__, "find relation with N in connection = ", conn_parts.size());

          for(int i = 0; i < conn_parts.size()-1; ++i){
            string src = conn_parts[i];
            string tgt = conn_parts[i+1];
            chain->AddRelation( src , tgt );
          }
        }
      }
      dc->AddSLaChain(name, chain);
    }
  }
}


#endif


