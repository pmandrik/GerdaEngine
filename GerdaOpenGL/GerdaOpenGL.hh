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

#include "GerdaDrawPrimitives.hh"
#include "GerdaCamera.hh"
#include "GerdaTexture.hh"
#include "GerdaShader.hh"
#include "GerdaFrameBuffer.hh"

#endif

