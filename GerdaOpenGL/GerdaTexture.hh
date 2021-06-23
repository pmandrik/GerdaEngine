// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef  GerdaTexture_H
#define GerdaTexture_H 1

namespace ge {

  const int MIP_LEVELS_FIXME = 5;

  // tile in normalised coordinates of texture
  class Texture;
  struct TexTile {
    TexTile() {}
    TexTile(v2 p, v2 s, Texture * owner) : pos(p), size(s), texture(owner) {
		  tsize = v2(1,1);
      tpos = v2();      
    }
    TexTile(v2 p, v2 s, v2 asize, Texture * owner) : pos(p), size(s), texture(owner) {
		  tsize = size / asize;
      tpos = pos / asize;
    }

    v2 pos, size, tpos, tsize;
    Texture * texture;
  };

  ostream & operator << (ostream & out, TexTile t){return out << "TextureTile( " << t.pos << " " << t.size << " " << t.tpos << " " <<t.tsize << " ) ";};

  class Texture {
    public :
    Texture():image(nullptr){}
    Texture(Image * img){
      glGenTextures(1, &id);
      image = img;
      
      w = image->w;
      h = image->h;
      size = v2(w, h);

      Bind();
      glEnable(GL_TEXTURE_2D);
      // glTexStorage2D(GL_TEXTURE_2D, MIP_LEVELS_FIXME, GL_RGBA, w, h);
      // void glTexImage2D(target, level, internalFormat, width, height, border, format, type, data);
      glTexImage2D_err(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, image->format, image->type, image->data);
      // void glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, const void *data);
      // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, image->format, image->type, image->data);
      glGenerateMipmap(GL_TEXTURE_2D);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      Unbind();

      atlas[ "" ] = TexTile(v2(0, 0), size, size, this);
    };
    

    inline void Bind()  { glBindTexture(GL_TEXTURE_2D, id); }
    inline void Unbind(){ glBindTexture(GL_TEXTURE_2D,  0); }

    void Draw(v2 pos=sys::WV2, v2 size=sys::WV2, v2 tpos=v2(0., 0.), v2 tsize=v2(1., 1.)){
      Bind();
      glBegin(GL_QUADS);
      glTexCoord2f(tpos.x + tsize.x, 	tpos.y + tsize.y);  glVertex3f( pos.x + size.x, pos.y + size.y, 0);
      glTexCoord2f(tpos.x,            tpos.y + tsize.y);  glVertex3f( pos.x - size.x, pos.y + size.y, 0);
      glTexCoord2f(tpos.x,            tpos.y);            glVertex3f( pos.x - size.x, pos.y - size.y, 0);
      glTexCoord2f(tpos.x + tsize.x, 	tpos.y);            glVertex3f( pos.x + size.x, pos.y - size.y, 0);
      glEnd();
      Unbind();
    }

    TexTile * GetTexTile(const string & key){
      auto ptr = atlas.find(key);
      if(ptr == atlas.end()) return nullptr;
      return &(ptr->second);
    }

    void AddTexTile(const string & key, const v2 & p, const v2 & s){ 
      if(GetTexTile(key)) return;
      atlas[ key ] = TexTile(p, s, size, this);
    }

    /*public :
    Texture(){};
    Texture(string filename) {
      ALLEGRO_BITMAP * bmap = load_bitmap(filename);
      ChangeBitmap(bmap);
    }
    Texture(ALLEGRO_BITMAP * bmap) {
      ChangeBitmap(bmap);
    }
    ~Texture(){
      atlas.clear();
    }

    void ChangeBitmap(ALLEGRO_BITMAP * bmap){
      if(not bmap){
        msg_err("Texture:change_bitmap(): null bitmap, skip");
        return;
      }
      al_convert_mask_to_alpha(bmap, al_map_rgba(0, 255, 0, 255));
      al_convert_mask_to_alpha(bmap, al_map_rgba(255, 0, 0, 255));

      bitmap = bmap;
      id = al_get_opengl_texture(bmap);
      w = al_get_bitmap_width( bmap );
      h = al_get_bitmap_height( bmap );
      size = v2(w, h);
      center = v2(w/2, h/2);

      Bind();
      glEnable(GL_TEXTURE_2D);
		  //glGenerateMipmap(GL_TEXTURE_2D);  // TODO FIXME PLEASE
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      Unbind();
    }

    void Draw(v2 pos=sys::dv2, v2 size=sys::dv2, v2 tpos=v2(0., 0.), v2 tsize=v2(1., 1.)){
      Bind();
      glBegin(GL_QUADS);
      glTexCoord2f(tpos.x + tsize.x, 	tpos.y + tsize.y);  glVertex3f( pos.x + size.x, pos.y + size.y, 0);
      glTexCoord2f(tpos.x,            tpos.y + tsize.y);  glVertex3f( pos.x - size.x, pos.y + size.y, 0);
      glTexCoord2f(tpos.x,            tpos.y);            glVertex3f( pos.x - size.x, pos.y - size.y, 0);
      glTexCoord2f(tpos.x + tsize.x, 	tpos.y);            glVertex3f( pos.x + size.x, pos.y - size.y, 0);
      glEnd();
      Unbind();
    }

    inline void Target(){ al_set_target_bitmap( bitmap );   }
    inline void AddToAtlas(string key, v2 p, v2 s){ atlas[ string(key) ] = TextureTile(p, s, size, string(key));}
    TextureTile * GetFromAtlas(string key){
      find_atlas = atlas.find(key);
      if(find_atlas == atlas.end()){ msg_err("texture : get_from_atlas : cant find ", key, ", return NULL"); return NULL;}
      return &(find_atlas->second);
    }
    TextureTile * GetFromAtlasSilent(string key){
      find_atlas = atlas.find(key);
      if(find_atlas == atlas.end()) return NULL;
      return &(find_atlas->second);
    }

    ALLEGRO_BITMAP * bitmap;
    int id;
    float w,h;
    v2 size, center;
    map<string, TextureTile> atlas;
    map<string, TextureTile>::iterator find_atlas;
  */

    map<string, TexTile> atlas;
    v2 size;

    GLuint id;
    double w,h;
    Image * image;
  };

  // simple interface to load RGBA PNG images
  Texture * load_texture_with_image(string file_name){
    Image * img = new Image();
    img->LoadPNG( file_name );
    Texture * text = new Texture( img );
    return text;
  }
}
#endif 