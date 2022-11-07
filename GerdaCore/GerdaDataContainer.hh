// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GerdaDataContainer_H
#define GerdaDataContainer_H 1  

static int verbose_lvl = ge::sys::verbose_lvl;

namespace ge {

  using namespace pm;
  using namespace std;
  using namespace tinyxml2;

  class SLa;
  class SLaChain;
  class Image;
  class Texture;
  class ArrayQuadsDrawer;
  class ImageLine;
  class Sprite;
  class TiledMap;
  class TiledMapDrawer;
  class ParticleSystem;
  class Shader;
  class Object;
  class QuadsDrawer;

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
    
    string name="Default_DataContainer";

    map<string, std::shared_ptr<Image> > images;
    map<string, std::shared_ptr<Texture> > textures;
    map<string, std::shared_ptr<ArrayQuadsDrawer> > array_quads_drawers;
    map<string, std::shared_ptr<ImageLine> > image_lines;
    map<string, std::shared_ptr<Sprite> > sprites;
    map<string, std::shared_ptr<TiledMap> > tiled_maps;
    map<string, std::shared_ptr<TiledMapDrawer> > tiled_map_drawers;
    map<string, std::shared_ptr<ParticleSystem> > particle_systems;
    map<string, std::shared_ptr<Shader> > frame_shaders;
    map<string, std::shared_ptr<SLa> > slas;
    map<string, std::shared_ptr<SLaChain> > sla_chains;
    map<string, std::shared_ptr<QuadsDrawer> > quads_drawers;

    Object * player;
    map<string, std::shared_ptr<Object> > players;
    map<string,string> path_db = { std::make_pair<string, string> ("def:", "Data/default_shaders/") };

    // ======= get ====================================================================
    template<typename T>
    std::shared_ptr<T> Get(const string & name, const map<string, std::shared_ptr<T> > & container){
      if( not name.size() ) return nullptr;
      typename map<string, std::shared_ptr<T> >::const_iterator ptr = container.find(name);
      if(ptr == container.end()) {
        MSG_WARNING(__PFN__, "can't find", name, ", return nullptr");
        return nullptr;
      }
      return ptr->second;
    }

    std::shared_ptr<Image> GetImage(const string & name){ return Get(name, images); }
    std::shared_ptr<Texture> GetTexture(const string & name){ return Get(name, textures); }
    std::shared_ptr<Sprite> GetSprite(const string & name){ return Get(name, sprites); }
    std::shared_ptr<TiledMap> GetTiledMap(const string & name){ return Get(name, tiled_maps); }
    std::shared_ptr<TiledMapDrawer> GetTiledMapDrawer(const string & name){ return Get(name, tiled_map_drawers); }
    std::shared_ptr<ParticleSystem> GetParticleSystem(const string & name){ return Get(name, particle_systems); }
    std::shared_ptr<Object> GetPlayer(const string & name){ return Get(name, players); }
    std::shared_ptr<Shader> GetShader(const string & name){ return Get(name, frame_shaders); }
    std::shared_ptr<SLa> GetSLa(const string & name){ return Get(name, slas); }
    std::shared_ptr<SLaChain> GetSLaChain(const string & name){ return Get(name, sla_chains); }
    std::shared_ptr<QuadsDrawer> GetQuadsDrawer(const string & name){ return Get(name, quads_drawers); }    

    // ======= add ====================================================================
    template<typename T>
    bool Add(const string & name, std::shared_ptr<T> obj, map<string, std::shared_ptr<T> > & container){
      if( CheckIfAlreadyExist(name, container) ) return false;
      MSG_INFO(__PFN__, name);
      container[ name ] = obj;
      return true;
    }

    bool Add(const string & name, std::shared_ptr<Image> obj){ return Add(name, obj, images); }
    bool Add(const string & name, std::shared_ptr<Texture   > obj){ return Add(name, obj, textures); }
    bool Add(const string & name, std::shared_ptr<ArrayQuadsDrawer   > obj){ return Add(name, obj, array_quads_drawers); }
    bool Add(const string & name, std::shared_ptr<Sprite   > obj){ return Add(name, obj, sprites); }
    bool Add(const string & name, std::shared_ptr<TiledMap   > obj){ return Add(name, obj, tiled_maps); }
    bool Add(const string & name, std::shared_ptr<TiledMapDrawer   > obj){ return Add(name, obj, tiled_map_drawers); }
    bool Add(const string & name, std::shared_ptr<ParticleSystem   > obj){ return Add(name, obj, particle_systems); }
    bool Add(const string & name, std::shared_ptr<Object > obj){ return Add(name, obj, players); }
    bool AddX(const string & name, std::shared_ptr<Shader> obj){ return Add(name, obj, frame_shaders); }
    bool Add(const string & name, std::shared_ptr<SLa > obj){ return Add(name, obj, slas); }
    bool Add(const string & name, std::shared_ptr<SLaChain > obj){ return Add(name, obj, sla_chains); }
    bool Add(const string & name, std::shared_ptr<QuadsDrawer > obj){ return Add(name, obj, quads_drawers); }    

    // ======= loads ====================================================================
    template<typename T>
    bool CheckIfAlreadyExist(const string & name, const map<string, std::shared_ptr<T> > & container){
      typename map<string, std::shared_ptr<T>>::const_iterator ptr = container.find(name);
      if(ptr == container.end()) return false;
      MSG_WARNING(__PFN__, name, "already exist, would not be able to add/create");
    }
  };

}
#endif
