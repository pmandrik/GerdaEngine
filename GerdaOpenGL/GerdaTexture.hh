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

  class Texture : BaseClass {
    public :
    Texture():image(nullptr){}
    Texture(Image * img){
      MSG_INFO( __PFN__, "from Image", img );
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

    void Draw(v2 pos=sys::WV2, v2 size=sys::WV2, v2 tpos=v2(0., 0.), v2 tsize=v2(1., 1.), float z_pos=0, bool flip_x=false, bool flip_y=false, const float & angle=0){
      if(flip_x) size.x *= -1;
      if(flip_y) size.y *= -1;

      v2 perp = v2(-size.x, size.y);
      size = size.Rotated(angle);
      perp = perp.Rotated(angle);

      Bind();
      glBegin(GL_QUADS);
      glTexCoord2f(tpos.x, 	tpos.y);                      glVertex3f(pos.x - size.x, pos.y - size.y, z_pos); 
	    glTexCoord2f(tpos.x + tsize.x, 	tpos.y);            glVertex3f(pos.x - perp.x, pos.y - perp.y, z_pos);
	    glTexCoord2f(tpos.x + tsize.x, 	tpos.y + tsize.y);  glVertex3f(pos.x + size.x, pos.y + size.y, z_pos);
	    glTexCoord2f(tpos.x, 	tpos.y + tsize.y);            glVertex3f(pos.x + perp.x, pos.y + perp.y, z_pos);
/*
      glTexCoord2f(tpos.x + tsize.x, 	tpos.y + tsize.y);  glVertex3f( pos.x + size.x, pos.y + size.y, z_pos);
      glTexCoord2f(tpos.x,            tpos.y + tsize.y);  glVertex3f( pos.x - size.x, pos.y + size.y, z_pos);
      glTexCoord2f(tpos.x,            tpos.y);            glVertex3f( pos.x - size.x, pos.y - size.y, z_pos);
      glTexCoord2f(tpos.x + tsize.x, 	tpos.y);            glVertex3f( pos.x + size.x, pos.y - size.y, z_pos);
*/
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

    string GenerateAtlasGridName(int x, int y) const {
      return "X" + to_string(x) + "Y" + to_string(y);
    };
    
    string GenerateAtlasName(string name, int x, int y) const {
      return name + "_" + to_string(x) + "_" + to_string(y);
    };

    map<string, TexTile> atlas;
    v2 size;

    GLuint id;
    double w,h;
    Image * image;
  };

  Texture * load_texture_with_image(string file_name){
    /// simple interface to load RGBA PNG images
    Image * img = new Image();
    img->LoadPNG( file_name );
    Texture * text = new Texture( img );
    return text;
  }
  
  void add_atlas_tiled(Texture * text, int nx, int ny, int dx=0, int dy=0, int border_width=0){
    /// default splitting of the texture based on grid with nx*ny tiles
    /// aligns - distances from borders in pixels
    /// keys: X0Y0, X1Y0, X2Y0, ... , XNY0, XNY1, XNY2, ..., XNYN
    if( not text ){
      msg_err(__PFN__, "texture is not defined, return");
      return;
    }

    if( not text->image ){
      msg_err(__PFN__, "text->image is not defined, return");
      return;
    }
    
    int w = text->image->w-border_width;
    int h = text->image->h-border_width;
    if( w % nx or h % ny){
      msg(__PFN__, "incorrect image splitting", w,"%", nx, "or", h, "%", ny, "!=0");
    }
    
    int tile_size_x = w / nx - dx - 2*border_width; 
    int tile_size_y = h / ny - dy - 2*border_width;
    for(int x = 0; x < nx; x++){
      for(int y = 0; y < ny; y++){
        string key = text->GenerateAtlasGridName(x, y);
        text->AddTexTile(key, v2(border_width + tile_size_x*x + dx*x, border_width + tile_size_y*y + dy*y), v2(tile_size_x, tile_size_y));
      }
    }
  }

  void add_atlas_line(Texture * text, string name, int nx, int ny, int start_x, int start_y, int width_x, int width_y, int border_width){
    if( not text->image ){
      msg_err(__PFN__, "text->image is not defined, return");
      return;
    }

    if(nx==0 and ny==0) return;
    if(nx==0) nx++;
    if(ny==0) ny++;

    int dx = border_width + width_x;
    int dy = border_width + width_y;
    for( int x = 0; x < nx; x++ ) { 
      for( int y = 0; y < ny; y++ ) { 
        string key = text->GenerateAtlasName(name, x, y);
        msg( key );
        text->AddTexTile(key, v2(start_x + border_width + dx * x, start_y + border_width + width_y + dy * y), v2(width_x, width_y));
      }
    }
  }

  void add_atlas_xline(Texture * text, string name, int n, int start_x, int start_y, int width_x, int width_y, int border_width){
    add_atlas_line(text, name, n, 0, start_x, start_y, width_x, width_y, border_width);
  }

  void add_atlas_yline(Texture * text, string name, int n, int start_x, int start_y, int width_x, int width_y, int border_width){
    add_atlas_line(text, name, 0, n, start_x, start_y, width_x, width_y, border_width);
  }
}

#endif 









