// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#ifndef GERDA_IMAGE_HH 
#define GERDA_IMAGE_HH 1

namespace ge {

  using namespace pm;
  using namespace std;
  using namespace tinyxml2;

  // ======= Functions for load and writing images ====================================================================
  // checker image, 4x4, GL_RED, GL_UNSIGNED_BYTE data.
  const float gl_test_image_data[] = { 1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 1.0f,
                                       0.0f, 0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f, 1.0f,
                                       1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 1.0f,
                                       0.0f, 0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f, 1.0f  };

  class Image;
  class ImageImplementation : public BaseClass {
    public:
      virtual void LoadPNG( Image * image, const string & file_name ){  }
      virtual void WritePNG(Image * image, const string & file_name ){  }
  };

  class Image : public BaseClass {
    public:
      void *data;
      string name;
      int w,h,comp,type,format;
      std::shared_ptr<ImageImplementation> image_implementation;

      Image(int width=0,int height=0,int comp_=4,int type_=0,int format_=0){ // comp=4 for RGBA
        w = width;
        h = height;
        comp = comp_;
        type   = type_;
        format = format_;
        data = nullptr;
      }

      void LoadCheck(){
        w = 4;
        h = 4;
        type   = GL_FLOAT;
        format = GL_RGBA;
        data = (void*)gl_test_image_data;
      }

      void LoadPNG( const string & file_name ){
        image_implementation->LoadPNG( this, file_name );
      }

      void WritePNG(const string & file_name){
        image_implementation->WritePNG( this, file_name );
      }

      void PrintPNG(const int & min_x, const int & min_y, const int & max_x, const int & max_y){
        int hh = min(h,min_y), ww = min(w, min_x);
        int hhh = min(h,max_y), www = min(w, max_x);
        for(int y = hh; y < hhh; y++)
          for(int x = ww; x < www; x++)
            msg(x,y, int(((unsigned char*)data)[y*comp*w + x*comp + 0]), int(((unsigned char*)data)[y*comp*w + x*comp + 1]), int(((unsigned char*)data)[y*comp*w + x*comp + 2]), int(((unsigned char*)data)[y*comp*w + x*comp + 3]));
      }

      // super slow Mirror & Flip for RGBA
      void Mirror(){
        for(int y = 0; y < h; y++){
          for(int x = 0, max = w/2; x < max; x++){
            swap( ((unsigned char*)data)[y*comp*w + x*comp + 0], ((unsigned char*)data)[y*comp*w + (w-x)*comp + 0] );
            swap( ((unsigned char*)data)[y*comp*w + x*comp + 1], ((unsigned char*)data)[y*comp*w + (w-x)*comp + 1] );
            swap( ((unsigned char*)data)[y*comp*w + x*comp + 2], ((unsigned char*)data)[y*comp*w + (w-x)*comp + 2] );
          }
        }
      }

      void Flip(){
        for(int x = 0; x < w; x++){
          for(int y = 0, max = h/2; y < max; y++){
            swap( ((unsigned char*)data)[y*comp*w + x*comp + 0], ((unsigned char*)data)[(h-y)*comp*w + x*comp + 0] );
            swap( ((unsigned char*)data)[y*comp*w + x*comp + 1], ((unsigned char*)data)[(h-y)*comp*w + x*comp + 1] );
            swap( ((unsigned char*)data)[y*comp*w + x*comp + 2], ((unsigned char*)data)[(h-y)*comp*w + x*comp + 2] );
          }
        }
      }
  };

