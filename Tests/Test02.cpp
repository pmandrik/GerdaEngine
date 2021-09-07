// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
/// load Image, create texture, create texture atlas, make static sprites
/// create array quad drawer, draw

#include "GerdaEngine.hh"
#include "set"
using namespace ge;

int main(){
  sys::set_windows_size(600, 1000);
  init_sdl();
  init_sdl_screen();
  init_opengl();
  init_gerda();

  Image * img = new Image();
  img->LoadPNG("Data/tiles.png");
  Texture * texture = new Texture(img);
  add_atlas_tiled(texture,9,3,-1,-1,1);
  
  ArrayQuadsDrawer * drawer = new ArrayQuadsDrawer();
  
  vector<Sprite*> sprites;
  for(int i = 0; i < 5000; i++){
    string key = "X" + to_string(randrange(0,8)) + "Y" + to_string(randrange(0,2));
    Sprite * sprite = new Sprite();
    TexTile * ttile = texture->GetTexTile(key);
    // if(not ttile) { msg(key); continue; }
    sprite->ttile = ttile;
    sprite->pos = v2(300,500);
    sprite->size = v2(10,10);
    sprite->SetDrawer( drawer );
    sprites.push_back(sprite);
  }

  msg("Loop ...");
  Timer timer = Timer(90);
  bool game = true;
  while( game ){
    tick_gerda();
    game = not check_exit();
    
    sys::camera->Tick();
    sys::camera->SetTarget(v2(-sys::WW2, -sys::WH2));
    sys::camera->Resize(sys::mouse->wheel);
    
    for(int i = 0; i < sprites.size(); i++){
      float dx = 2*cos(timer.ftime * 3.14 + i*0.5 + 0.001*(sprites.at(i)->pos.y-300));
      float dy = 2*sin(sprites.at(i)->pos.x*0.01* + dx + timer.ftime * 3.14 * 0.4 + i*0.25);
      sprites.at(i)->pos += v2(dx, dy);
      if(not Rect(0,0,600,1000).Contain( sprites.at(i)->pos )) sprites.at(i)->pos = v2(300, 500);
      sprites.at(i)->size = v2(5,5)*abs(abs(dy));
      sprites.at(i)->SendChanges();
    }
    
    texture->Bind();
    drawer->Draw();
    texture->Unbind();
  }
};


