// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#ifndef GerdaEngine_H
#define GerdaEngine_H 1

// ======= SDL ====================================================================
#include "SDL.h"
#include "SDL_opengl.h"
#include "GerdaSDL_prerequests.hh"

// ======= OPENGL =================================================================

// ======= STL ====================================================================
#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>
#include <list>
#include <stack>
#include <string>
#include <random>
#include <math.h>
#include <stdlib.h>
#include <ctime>
#include <cctype>
#include <algorithm>
#include <functional>
#include <memory>

// ======= PMANDRIK_LIBRARY ============================================================
#include "pmlib_v2d.hh"
#include "pmlib_msg.hh"
#include "pmlib_other.hh"
#include "pmlib_2d_physics.c"

// ======= EXTERNAL ====================================================================
#include "tinyxml2.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "utf8.h"

// ======= GERDA ====================================================================
#include "GerdaMath.hh"
#include "GerdaCore.hh"
#include "GerdaSDL.hh"
#include "GerdaOpenGL.hh"

namespace ge {

  using namespace pm;
  using namespace std;
  using namespace tinyxml2;
  
  class CoreFactory : public BaseClass {
    public:

    //================================================================================
    virtual std::shared_ptr<Image> CreateImage(string name, string path, string kind){
      /// CrateTYPE know how to create Image, Testure, Shader, QuadsDrawer
      std::shared_ptr<Image> image = std::make_shared<Image>();
      if(kind == "" or kind=="STBI") image->image_implementation = std::unique_ptr<STBIImageImplementation>();
      else{
        MSG_ERROR(__PFN__, "skip not-valid name", name, "of a kind", kind);
        throw std::invalid_argument( "skip not-valid name \"" +  name + "\" of a kind \"" + kind + "\"" );
      }
      image->name = name;
      image->LoadPNG( path );
      return image;
    }
    
    virtual std::shared_ptr<Texture> CreateTexture(string name, std::shared_ptr<Image> img, string kind){
      std::shared_ptr<Texture> txt;
      if(kind == "" or kind=="GL") txt = std::make_shared<TextureGL>( img );
      else{
        MSG_ERROR(__PFN__, "skip not-valid name", name, "of a kind", kind);
        throw std::invalid_argument( "skip not-valid name \"" +  name + "\" of a kind \"" + kind + "\"" );
      }
      txt->name = name;
      return txt;
    }
    
    virtual std::shared_ptr<Shader> CreateShader(string name, std::string & vert, std::string & frag, string kind){
      std::shared_ptr<Shader> obj = std::make_shared<Shader>( );
      if(kind == "" or kind=="SLaShader") obj = std::make_shared<SLaShader>();
      else{
        MSG_ERROR(__PFN__, "skip not-valid name", name, "of a kind", kind);
        throw std::invalid_argument( "skip not-valid name \"" +  name + "\" of a kind \"" + kind + "\"" );
      }
      /// TODO
      obj->Load( vert, frag );
      return obj;
    }
    
    virtual std::shared_ptr<QuadsDrawer> CreateDrawer(string name, std::shared_ptr<Texture> texture, string kind){
      std::shared_ptr<QuadsDrawer> obj;
      if(kind=="TextureDrawer") obj = std::make_shared<TextureDrawer>();
      else if(kind == "" or kind=="ArrayQuadsDrawer") obj = std::make_shared<ArrayQuadsDrawer>();
      else{
        MSG_ERROR(__PFN__, "skip not-valid name", name, "of a kind", kind);
        throw std::invalid_argument( "skip not-valid name \"" +  name + "\" of a kind \"" + kind + "\"" );
      }
      obj->SetTexture( texture );
      return obj;
    }
    
    virtual std::shared_ptr<SLaFB> CreateSLaFB(string id, std::shared_ptr<SLaShader> shader){
      std::shared_ptr<SLaFB> obj = std::make_shared<SLaFB>( id, shader );
      obj->Init();
      return obj;
    }

    virtual std::shared_ptr<SLaQD> CreateSLaQD(string id, std::shared_ptr<QuadsDrawer> drawer, std::shared_ptr<SLaShader> shader){
      std::shared_ptr<SLaQD> obj = std::make_shared<SLaQD>( id, shader );
      obj->Init( drawer );
      return obj;
    }
    
    /// ImportTYPECfi convert ConfigItem data into TYPE object
    virtual void ImportImageCfi(std::shared_ptr<DataContainer> dc, const ConfigItem & item, const ConfigItem & sys_item){
      string name = item.GetAttribute("id");
      string path = item.GetAttribute("path");
      string imp = get_from_configs( item, "imp", sys_item, "image_imp");
      auto image = CreateImage(name, path, imp);
      dc->Add( name, image );
    }
    