  class STBIImageImplementation : public ImageImplementation {
    public:
      virtual void LoadPNG( Image * image, const string & file_name ){
        int & w = image->w;
        int & h = image->h;
        int & comp = image->comp;

        image->type   = GL_UNSIGNED_BYTE;
        image->format = GL_RGBA;

        MSG_DEBUG(__PFN__, "try to load file ", file_name);
        image->data   = stbi_load(file_name.c_str(), &w, &h, &comp, 0);
        MSG_DEBUG(__PFN__, file_name, "(",w,"x",h,")", image->comp, "ok");

        for(int y = 0; y < h; y++){
          for(int x = 0; x < w; x++){
            int r = (int)((unsigned char*)image->data)[y*comp*w + x*comp + 0];
            int g = (int)((unsigned char*)image->data)[y*comp*w + x*comp + 1];
            int b = (int)((unsigned char*)image->data)[y*comp*w + x*comp + 2];
            if( sys::is_rgb_alpha_mask( r,g,b ) ) 
              ((unsigned char*)image->data)[y*comp*w + x*comp + 3] = 0;
          }
        }
        image->name = file_name;
      }
      virtual void WritePNG( Image * image, const string & file_name ){
        MSG_DEBUG("Image.WritePNG(): ", file_name);
        // int stbi_write_png(char const *filename, int w, int h, int comp, const void *data, int stride_in_bytes);
        stbi_write_png(file_name.c_str(), image->w, image->h, image->comp, image->data, 0);
      }
  };

  // image factory function
  std::shared_ptr<Image> factory_image(string kind,int width=0,int height=0,int comp_=4,int type=0,int format=0){
    std::shared_ptr<Image> image = std::make_shared<Image>(width,height,comp_,type,format);
    if(kind=="STBI") image->image_implementation = std::unique_ptr<STBIImageImplementation>();
    return image;
  };

  // some slow functon to change Image content
  void image_fill_random_monochrome(Image * img, int dmin, int dmax){
    /// change no-zero-alpha RGB pixel value to random(dmin, dmax), dmin in [0,255], dmax in [0,255], slow
    int h = img->h, w = img->w, comp = img->comp;
    dmin = min(max(dmin, 0), 255);
    dmax = min(max(dmin, 0), 255);
    for(int y = 0; y < h; y++){
      for(int x = 0; x < w; x++){
        if( int(((unsigned char*)img->data)[y*comp*w + x*comp + 3]) == 0) continue;
        unsigned char val = randint(dmin, dmax);
        ((unsigned char*)img->data)[y*comp*w + x*comp + 0] = val;
        ((unsigned char*)img->data)[y*comp*w + x*comp + 1] = val;
        ((unsigned char*)img->data)[y*comp*w + x*comp + 2] = val;
      }
    }
  }

  bool check_if_alpha_pixels_around(Image * img, int x, int y){
    /// check points (x-1,y),(x+1,y),(x,y-1),(x,y+1) around (x,y) for pixels with alpha!=0, unsafe
    int w = img->w, comp = img->comp;
    for(int xx = x-1; xx <= x+1; xx+=2)
      if( int(((unsigned char*)img->data)[y*comp*w + xx*comp + 3]) != 0) return true;
    for(int yy = y-1; yy <= y+1; yy+=2)
      if(((unsigned char*)img->data)[yy*comp*w + x*comp + 3] != 0) return true;
    return false;
  }

  void image_set_black_edges(Image * img){
    /// turn (r,g,b,0) pixels to (0,0,0,255) if check_if_alpha_pixels_around()==true
    int h = img->h, w = img->w, comp = img->comp;
    for(int y = 1; y < h-1; y++){
      for(int x = 1; x < w-1; x++){
        if( int(((unsigned char*)img->data)[y*comp*w + x*comp + 3]) != 0) continue;
        if( not check_if_alpha_pixels_around(img, x, y) ) continue;
        ((unsigned char*)img->data)[y*comp*w + x*comp + 0] = (unsigned char)0;
        ((unsigned char*)img->data)[y*comp*w + x*comp + 1] = (unsigned char)0;
        ((unsigned char*)img->data)[y*comp*w + x*comp + 2] = (unsigned char)0;
        ((unsigned char*)img->data)[y*comp*w + x*comp + 3] = (unsigned char)255;
      }
    }
  }

}  
#endif







