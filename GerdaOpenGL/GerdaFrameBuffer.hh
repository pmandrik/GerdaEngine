// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef  GerdaFrameBuffer_H
#define GerdaFrameBuffer_H 1 

namespace ge {
  class FrameBuffer : public BaseClass {
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sys::WW, sys::WH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Depth buffer
        glGenRenderbuffers(1, &depth_id);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_id);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, sys::WW, sys::WH);
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
        glViewport( 0, 0, sys::WW, sys::WH );
      }
      void Untarget(){ glBindFramebuffer(GL_FRAMEBUFFER, 0);      }

      void Clear(){
        Target();
        glClearColor(sys::fb_def_color.r,sys::fb_def_color.g,sys::fb_def_color.b,sys::fb_def_color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Untarget();
      }

      void Draw(){
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glBegin(GL_QUADS);
        glTexCoord2f(1, 	1);  glVertex3f(  sys::FBW2,  sys::FBH2, 0.);
        glTexCoord2f(0,   1);  glVertex3f( -sys::FBW2,  sys::FBH2, 0.);
        glTexCoord2f(0,   0);  glVertex3f( -sys::FBW2, -sys::FBH2, 0.);
        glTexCoord2f(1, 	0);  glVertex3f(  sys::FBW2, -sys::FBH2, 0.);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
      }

      void DrawOnScreen(){
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glBegin(GL_QUADS);
        glTexCoord2f(1, 	1);  glVertex3f(  sys::WW2,  sys::WH2, 0.);
        glTexCoord2f(0,   1);  glVertex3f( -sys::WW2,  sys::WH2, 0.);
        glTexCoord2f(0,   0);  glVertex3f( -sys::WW2, -sys::WH2, 0.);
        glTexCoord2f(1, 	0);  glVertex3f(  sys::WW2, -sys::WH2, 0.);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
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
    void DrawOnScreenActive(){ active->DrawOnScreen();     }

    FrameBuffer *active, *back;
  };
}

#endif