    virtual void ImportTextureCfi(std::shared_ptr<DataContainer> dc, const ConfigItem & item, const ConfigItem & sys_item){
      string name = item.GetAttribute("id");
      string image = item.GetAttribute("image_id");
      string imp = item.GetAttribute("imp");
        
      string image_path = item.GetAttribute("image_path");
      string image_imp  = get_from_configs( item, "image_imp", sys_item, "image_imp" );
      std::shared_ptr<Image> image_obj ;
      if( image.size() ){
        image_obj = CreateImage(image, image_path, image_imp);
        dc->Add( image, image_obj );
      } 
      image_obj = dc->GetImage( image );
      
      auto texture = CreateTexture(name, image_obj, imp);
      dc->Add( name, texture );
    }
    
    virtual void ImportDrawerCfi(std::shared_ptr<DataContainer> dc, const ConfigItem & item, const ConfigItem & sys_item){
      string name = item.GetAttribute("id");
      string texture_id = item.GetAttribute("texture_id");
      string imp = item.GetAttribute("imp" );
      std::shared_ptr<Texture> texture = dc->GetTexture( texture_id );
      auto drawer = CreateDrawer(name, texture, imp);
      dc->Add( name, drawer );
    }
    
    virtual void ImportShaderCfi(std::shared_ptr<DataContainer> dc, const ConfigItem & item, const ConfigItem & sys_item){
      string name = item.GetAttribute("id");
      string vert = item.GetAttribute("vert");
      string frag = item.GetAttribute("frag");
      string imp = get_from_configs( item, "imp", sys_item, "shader_imp" );
      auto obj = CreateShader(name, vert, frag, imp);
      // dc->Add( name, obj );
    }
    
    virtual void ImportSLaCfi(std::shared_ptr<DataContainer> dc, const ConfigItem & item, const ConfigItem & sys_item){
      string id = item.GetAttribute("id");
      string shader_id = item.GetAttribute("shader_id", "");
      string drawer_id = item.GetAttribute("drawer_id", "");
      std::shared_ptr<ge::Shader> shader = dc->GetShader( shader_id );
      std::shared_ptr<ge::SLaShader>  frame_shader = std::dynamic_pointer_cast<ge::SLaShader>( shader );
      std::shared_ptr<QuadsDrawer> drawer = dc->GetQuadsDrawer( drawer_id );
      shared_ptr<SLa> obj;
      if( item.type == "slaFB" ) obj = CreateSLaFB(id, frame_shader);
      if( item.type == "slaQD" ) obj = CreateSLaQD(id, drawer, frame_shader);
      if(obj) dc->Add( id, obj );
    }
    
    //================================================================================
    void ImportConfigItem(std::shared_ptr<DataContainer> dc, const ConfigItem & item, const ConfigItem & sys_item){
      /// ImportConfigItem call ImportTYPECfi based on ConfigItem type
      if( not item.valid){
        MSG_WARNING( __PFN__, "skip not-valid item of type = ", item.type, "\nskip" );
        return;
      }
      
      try{
        MSG_DEBUG( __PFN__, "process", item.type );
        if( item.type == "image" ) ImportImageCfi( dc, item, sys_item );
        if( item.type == "texture" ) ImportTextureCfi( dc, item, sys_item );
        if( item.type == "shader" ) ImportShaderCfi( dc, item, sys_item );
        if( item.type == "drawer" ) ImportDrawerCfi( dc, item, sys_item );
        if( item.type == "slaFB" ) ImportSLaCfi( dc, item, sys_item );
      } catch (std::invalid_argument const& ex) {
        MSG_ERROR( ex.what(), "\nskip" );
        return;
      }
    }
    
    //================================================================================
    void ImportConfig(std::shared_ptr<DataContainer> dc, std::shared_ptr<Config> cfg){
      /// ImportConfig create items defined in Config into DataContainer
      auto sys_item = cfg->GetSysItem();    
    
      vector<string> types_for_importcfg = { "image", "texture", "shader", "drawer", "slaFB" };
      for( auto type : types_for_importcfg ){
        auto items = cfg->GetItems( type );
        for(auto item : items)
          ImportConfigItem( dc, item, sys_item );
      }
    }
  };

  namespace sys {
    shared_ptr<CoreFactory> core_factory;
    shared_ptr<DataContainer> def_container;
  };

