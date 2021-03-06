// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GerdaOpengl_H
#define GerdaOpengl_H 1

namespace ge {
  // ======= OPENGL FUNCTION LOAD VIA SDL ====================================================================
  #define SDL_PROC(ret,func,params) typedef ret (APIENTRY * GL_ ## func ## _Func) params;
  #include "SDL_glfuncs.h"
  #undef SDL_PROC

  #define SDL_PROC(ret,func,params) GL_ ## func ## _Func func = nullptr;
  #include "SDL_glfuncs.h"
  #undef SDL_PROC

  bool load_opengl_context(){
    msg("load_opengl_context() ... ");
    #if SDL_VIDEO_DRIVER_UIKIT
    #define __SDL_NOGETPROCADDR__
    #elif SDL_VIDEO_DRIVER_ANDROID
    #define __SDL_NOGETPROCADDR__
    #elif SDL_VIDEO_DRIVER_PANDORA
    #define __SDL_NOGETPROCADDR__
    #endif

    #if defined __SDL_NOGETPROCADDR__
    #define SDL_PROC(ret,func,params) ge::##func=func;
    #else
    #define SDL_PROC(ret,func,params) \
        do { \
            ge::func = (GL_ ## func ## _Func) SDL_GL_GetProcAddress(#func); \
            msg(#func, ge::func); \
            if ( ge::func == nullptr ) { \
                msg_err("Couldn't load GL function", #func, SDL_GetError());\
                return 0;\
            } \
        } while ( 0 );
    #endif /* __SDL_NOGETPROCADDR__ */

    #include "SDL_glfuncs.h"
    #undef SDL_PROC
    return 1;
  }

  // ======= OPENGL ERROR CHECK WRAPPERS ====================================================================
  void gl_check_error(const char * fname){
    GLenum code;
    while((code = glGetError()) != GL_NO_ERROR){
      msg_err(fname, " ERROR ", code);
    }
  }

  void glTexImage2D_err(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * pixels){
    glTexImage2D( target, level, internalformat, width, height, border, format, type, pixels);
    gl_check_error("glTexImage2D");
  }
}

#include "GerdaCamera.hh"
#include "GerdaTexture.hh"
#include "GerdaDrawPrimitives.hh"
#include "GerdaShader.hh"
#include "GerdaFrameBuffer.hh"

namespace ge {
  // High level functions for drawing of OpenGL primitives ============== ============== ============== ============== ============== ==============
  // Texture to FB ==============
  void draw_Text_to_FB(Texture * text, v2 & text_pos, v2 & text_size, FrameBuffer * fb_target, float z_level = 0, float angle = 0, FrameShader * shader=nullptr){
    if(shader){
      shader->Bind();
      shader->UpdateUniforms();
    }
    fb_target->Target();
    text->Draw(text_pos, text_size, v2(0., 0.), v2(1., 1.), z_level, false, false, angle);
    fb_target->Untarget();
    if(shader) shader->Unbind();
  };

  void draw_Text_to_Screen(Texture * text, v2 & text_pos, v2 & text_size, float z_level = 0, float angle = 0, FrameShader * shader=nullptr){
    if(shader){
      shader->Bind();
      shader->UpdateUniforms();
    }
    text->Draw(text_pos, text_size, v2(0., 0.), v2(1., 1.), z_level, false, false, angle);
    if(shader) shader->Unbind();
  };

  // FB to screen ==============
  void draw_FB_to_Screen(FrameBuffer * fb, float z_level = 0, FrameShader * shader=nullptr){
    if(shader){
      shader->Bind();
      shader->UpdateUniforms();
    }
    fb->BindTexture(0);
    fb->DrawQuad(-sys::WW2, -sys::WH2, sys::WW2, sys::WH2, z_level);
    fb->UnbindTexture(0);
    if(shader) shader->Unbind();
  }

  // FB to FB ==============
  void draw_FB_to_FB(FrameBuffer * fb, FrameBuffer * fb_target, float z_level = 0, FrameShader * shader=nullptr){
    if(shader){
      shader->Bind();
      shader->UpdateUniforms();
    }
    fb_target->Target();
    fb->BindTexture(0);
    fb->DrawQuad(-sys::FBW2, -sys::FBH2, sys::FBW2, sys::FBH2, z_level);
    fb->UnbindTexture(0);
    fb_target->Untarget();
    if(shader) shader->Unbind();
  }

  // FB+FB to FB ==============
  void draw_2FB_to_FB(FrameBuffer * fb1, FrameBuffer * fb2, FrameBuffer * fb_target, float z_level = 0, FrameShader * shader=nullptr){
    if(shader){
      shader->Bind();
      shader->UpdateUniforms();
    }
    fb_target->Target();
    fb1->BindTexture(0);
    fb2->BindTexture(1);
    fb1->DrawQuad(-sys::FBW2, -sys::FBH2, sys::FBW2, sys::FBH2, z_level);
    fb1->UnbindTexture(0);
    fb2->UnbindTexture(1);  
    glActiveTexture(GL_TEXTURE0);
    fb_target->Untarget();
    if(shader) shader->Unbind();
  }

  void draw_2FB_to_Screen(FrameBuffer * fb1, FrameBuffer * fb2, float z_level = 0, FrameShader * shader=nullptr){
    if(shader){
      shader->Bind();
      shader->UpdateUniforms();
    }
    fb1->BindTexture(0);
    fb2->BindTexture(1);
    fb1->DrawQuad(-sys::WW2, -sys::WH2, sys::WW2, sys::WH2, z_level);
    fb1->UnbindTexture(0);
    fb2->UnbindTexture(1);  
    glActiveTexture(GL_TEXTURE0);
    if(shader) shader->Unbind();
  }

  // ArrayQuadsDrawer on FB ==============
  void draw_AQD_to_FB(ArrayQuadsDrawer * drawer, Texture * texture, FrameBuffer * fb, FrameShader * shader=nullptr){
    if(shader){
      shader->Bind();
      shader->UpdateUniforms();
    }
    fb->Target();   
    if(texture) texture->Bind();
    drawer->Draw();
    if(texture) texture->Unbind();
    fb->Untarget();
    if(shader) shader->Unbind();
  }

  void draw_AQD_to_Screen(ArrayQuadsDrawer * drawer, Texture * texture, FrameShader * shader=nullptr){
    if(shader){
      shader->Bind();
      shader->UpdateUniforms();
    }
    if(texture) texture->Bind();
    drawer->Draw();
    if(texture) texture->Unbind();
    if(shader) shader->Unbind();
  }
};

#endif

