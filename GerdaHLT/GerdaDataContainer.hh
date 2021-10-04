// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GerdaDataContainer_H
#define GerdaDataContainer_H 1  

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

    // ======= loads ====================================================================
    template<typename T>
    bool CheckIfAlreadyExist(const string & name, const map<string, T*> & container){
      typename map<string, T*>::const_iterator ptr = container.find(name);
      if(ptr == container.end()) return false;
      MSG_WARNING(__PFN__, name, "already exist, would not be able to add/create");
    }

    Image * CreateImagePNG(const string & name){
      /// create Image(), call Image::LoadPNG, add to DataContainer
      if( CheckIfAlreadyExist(name, images) ) return nullptr;
      Image * img = new Image();
      img->LoadPNG(name);
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
};

#endif
