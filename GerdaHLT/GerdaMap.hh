// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#ifndef GerdaMap_H
#define GerdaMap_H 1 

namespace ge {
  
  /// pmTiledMap work with map of integer of sizes: map_size_x * map_size_y * map_size_layers  
  /// map_size_layers mean multiple layers of tiles with different int type 
  /// value 0 of 0 layer is reserved for Solid tiles  
  class TiledMap : public BaseClass, public pmTiledMap {
    // pmTiledMap:
    // int msize_x, msize_y, msize_layers, msize_layers_x_msize_y;
    // int tsize, bullet_grid_size;
    // int * mdata;
    public:
      TiledMap( const int & map_size_x, const int & map_size_y, const int & map_size_layers, const int & tile_size, int * map_data ) : BaseClass(), pmTiledMap( map_size_x, map_size_y, map_size_layers, tile_size, map_data) {
      }

      void DrawSlow(int layer=0, int mask=0, float z_pos = 0.){
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

  /// basic 
  class TiledPhysic : public BaseClass, public pmPhysicContainer {
    public:
      TiledPhysic(TiledMap * map){
        SetMap( map );
      }
      
      void SetMap(TiledMap * map_){
        pmPhysicContainer::msize_layers = map_->msize_layers;
        pm_physic.SetMap( (pmTiledMap*)map_ );
        map = map_;
      }
    
      void Tick(){
        interacted_object_x_object_pairs.clear();
        interacted_bullet_x_object_pairs.clear();
        pm_physic.Tick( objects, particles, interacted_object_x_object_pairs, interacted_bullet_x_object_pairs);
      }
      
      std::vector< std::pair<int, int> > interacted_object_x_object_pairs;
      std::vector< std::pair<int, int> > interacted_bullet_x_object_pairs;
      TiledMap * map;
      pmPhysic pm_physic;
  };
  

};

#endif

