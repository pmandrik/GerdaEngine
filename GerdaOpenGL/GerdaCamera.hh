// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine 
#ifndef  GerdaCamera_H
#define GerdaCamera_H 1

namespace ge {

  static const GLfloat INIT_MATRIX[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};

  struct CameraTarget{
    CameraTarget() : pos(0), time(0), zoom(0), normal(0), max_time(1)  {}
    CameraTarget(v2 p, int t, float z, v2 n) : pos(p), time(t), zoom(z), normal(n), max_time(t)  {}

    void Tick( float mval ){
      float val = mval * (time / (float) max_time);
      pos *= val;
      zoom *= val;
      normal *= val;
      time = max(0, time-1);
    }

    v2 pos;
    int time;
    int max_time;
    float zoom;
    v2 normal;
  };

  class Camera {
    public:
      Camera(){
        Reset();
        projMatrix[0] = 777;
      }

      inline void Resize( const int & r){ zoom = max(0.000000001, zoom - 0.1*r*zoom); }
      inline void ResetZoom(){ zoom = 1.; }
      void Reset(){
        to = v2(0, 0, 0);
        from = v2(0, 0, 1);
        normal = v2(0, 1, 0);
        zoom = 1;
        buffer = sys::z_buffer;
        pause = true;
        smooth = false;
        angleZ = 0.;
      }

      void SetTarget(v2 target){
        to.Set(target);
        from.Set(target);
      }

      void SetTarget(v2 target, float z, v2 norm){
        SetTarget(target);
        zoom = z;
        normal = norm;
      }

      void Move(v2 shift){
        to += shift;
        from += shift;
      }

      void PushPoint(v2 pos, int time, float zoom, v2 normal = v2(0,1)){
        targets.push_back( CameraTarget(pos, time, zoom, normal) );
      }

      v2 AbsToScreen(int x, int y){ return -to + v2(x - sys::WW2, -y + sys::WH2) * zoom; }
      v2 AbsToScreen(v2 pos)      { return -to + v2(pos.x - sys::WW2, -pos.y + sys::WH2) * zoom; }

      void ReTick(){
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(projMatrix);

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(modelMatrix);
      }

      void LoadFBDefault(){
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-sys::FBW2, sys::FBW2, -sys::FBH2, sys::FBH2, -buffer, buffer);  // ge::glViewport( 0, 0, sys::WW, sys::WH );

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef(angleZ, 0.f, 0.f, 1.f);
      }

      void LoadDefault(){
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-sys::SW2, sys::SW2, -sys::SH2, sys::SH2, -buffer, buffer);  // ge::glViewport( 0, 0, sys::WW, sys::WH );

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef(angleZ, 0.f, 0.f, 1.f);
      }

      float GetPhi(){
        return normal.Angle();
      }

      void Print(){
        msg(to, zoom, normal);
      }

      bool Finished(){
        return not targets.size();
      }

      void Tick(){
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

    v2 to, from, normal; 
    CameraTarget inertia;
    float zoom, buffer, angleZ;
    vector<CameraTarget> targets;
    bool pause, smooth;
    GLfloat projMatrix[16], modelMatrix[16], modelViewProjectionMatrix[16];
  };
}

#endif