  bool init_gerda(string kind, std::shared_ptr<Config> cfg){
    msg("init_gerda() ... step I, internal");
    sys::mouse        = new Mouse();
    sys::keyboard     = new Keyboard();
    sys::file_input   = new FileInput();
    sys::core_factory  = make_shared<CoreFactory>();
    sys::def_container = make_shared<DataContainer>();

    /// Load screen width or height from Config if available
    int screen_width  = cfg->GetSysAttributeI("screen_width", 0);
    int screen_height = cfg->GetSysAttributeI("screen_height", 0);
    msg( screen_width,  screen_height, "screen_width,  screen_height" );
    if( screen_width and screen_height ) sys::set_windows_size(screen_width, screen_height);

    sys::screenshoot_counter     = 100;
    sys::screenshoot_image       = new Image(sys::WW, sys::WH);
    sys::screenshoot_image->data = new float[4 * sys::WW * sys::WH];
    sys::screenshoot_timer       = new Timer(1);

    msg("init_gerda() ... step II, backend");
    if( kind == "sdl" ){
      // init sdl ...
      msg("init_sdl ... ");
      SDL_Init(SDL_INIT_VIDEO);
      msg("SDL_GetPlatform = ", SDL_GetPlatform());
      msg("SDL_GetNumVideoDisplays = ", SDL_GetNumVideoDisplays());
      msg("init_sdl ... done");

      // set sdl screen ...
      SDL_DisplayMode desktop_mode;
      bool not_ok = SDL_GetDesktopDisplayMode(0, &desktop_mode);

      if(not_ok){
        msg("SDL_GetDesktopDisplayMode() error ... ", SDL_GetError());
        msg(" will use screen width and height from ge.cfg");
      }

      SDL_Window * window;
      if(not_ok or not sys::FULLSCREEN){
        sys::set_screen_size(sys::WW, sys::WH);
        window = SDL_CreateWindow("GerdaWindow", 0, 0, sys::WW, sys::WH, SDL_WINDOW_OPENGL);
      }
      else{
        msg("SDL_GetDesktopDisplayMode() =", desktop_mode.w, desktop_mode.h, desktop_mode.refresh_rate);
        sys::set_screen_size(desktop_mode.w, desktop_mode.h);
        window = SDL_CreateWindow("GerdaWindow", 0, 0, sys::SW, sys::SH, SDL_WINDOW_OPENGL|SDL_WINDOW_FULLSCREEN);
      }

      msg("init_sdl_screen ... ok");

      msg("init_opengl via sdl ... ");
      SDL_GLContext glcontext = SDL_GL_CreateContext(window);
      load_opengl_context();

      // Alpha
	    glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	    //glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA );
	    glEnable(GL_ALPHA_TEST);
	    glAlphaFunc(GL_GREATER, 0);

      // Depth
      glEnable(GL_DEPTH_TEST);
	    glDepthMask(GL_TRUE);
	    glDepthFunc(GL_LEQUAL);
	    glDepthRange(0.0, 1.0);
      msg("init_opengl ... done");

      sys::core = new CoreSDL(sys::mouse, sys::keyboard, sys::fps);
      ((CoreSDL*)sys::core)->window = window;
      ((CoreSDL*)sys::core)->glcontext = glcontext;

      //
      sys::file_input->read_text_files_imp = shared_ptr<ReadTxtFileImpSDL>();
    } else {
      msg("Wrong parameter", kind, ", return false");
      return false;
    }

    msg("init_gerda() ... step III, internal");
    sys::camera       = new Camera();

    msg("init_gerda() ... done");
    return true;
  }

  void tick_gerda(){
    sys::core->Tick();

    // remove all single-time-used flags
    sys::mouse->Tick();

    // get keyboard state after all SDL events was parsed
    sys::keyboard->Tick();

    // after this call apply in main loop draw functions
    // make screenshoot
    if(sys::keyboard->screenshoot and not sys::screenshoot_timer->itime){
      glReadPixels(0, 0, sys::WW, sys::WH, GL_RGBA, GL_UNSIGNED_BYTE, sys::screenshoot_image->data);
      // sys::screenshoot_image->Mirrow();
      sys::screenshoot_image->Flip();
      // sys::screenshoot_image->WritePNG( get_screnshoot_name() );
      sys::screenshoot_timer->Tick();
    }
    if(sys::screenshoot_timer->itime) sys::screenshoot_timer->Tick();
  }

  bool check_exit(){
    if(sys::keyboard->exit) return true; // FIXME menu, other
    return false;
  }

};

#endif
