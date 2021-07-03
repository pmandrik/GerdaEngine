// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
/// create TiledMap, fill it by hand, draw with opengl quads
/// create some Objects, check intersections with map

#include "GerdaEngine.hh"
#include "set"
using namespace ge;

int main(){
  sys::set_windows_size(600, 1000);
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
  map.Fill(6, 6, size_x-6, size_y-6, 0, 1);
  map.Fill(25, 15, 30, 30, 0, 0);
  map.Fill(36, 25, 40, 31, 0, 0);
  
  TiledPhysic physic = TiledPhysic( & map );
  physic.pm_physic.check_solid_tiles_of_body_points = true;
  physic.pm_physic.tune_collision_speed = true; // false

  for(int i = 0; i < 50; i++){
    msg( physic.msize_layers );
    pmObject * object = physic.Create();
    object->pos   = v2( sys::WW2, sys::WH2 );
    object->speed = v2( randrange(-1, 1), randrange(-1, 1) );
    object->size = v2(30 + randint(-10, 10), 30 + randint(-10, 10));
    object->angle = randint(0, 90);
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

    set<int> interacts;
    for(int i = 0; i < physic.interacted_object_x_object_pairs.size(); i++){
      std::pair<int, int> p = physic.interacted_object_x_object_pairs[i];
      interacts.insert( physic.objects[ p.first ]->id );
      interacts.insert(  physic.objects[ p.second ]->id );
      v2 dv = physic.objects[ p.first ]->pos - physic.objects[ p.second ]->pos;
      physic.objects[ p.first ]->speed  += dv * 0.01;
      physic.objects[ p.second ]->speed -= dv * 0.01;
    }
    
    for(auto object : physic.objects){
      object->speed += v2(sys::keyboard->hor, sys::keyboard->ver);
      // if(object->speed.x and object->speed.y) msg( object->angle );

      if( object->interact_with_solid ) ge::glColor3f(1.,1.,0.25);
      else if( interacts.count( object->id ) ) ge::glColor3f(1.,1.,0.25);
      else ge::glColor3f(0.75,0.75,0.75);
      draw_quad(object->pos, object->size, object->angle);
      // object->angle += 1;
    }
    
    if(sys::mouse->left_hold) msg(sys::mouse->x, sys::mouse->y);

    ge::glColor3f(1.0,1.0,1.0);
    map.DrawSlow(0,0);
  }
};


