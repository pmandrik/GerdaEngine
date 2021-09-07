// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
/// load Image, create texture, create texture atlas, create animation line, make sprites
/// create array quad drawer, draw on framebuffer
/// apply shader, draw from framebuffer on screen

#include "GerdaEngine.hh"
#include "set"
using namespace ge;

int main(){
  sys::set_windows_size(600, 1000);
  init_sdl();
  init_sdl_screen();
  init_opengl();
  init_gerda();

  sys::alpha_color_3 = rgb(1,1,0);
  Texture * texture = load_texture_with_image("Data/sprite.png");

  add_atlas_xline(texture,"walk",4,1,1,53,115,1);
  ImageLine * image_line = create_imageline(texture, "walk", 4, 0, 40);
  ArrayQuadsDrawer * drawer = new ArrayQuadsDrawer();
  Sprite * sprite_0 = create_sprite(drawer, {"walk"}, { image_line } );
  vector<Sprite*> sprites;
  for(int i = 0; i < 4500; i++){
    Sprite * sprite = copy_sprite( sprite_0 );
    sprite->pos = v2(randrange(-300,300), randrange(-1200,1200));
    sprite->size = v2(100,100);
    sprite->Get("walk");
    sprite->active_line->SetRandom();
    sprites.push_back(sprite);
    sprites.at(i)->angle = randrange(-305, 305);
  }

  FrameBuffer     * buffer_main = new FrameBuffer();
  buffer_main->Clear();
  PairFrameBuffer * buffer_mix  = new PairFrameBuffer();
  FrameShader* shader_mix = load_frame_shader("main", "Data/default_shaders/shader_FB_def.vert", "Data/default_shaders/shader_FB_mix.frag", 2, 2);
  shader_mix->vars[ 0 ] = 0.01;
  shader_mix->vars[ 1 ] = 0.985;

  msg("Loop ...");
  Timer timer = Timer(900);
  bool game = true;
  while( game ){
    tick_gerda();
    game = not check_exit();
    timer.Tick();
    
    for(int i = 0; i < sprites.size(); i++){
      sprites.at(i)->Get("walk");
      
      if(sprites.at(i)->pos.y < -1200) sprites.at(i)->pos.y += 2400;
      if(sprites.at(i)->pos.y >  1200) sprites.at(i)->pos.y -= 2400;
      if( sprites.at(i)->active_line->get_next ) sprites.at(i)->pos.y -= 1*abs(0.95 - 0.8 * (sprites.at(i)->pos.y+600)/1000.);
      sprites.at(i)->pos.y += 0.5*abs(0.75 - 0.0 * (sprites.at(i)->pos.y+600)/1000.) + 0.01*cos(3.14 * i * 0.01);
      sprites.at(i)->pos.z = 1. - (sprites.at(i)->pos.y+600)/1000.;
      if( i > sprites.size()-500 ) sprites.at(i)->pos.z = 1.7 - (sprites.at(i)->pos.y+600)/1000.;
      sprites.at(i)->size   = v2(53,115)*(0.65 - 0.5 * (sprites.at(i)->pos.y+600)/1000.);
      sprites.at(i)->angle += 0.001*cos(2*3.14 * i * 0.01 + timer.ftime * 3.14);
      sprites.at(i)->SendChanges();
    }

    // draw sprites on main buffer 
    //buffer_main->Clear();
        buffer_main->Target();
        ge::glClearColor(64/255./2,81/255./2,122/255./2,sys::fb_def_color.a);
        ge::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        buffer_main->Untarget();
    buffer_main->Target();   
    texture->Bind();
    drawer->Draw();
    texture->Unbind();
    buffer_main->Untarget();

    // draw main buffer + old buffer into final buffer
    draw_2fb_with_shader(shader_mix, buffer_main, buffer_mix->back, buffer_mix->active);

    // draw final buffer on screen
    buffer_mix->active->DrawOnWindow(false, false);

    // swap final and old buffers
    buffer_mix->Flip();

    sys::camera->Tick();
    sys::camera->Resize(sys::mouse->wheel);
  }
};


