// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GerdaHLT_H
#define GerdaHLT_H 1

#include  "GerdaSprite.hh"
#include  "GerdaDialog.hh"
#include  "GerdaMap.hh"
#include  "GerdaObject.hh"
#include  "GerdaParticle.hh"
#include  "GerdaPhysic.hh"
#include  "GerdaDataContainer.hh"
#include  "GerdaSLa.hh"
#include  "GerdaScene.hh"

namespace ge{
  class ScreenSaver{
    public:
    bool flip = false;
    int frame_index = 0;
    int frame_rate  = 2;
    int N_zeros = 10;
    string prefix = "screen", postfix = ".png";

    string GetName(){
      return prefix + add_leading_zeros(frame_index, N_zeros) + postfix;
    }

    void DumpOpenGLScreen() {
      if(not (frame_index % frame_rate)){
        string out_name = GetName();
        ge::glReadPixels(0, 0, ge::sys::WW, ge::sys::WH, GL_RGBA, GL_UNSIGNED_BYTE, ge::sys::screenshoot_image->data);
        if( flip ) ge::sys::screenshoot_image->Flip();
        ge::sys::screenshoot_image->WritePNG( out_name );
      }
      frame_index++;
    };
  };
};

#endif
