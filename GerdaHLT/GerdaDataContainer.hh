// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GerdaDataContainer_H
#define GerdaDataContainer_H 1  

static int verbose_lvl = ge::sys::verbose_lvl;

namespace ge {
  class DataContainer : public BaseClass {
    public:
    /// handle default data expected to be used in application, such as  
    /// 1. images  
    /// 2. textures  
    /// 3. animations  
    /// 4. shaders  
    /// 5. tilemap informatiom  
    /// 6. sounds  
    /// ...
    
    map<string, Image *> images;
    map<string, Texture * > textures;
    map<string, ArrayQuadsDrawer * > array_quads_drawers;
    map<string, ImageLine * > image_lines;
    map<string, Sprite * > sprites;
    map<string, TiledMap * > tiled_maps;
    map<string, TiledMapDrawer * > tiled_map_drawers;
    map<string, ParticleSystem * > particle_systems;
    map<string, FrameShader * > frame_shaders;

    Object * player;
    map<string, Object*> players;

    // ======= get ====================================================================
    template<typename T>
    T * Get(const string & name, const map<string, T*> & container){
      typename map<string, T*>::const_iterator ptr = container.find(name);
      if(ptr == container.end()) {
        MSG_WARNING(__PFN__, "can't find", name, ", return nullptr");
        return nullptr;
      }
      return ptr->second;
    }

    Image   * GetImage(const string & name){ return Get(name, images); }
    Texture * GetTexture(const string & name){ return Get(name, textures); }
    ArrayQuadsDrawer * GetArrayDrawer(const string & name){ return Get(name, array_quads_drawers); }
    Sprite * GetSprite(const string & name){ return Get(name, sprites); }
    TiledMap * GetTiledMap(const string & name){ return Get(name, tiled_maps); }
    TiledMapDrawer * GetTiledMapDrawer(const string & name){ return Get(name, tiled_map_drawers); }
    ParticleSystem * GetParticleSystem(const string & name){ return Get(name, particle_systems); }
    Object * GetPlayer(const string & name){ return Get(name, players); }
    FrameShader * GetFrameShader(const string & name){ return Get(name, frame_shaders); }

    // ======= add ====================================================================
    template<typename T>
    bool Add(const string & name, T * obj, map<string, T*> & container){
      if( CheckIfAlreadyExist(name, container) ) return false;
      MSG_INFO(__PFN__, name);
      container[ name ] = obj;
      return true;
    }

    bool AddImage(const string & name, Image   * obj){ return Add(name, obj, images); }
    bool AddTexture(const string & name, Texture   * obj){ return Add(name, obj, textures); }
    bool AddArrayDrawer(const string & name, ArrayQuadsDrawer   * obj){ return Add(name, obj, array_quads_drawers); }
    bool AddSprite(const string & name, Sprite   * obj){ return Add(name, obj, sprites); }
    bool AddTiledMap(const string & name, TiledMap   * obj){ return Add(name, obj, tiled_maps); }
    bool AddTiledMapDrawer(const string & name, TiledMapDrawer   * obj){ return Add(name, obj, tiled_map_drawers); }
    bool AddParticleSystem(const string & name, ParticleSystem   * obj){ return Add(name, obj, particle_systems); }
    bool AddPlayer(const string & name, Object * obj){ return Add(name, obj, players); }
    bool AddFrameShader(const string & name, FrameShader * obj){ return Add(name, obj, frame_shaders); }

    // ======= loads ====================================================================
    template<typename T>
    bool CheckIfAlreadyExist(const string & name, const map<string, T*> & container){
      typename map<string, T*>::const_iterator ptr = container.find(name);
      if(ptr == container.end()) return false;
      MSG_WARNING(__PFN__, name, "already exist, would not be able to add/create");
    }

    Image * CreateImagePNG(const string & name, const string & path){
      /// create Image(), call Image::LoadPNG, add to DataContainer
      if( CheckIfAlreadyExist(name, images) ) return nullptr;
      Image * img = new Image();
      img->LoadPNG(path);
      images[name] = img;
      return img;
    }

    Texture * CreateTexture(const string & name, const string & img_name){
      /// find Image, create Texture(Image)
      if( CheckIfAlreadyExist(name, textures) ) return nullptr;
      Image * img = GetImage(img_name);
      Texture * txt = new Texture( img );
      textures[name] = txt;
      return txt;
    }

    // ======= set ====================================================================
    bool SetPlayer(const string & name){
      player = Get(name, players);
      return player != nullptr;
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

  bool load_sla_xml(string filename, DataContainer * dc, SLaSystem * system){
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

      float pos_x  = get_float_xml_element_value( el, "pos_x", 0.f );
      float pos_y  = get_float_xml_element_value( el, "pos_y", 0.f );
      float size_x = get_float_xml_element_value( el, "size_x", 1.f );
      float size_y = get_float_xml_element_value( el, "size_y", 1.f );
      sla->text_pos = v2(pos_x, pos_y);
      sla->text_size = v2(size_x, size_y);

      MSG_DEBUG(__PFN__, "with pos, size", sla->text_pos, sla->text_size);

      system->slas_map[sla->name] = sla;
    }

    /// Load SLaFB
    for(XMLElement* el = doc.FirstChildElement( "SLaFB" ); el; el = el->NextSiblingElement("SLaFB")){
      SLaFB * sla = new SLaFB();
      if( not get_common_sla_xml( el, dc, (SLa*)sla ) ) { delete sla; continue; }
      MSG_DEBUG(__PFN__, "kind SLaFB");
      system->slas_map[sla->name] = sla;
    }

    /// Load SLa2FB
    for(XMLElement* el = doc.FirstChildElement( "SLa2FB" ); el; el = el->NextSiblingElement("SLa2FB")){
      SLa2FB * sla = new SLa2FB();
      if( not get_common_sla_xml( el, dc, (SLa*)sla ) ) { delete sla; continue; }
      MSG_DEBUG(__PFN__, "kind SLa2FB");
      system->slas_map[sla->name] = sla;
    }

    /// Load SLaFBLoop
    for(XMLElement* el = doc.FirstChildElement( "SLaFBLoop" ); el; el = el->NextSiblingElement("SLaFBLoop")){
      SLaFBLoop * sla = new SLaFBLoop();
      if( not get_common_sla_xml( el, dc, (SLa*)sla ) ) { delete sla; continue; }
      sla->loops = get_int_xml_element_value( el, "loops", 1 );
      MSG_DEBUG(__PFN__, "kind SLaFBLoop");
      system->slas_map[sla->name] = sla;
    }

    /// Chain
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
        MSG_DEBUG(__PFN__, "var", vars.size(), var_el->GetText() );
        vars.push_back( atof(var_el->GetText()) );
      }

      float time     = get_float_xml_element_value( shaders_el, "time", 0.f );
      int n_textures = get_int_xml_element_value( shaders_el, "n_textures", 1 );

      FrameShader * shader = load_frame_shader(name, vert_str, frag_str, vars.size(), n_textures);
      shader->SetDefVars( vars );
      shader->SetDefTime( time );
  
      dc->AddFrameShader(shader->shader_name, shader);
    }
  }

};

#endif
