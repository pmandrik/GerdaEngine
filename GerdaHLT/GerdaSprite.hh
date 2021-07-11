// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GerdaSprite_H
#define GerdaSprite_H 1

namespace ge {

class ImageLine {
  public:
  /// Image mean part of Texture represented as TexTile - coordinated and sizes of rectangle
  /// ImageLine - series of Texture parts
  ImageLine() : empty(true) { Reset(); }
  ~ImageLine(){}

  void Reset(bool reverse = false){ 
    if(reverse) image = images.size()-1;
    else        image = 0;
    time = 0; get_end = false; 
  }

  void Add(TexTile * image, int time){
    images.push_back(image);
    times.push_back(time);
    empty = false;
  }

  const void Print() const {
    msg("ImageLine(");
    for(vector <TexTile*>::const_iterator it = images.begin(); it != images.end(); it++) msg( **it );
    msg(")ImageLine");
  }

  TexTile * Get(bool reverse = false){
    TexTile * answer = images[image];
    if((++time) >= times[image]){
      if(reverse){
        if((--image) < 0){
          image = images.size()-1;
          get_end = true;
        }
      }
      else{
        if((++image) >= images.size()){
          image = 0;
          get_end = true;
        }
      }
      time = 0;
    }
    return answer;
  }

  vector <TexTile*> images;
  vector <int> times;
  int time, image;
  bool get_end, empty;
};

class Sprite : public DrawableQuad {
  public:
  /// Sprite is used to control and show single Image as well as ImageLine
  /// pos, size, angle and if it is flip
  /// Sprites are Quads or 4 point vertexes
  Sprite(Sprite * sprite){
    active_state = "";

    flip   = sprite->flip;
    angle  = sprite->angle;
    size   = sprite->size;
    pos    = sprite->pos;
    pos.z  = sprite->pos.z;
    ttile = nullptr;
    drawer       = nullptr;
    draw_id      = -1;
    if(sprite->drawer) SetDrawer(sprite->drawer);

    states = sprite->states;
    if(states.size()){
      active_line = &(states.begin()->second);
      ttile = active_line->Get();
    }
    else active_line = nullptr;
  }

  Sprite(){
    active_state = "";

    flip   = 0;
    angle  = 0;
    size   = v2();
    pos    = v2();
    pos.z  = 1 * 0.1;
    ttile = nullptr;
    drawer       = nullptr;
    draw_id = -1;
  }

  Sprite( TiledMapObject* obj, TexTile * tile ) {
    flip  = obj->flip;
    angle = obj->angle;
    size  = obj->size * 0.5;
    pos   = obj->pos;
    pos.z = 0.5 + obj->level * 0.1;
    ttile = tile;
    drawer  = nullptr;
    draw_id = -1;
  }

  ~Sprite(){
    if(drawer) drawer->Remove(draw_id);
  }

  // IMAGE PART
  void AddLine(const char * str, ImageLine * line){
    states[string(str)] = *line;
    if( not active_state.size() ){ 
      active_state = string(str);
      active_line = & states.find(string(str))->second;
      ttile = active_line->Get();
      active_line->Reset();
    }
  }

  TexTile * Get(string state="", bool circle = true, bool reverse = false){
    if((active_state == state or state == "") and active_line){
      if( circle or not active_line->get_end) ttile = active_line->Get(reverse);
    }
    else{
      if(active_line) active_line->Reset();
      auto find = states.find(state);
      if(find == states.end()){
        msg("Sprite : get() : bad state name ", state);
        return ttile;
      }
      active_line = & find->second;
      active_state = state;
      ttile = active_line->Get(reverse);
    }
    return ttile;
  }

  bool HasState(string state){
    auto find = states.find(state);
    if(find == states.end()) return false;
    return true;
  }

  inline void ResetActive(bool reverse = false){ 
    active_line->Reset(reverse); 
    SetImageIndex( 0 );
  }
  
  void SetImageIndex(const int  & index){
    if(not active_line) return;
    if(index < 0 or index > active_line->images.size()) return;
    ttile = active_line->images.at(index);
  }
  
  ImageLine * active_line;
  string active_state;
  map <string, ImageLine> states;
};







  // Help functions
  Sprite * create_sprite(Texture * texture, string key, ArrayQuadsDrawer * drawer){
    Sprite * sprite = new Sprite();
    sprite->SetDrawer( drawer );
    TexTile * ttile = texture->GetTexTile(key);
    sprite->ttile = ttile;
    return sprite;
  }
/*
   Sprite * create_sprite(ArrayQuadsDrawer * drawer, v2 size){
    Sprite * sprite = new Sprite();
    sprite->size = size*0.5;
    if(drawer) sprite->SetDrawer( drawer );
    return sprite;
  }

  
  Sprite * load_sprite(Texture * texture, string anim_name, v2 size, v2 stride, vector<v2> poss){
    Sprite * sprite = load_sprite(nullptr, size);

    ImageLine * anim_line = new ImageLine();
    for(int i = 0; i < poss.size(); i++){
      anim_line->Add(new TexTile( poss.at(i) * size + stride, size - 2*stride, texture->size, texture ), 0);
    }
    sprite->AddLine(anim_name.c_str(), anim_line);

    return sprite;
  }

  Sprite * create_sprite(Texture * texture, string anim_name, v2 size, v2 stride, vector<v2> poss, vector<int> times){
    Sprite * sprite = load_sprite(nullptr, size);

    ImageLine * anim_line = new ImageLine();
    for(int i = 0; i < poss.size(); i++){
      anim_line->Add(new TexTile( poss.at(i) * size + stride, size - 2*stride, texture->size, texture ), times.at(i));
    }
    sprite->AddLine(anim_name.c_str(), anim_line);

    return sprite;
  }
*/

}

#endif 
