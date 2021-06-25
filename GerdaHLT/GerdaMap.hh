// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#ifndef GerdaMap_H
#define GerdaMap_H 1 

namespace ge {
  
  class TiledMap : public BaseClass, public pmTiledMap {
    /// pmTiledMap work with map of integer of sizes: map_size_x * map_size_y * map_size_types
    /// mean, multiple layers of tiles with different short type
    // pmTiledMap:
    // int msize_x, msize_y, msize_layers, msize_layers_x_msize_y;
    // int tsize, bullet_grid_size;
    // int * mdata;
    public:

  };
};

#endif
