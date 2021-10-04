// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#ifndef GerdaMap_H
#define GerdaMap_H 1 

namespace ge {

  struct CartaPoint{
    CartaPoint(const v2 & pos_){ pos = pos_; }
    CartaPoint(const v2 & pos_, const vector<string> & meta_){
      pos = pos_;
      meta = meta_;
    }
    v2 pos;
    vector<string> meta;
  };

  struct CartaTrack{
    vector<v2> points;
    float lenght = -1;

    float GetLenght(){
      float L = 0;
      for(int i = 1; i < points.size(); ++i)
        L += (points[i] - points[i-1]).L();
      return L;
    }

    v2 GetPos(const float & step){
      if( not points.size() ) return v2();
      if( lenght < 0 ) lenght = GetLenght();
      if( points.size() == 1 or lenght <= 0) return points.at(0);

      float L = 0, dL = 0;
      int N_step = 1;
      for(N_step = 1; i < points.size(); ++i){
        dL = (points[i] - points[i-1]).L();
        if( step <= (L+dL) / lenght ) break;
        L += dL;
      }

      float frac = (step - L/lenght);
      return interpolation_linear( points[N_step-1], points[N_step], frac );
    }

  };

  class Carta {
    public:
    map<string, vector<CartaPoint> > coordinates;
    map<string, vector<CartaTrack> > tracks;

    void AddCoodinate(const string & id, const v2 & pos){
      auto it = coordinates.find(id);
      if( it != coordinates.end() ) it->second.emplace_back( CartaPoint(pos) );
      else coordinates[id] = { CartaPoint(pos) };
    }

    void AddCoodinate(const string & id, const v2 & pos, const vector<string> & meta){
      auto it = coordinates.find(id);
      if( it != coordinates.end() ) it->second.emplace_back( CartaPoint(pos, meta) );
      else coordinates[id] = { CartaPoint(pos, meta) };
    }
  };
  
  /// pmTiledMap work with map of integer of sizes: map_size_x * map_size_y * map_size_layers  
  /// map_size_layers mean multiple layers of tiles with different int type  
  /// value 0 of 0 layer is reserved for Solid tiles  
  class TiledMap : public BaseClass, public pmTiledMap, public Carta {
    // pmTiledMap:
    // int msize_x, msize_y, msize_layers, msize_layers_x_msize_y;
    // int tsize, bullet_grid_size;
    // int * mdata;
    public:
      TiledMap(){}
      TiledMap( const int & map_size_x, const int & map_size_y, const int & map_size_layers, const int & tile_size, int * map_data ) : BaseClass(), pmTiledMap( map_size_x, map_size_y, map_size_layers, tile_size, map_data) {
      }

      void DrawSlow(int layer=0, int mask=0, float z_pos = 0.){
        /// defaut function for drawing map using GL_QUADS, slow
        glBegin(GL_QUADS);
        for(int y = 0; y < msize_y; y++){
          for(int x = 0; x < msize_x; x++){
            if( mask != mdata[ msize_layers_x_msize_y*x + msize_layers*y + layer ] ) continue;
	          glVertex3f(x * tsize, y * tsize, z_pos);
	          glVertex3f(x * tsize, y * tsize + tsize, z_pos);
	          glVertex3f(x * tsize + tsize, y * tsize + tsize, z_pos);
	          glVertex3f(x * tsize + tsize, y * tsize, z_pos);
          }
        }
        glEnd();
      }
  };

  class TiledMapDrawer : public BaseClass {
    public:
    /// draw map using ArrayQuadsDrawer and Sprites
    TiledMapDrawer(TiledMap * map_, ArrayQuadsDrawer * drawer_){
      tiled_map = map_;
      drawer = drawer_;
    }

    void Tick(){
      /// update animated tiles
      for(auto iter : animated_tiles){
        iter.second->Get();
        iter.second->SendChanges();
      }
    }

    void Draw(){
      drawer->Draw();
    }

    void AddSprite(Sprite* sprite, bool animated=false){
      sprite->SetDrawer( drawer );
      tiles.push_back(sprite);
    }

    vector <Sprite*> tiles;
    map<pair<int, int>, Sprite*> animated_tiles;
  
    ArrayQuadsDrawer * drawer;
    TiledMap * tiled_map;
  };

  // basic lightweight wrapper around pmPhysicContainer
  class TiledPhysic : public BaseClass, public pmPhysicContainer {
    public:
      TiledPhysic(TiledMap * tiled_map){
        SetMap( tiled_map );
      }
      
      void SetMap(TiledMap * map_){
        pmPhysicContainer::msize_layers = map_->msize_layers;
        pm_physic.SetMap( (pmTiledMap*)map_ );
        tiled_map = map_;
      }
    
      void Tick(){
        interacted_object_x_object_pairs.clear();
        interacted_bullet_x_object_pairs.clear();
        pm_physic.Tick( objects, particles, interacted_object_x_object_pairs, interacted_bullet_x_object_pairs);
      }
      
      std::vector< std::pair<int, int> > interacted_object_x_object_pairs;
      std::vector< std::pair<int, int> > interacted_bullet_x_object_pairs;
      TiledMap * tiled_map;
      pmPhysic pm_physic;
  };
  

};

#endif

