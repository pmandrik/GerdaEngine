// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
/// create TiledMap, fill it by hand, draw with opengl quads
/// create some Objects, check intersections with map

#include "GerdaEngine.hh"
using namespace ge;

struct Object {
  v2 pos, size;
  float angle;
};

vector<Object> objects;

int main(){
  init_sdl();
  init_sdl_screen();
  init_opengl();
  init_gerda();

  int tile_size = 10;
  int size_x = sys::WW / tile_size;
  int size_y = sys::WH / tile_size;
  int * map_data = new int[size_x * size_y * 1];
  TiledMap map = TiledMap(size_x, size_y, 1, 10, map_data);
  map.Fill(0, 0, size_x, size_y, 0, 0);
  map.Fill(2, 2, size_x-2, size_y-2, 0, 1);

  Timer timer = Timer(90);
  bool game = true;
  while( game ){
    tick_gerda();
    game = not check_exit();
    
    sys::camera->Tick();
    sys::camera->SetTarget(v2(-sys::WW2, -sys::WH2));
    sys::camera->Resize(sys::mouse->wheel);

    for(auto object : objects){
      draw_quad(object.pos, object.size, object.angle);
    }

    map.DrawSlow(0,0);
  }
};


