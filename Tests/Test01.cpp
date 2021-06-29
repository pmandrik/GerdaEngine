// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
/// create TiledMap, fill it by hand, draw with opengl quads
/// create some Objects, check intersections with map

#include "GerdaEngine.hh"
using namespace ge;

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
  
  TiledPhysic physic = TiledPhysic( & map );
  physic.pm_physic.check_solid_tiles_of_body_points = true;
  physic.pm_physic.tune_collision_speed = true;

  for(int i = 0; i < 1; i++){
    msg( physic.msize_layers );
    pmObject * object = physic.Create();
    object->pos   = v2( randint(tile_size*6, sys::WW-tile_size*6), randint(tile_size*6, sys::WH-tile_size*6) );
    object->speed = v2( randrange(-1, 1), randrange(-1, 1) );
    object->size = v2(30, 30);
    object->angle = 0;//randrange(0, 360);
    msg( object->pos );
  }

  Timer timer = Timer(90);
  bool game = true;
  while( game ){
    tick_gerda();
    game = not check_exit();
    
    sys::camera->Tick();
    sys::camera->SetTarget(v2(-sys::WW2, -sys::WH2));
    sys::camera->Resize(sys::mouse->wheel);

    physic.Tick();
    
    for(auto object : physic.objects){
      object->speed += v2(sys::keyboard->hor, sys::keyboard->ver);
      if(object->speed.x and object->speed.y) msg( object->angle );
      draw_quad(object->pos, object->size, object->angle);
    }
    
    if(sys::mouse->left_hold) msg(sys::mouse->x, sys::mouse->y);

    map.DrawSlow(0,0);
  }
};


