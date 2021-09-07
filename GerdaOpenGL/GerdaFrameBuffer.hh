// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef  GerdaFrameBuffer_H
#define GerdaFrameBuffer_H 1 

namespace ge {
  class FrameBuffer : public BaseClass {
    /// OpenGL frame buffer class wrapper = texture + depth buffer stored at GPU and with fast GPU access
    public :
      FrameBuffer(){
        // Texture
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sys::FBW, sys::FBH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Depth buffer
        glGenRenderbuffers(1, &depth_id);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_id);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, sys::FBW, sys::FBH);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // Framebuffer to link everything together
        glGenFramebuffers(1, &fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_id);

        // check
        GLenum status;
        if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
          msg_err("glCheckFramebufferStatus" , status);
        } else MSG_INFO("FrameBuffer generate done ok", fbo_id, texture_id, depth_id);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
      }

      void Target()  { 
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
        glViewport( 0, 0, sys::FBW, sys::FBH );
      }
      void Untarget(){ glBindFramebuffer(GL_FRAMEBUFFER, 0);      }

      void Clear(){
        Target();
        glClearColor(sys::fb_def_color.r,sys::fb_def_color.g,sys::fb_def_color.b,sys::fb_def_color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Untarget();
      }

      void BindTexture(const int & index){
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, texture_id);
      }

      void UnbindTexture(const int & index){
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, 0);
      }

      void Draw(const float & xd = -sys::FBW2, const float & yd = -sys::FBH2, const float & xu = sys::FBW2, const float & yu = sys::FBH2){
        /// Draw framebuffer rectangle, in most cases:
        /// 1) on the another framebuffer, than sizes are taken from sys::FBW, sys::FBH
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glBegin(GL_QUADS);
        glTexCoord2f(1, 	1);  glVertex3f( xu,  yu, 0.);
        glTexCoord2f(0,   1);  glVertex3f( xd,  yu, 0.);
        glTexCoord2f(0,   0);  glVertex3f( xd,  yd, 0.);
        glTexCoord2f(1, 	0);  glVertex3f( xu,  yd, 0.);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
      }

      void DrawOnWindow(const bool & mirror_x=false, const bool & mirror_y=false){
        if(mirror_x and mirror_y) Draw(sys::WW2, sys::WH2, -sys::WW2, -sys::WH2);
        else if(mirror_x) Draw(sys::WW2, -sys::WH2, -sys::WW2, sys::WH2);
        else if(mirror_y) Draw(-sys::WW2, sys::WH2, sys::WW2, -sys::WH2);
        else Draw(-sys::WW2, -sys::WH2, sys::WW2, sys::WH2);
      }

      GLuint fbo_id, texture_id, depth_id;
  };

  class PairFrameBuffer {
    public :
    PairFrameBuffer(){
      active = new FrameBuffer();
      back   = new FrameBuffer();

      active->Clear();
      back->Clear();
    }

    ~PairFrameBuffer(){
      delete active;
      delete back;
    }

    void Flip(){ swap(active, back); }

    void DrawBackOnActive(){
      active->Target();
      back->Draw();
      active->Untarget();
    };

    void TargetActive(){   active->Target();   }
    void UntargetActive(){ active->Untarget(); }
    void ClearActive(){    active->Clear();    }
    void DrawActive(){     active->Draw();     }
    void DrawActiveOnWindow(const bool & mirror_x=false, const bool & mirror_y=false){ active->DrawOnWindow(mirror_x, mirror_y);     }
    void DrawTexture(Texture * text){
      TargetActive();
      text->Draw(v2(), sys::FBV2);
      UntargetActive();
    }
    void ApplyShader(FrameShader * shader, const int & N_times){
      shader->Bind();
      shader->UpdateUniforms();
      for(int i = 0; i < N_times; i++){
        DrawBackOnActive();
        Flip();
      }
      Flip();
      shader->Unbind();
    }

    FrameBuffer *active, *back;
  };

  void draw_2fb_with_shader(FrameShader * shader, FrameBuffer * fb1, FrameBuffer * fb2, FrameBuffer * fb_target){
    shader->Bind();
    shader->UpdateUniforms();
    fb_target->Target();
    fb1->BindTexture(0);
    fb2->BindTexture(1);
      ge::glBegin(GL_QUADS);
      const float & xd = -sys::FBW2;
      const float & yd = -sys::FBH2;
      const float & xu = ge::sys::FBW2;
      const float & yu = ge::sys::FBH2;
      ge::glTexCoord2f(1, 	1);  ge::glVertex3f( xu, yu, 0. );
      ge::glTexCoord2f(0,   1);  ge::glVertex3f( xd, yu, 0. );
      ge::glTexCoord2f(0,   0);  ge::glVertex3f( xd, yd, 0. );
      ge::glTexCoord2f(1, 	0);  ge::glVertex3f( xu, yd, 0. );
      ge::glEnd();  
    fb1->UnbindTexture(0);
    fb2->UnbindTexture(1);  
    glActiveTexture(GL_TEXTURE0);
    fb_target->Untarget();
    shader->Unbind();
  }
}

#endif
