// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine 
#ifndef  GerdaCamera_H
#define GerdaCamera_H 1

namespace ge {

  using namespace pm;
  using namespace std;
  using namespace tinyxml2;

  static const GLfloat INIT_MATRIX[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};

  class CameraGL : public Camera {
    public:
      CameraGL(){
        Reset();
        projMatrix[0] = 777;
      }

      virtual void ReTick(){
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(projMatrix);

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(modelMatrix);
      }

      virtual void LoadFBDefault(){
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-sys::FBW2, sys::FBW2, -sys::FBH2, sys::FBH2, -buffer, buffer);  // ge::glViewport( 0, 0, sys::WW, sys::WH );

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
      }

      virtual void LoadDefault(){
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-sys::SW2, sys::SW2, sys::SH2, -sys::SH2, -buffer, buffer);  // ge::glViewport( 0, 0, sys::WW, sys::WH );

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
      }

      virtual void Tick(){
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-sys::WW2 * zoom, sys::WW2 * zoom, -sys::WH2 * zoom, sys::WH2 * zoom, -buffer, buffer);
        glGetFloatv(GL_PROJECTION_MATRIX, projMatrix);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef(angleZ, 0.f, 0.f, 1.f);
        glTranslatef(to.x, to.y, to.z);
        //glLoadMatrixf(INIT_MATRIX);
        //gluLookAt(from.x, from.y, from.z, to.x, to.y, to.z, normal.x, normal.y, normal.z);
        glGetFloatv(GL_MODELVIEW_MATRIX, modelMatrix);

        for(int x = 0; x < 4; x++){
          for(int y = 0; y < 4; y++){
            modelViewProjectionMatrix[x + y*4] = 0;
            for(int xy = 0; xy < 4; xy++)
            modelViewProjectionMatrix[x + y*4] += projMatrix[4 * xy + x] * modelMatrix[4 * y + xy];
          }
        }

        if(not pause and targets.size()){
          CameraTarget * target = & targets.front();
          v2 dpos = (target->pos - to) / target->time;
          float dzoom = (target->zoom - zoom) / target->time;
          v2 dnormal = (target->normal - normal) / target->time;

          move( dpos + inertia.pos );
          zoom += dzoom + inertia.zoom;
          normal += dnormal + inertia.normal;
          inertia.Tick( smooth );
    
          //MSG( inertia.pos SPC inertia.time SPC inertia.max_time );
          target->time--;
          if(target->time <= 0){
            inertia = CameraTarget(dpos, 10, dzoom, dnormal);
            targets.erase( targets.begin() );
            if(targets.size()){
              inertia.time = int(0.55 * target->time);
              inertia.max_time = int(0.55 * target->time);
            }
          }
        }
      }

    GLfloat projMatrix[16], modelMatrix[16], modelViewProjectionMatrix[16];
  };

}//namespace
#endif
