// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine 
#ifndef  GerdaDrawPrimitives_H
#define GerdaDrawPrimitives_H 1

namespace ge {

  // ======= simple draw functions ====================================================================
  void draw_quad(const v2 & pos, v2 size, const float & angle){
    glBegin(GL_QUADS);

    v2 perp = v2(-size.x, size.y);
    size = size.Rotated(angle);
    perp = perp.Rotated(angle);

    glVertex3f(pos.x - size.x, pos.y + size.y, pos.z); 
	  glVertex3f(pos.x - perp.x, pos.y + perp.y, pos.z);
	  glVertex3f(pos.x + size.x, pos.y - size.y, pos.z);
	  glVertex3f(pos.x + perp.x, pos.y - perp.y, pos.z);
	  glEnd();
  }

  void draw_texture_quad(const v2 & pos, v2 size, const v2 & tpos, const v2 & tsize, const float & angle, const v2 & flip){
    glBegin(GL_QUADS);
    if(flip.x) size.x *= -1;
    if(flip.y) size.y *= -1;
        
    v2 perp = v2(-size.x, size.y);
    size = size.Rotated(angle);
    perp = perp.Rotated(angle);

    glTexCoord2f(tpos.x, 	tpos.y);                      glVertex3f(pos.x - size.x, pos.y + size.y, pos.z); 
	  glTexCoord2f(tpos.x + tsize.x, 	tpos.y);            glVertex3f(pos.x - perp.x, pos.y + perp.y, pos.z);
	  glTexCoord2f(tpos.x + tsize.x, 	tpos.y + tsize.y);  glVertex3f(pos.x + size.x, pos.y - size.y, pos.z);
	  glTexCoord2f(tpos.x, 	tpos.y + tsize.y);            glVertex3f(pos.x + perp.x, pos.y - perp.y, pos.z);
	  glEnd();
  }

  // ======= draw array of quads ====================================================================
  namespace sys{
    const int   MAX_QUAD_NUMBER      = 10000;
    const int   QUAD_VERTEXES_NUMBER = 4;
    const int   VERTEX_ARRAY_SIZE    = 5;
    const int   QUAD_ARRAY_SIZE      = QUAD_VERTEXES_NUMBER * VERTEX_ARRAY_SIZE;
    const int   VERTEX_BITE_SIZE     = VERTEX_ARRAY_SIZE*sizeof(GL_FLOAT);
    const static float PERSPECTIVE_EDGE     = -1000000.;
  };

  class ArrayQuadsDrawer : public BaseClass { 
    public :
      ArrayQuadsDrawer(int max_size = sys::MAX_QUAD_NUMBER) {
        max_quads_number = min(max_size, sys::MAX_QUAD_NUMBER);
        quads_number = 0;
        for(int i = 0; i < max_quads_number; i++) Remove(i*sys::QUAD_ARRAY_SIZE);
      }

      int FindFreePosition(){
        for(int i = quads_number; i < max_quads_number; i++){
          if(data[i*sys::QUAD_ARRAY_SIZE+2] <= sys::PERSPECTIVE_EDGE){ quads_number = min(i+1, sys::MAX_QUAD_NUMBER); return i; }
        }
        for(int i = 0; i < quads_number; i++){
          if(data[i*sys::QUAD_ARRAY_SIZE+2] <= sys::PERSPECTIVE_EDGE){ quads_number = min(i+1, sys::MAX_QUAD_NUMBER); return i; }
        }
        MSG_DEBUG("ArrayQuadsDrawer.FindFreePosition(): warning, cant find free position");
        return (quads_number < max_quads_number ? quads_number : 0);
      }

      int AddDummy(){
        int id = FindFreePosition() * sys::QUAD_ARRAY_SIZE;
        return id;
      }

      int Add(v2 pos, v2 size, v2 tpos, v2 tsize){
        int id = FindFreePosition() * sys::QUAD_ARRAY_SIZE;
        Change(id, pos, size, tpos, tsize, 0, false);
        return id;
      }

      void Remove(const int & id){
        data[id+2]  = sys::PERSPECTIVE_EDGE;
        data[id+7]  = sys::PERSPECTIVE_EDGE;
        data[id+12] = sys::PERSPECTIVE_EDGE;
        data[id+17] = sys::PERSPECTIVE_EDGE;
      }

      void Move(const int & id, const v2 & shift){
        for(int i = id; i < id+sys::QUAD_ARRAY_SIZE; i+=sys::VERTEX_ARRAY_SIZE){
          data[i+0] += shift.x;
		      data[i+1] += shift.y;
        }
      }

      void ChangeFast(const int & id, const v2 & pos, const v2 & size){
        data[id+0]  = pos.x - size.x;
		    data[id+1]  = pos.y + size.y;

		    data[id+5]  = pos.x + size.x;
		    data[id+6]  = pos.y + size.y;

		    data[id+10] = pos.x + size.x;
		    data[id+11] = pos.y - size.y;

		    data[id+15] = pos.x - size.x;
		    data[id+16] = pos.y - size.y;
      }

      void Change(const int & id, const v2 & pos, v2 size, const v2 & tpos, const v2 & tsize, const int & angle, const v2 & flip){
        if(flip.x) size.x *= -1;
        if(flip.y) size.y *= -1;
        
        v2 perp = v2(-size.x, size.y);
        if(angle){
          size = size.Rotated(angle);
          perp = perp.Rotated(angle);
          // perp = v2(-size.x, size.y);
        }

        data[id+0]  = pos.x - size.x; data[id+3]  = tpos.x;
		    data[id+1]  = pos.y + size.y; data[id+4]  = tpos.y;
        data[id+2]  = pos.z;

		    data[id+5]  = pos.x - perp.x; data[id+8]  = tpos.x + tsize.x;
		    data[id+6]  = pos.y + perp.y; data[id+9]  = tpos.y;
        data[id+7]  = pos.z;

		    data[id+10] = pos.x + size.x; data[id+13] = tpos.x + tsize.x;
		    data[id+11] = pos.y - size.y; data[id+14] = tpos.y + tsize.y;
        data[id+12] = pos.z;

		    data[id+15] = pos.x + perp.x; data[id+18] = tpos.x;
		    data[id+16] = pos.y - perp.y; data[id+19] = tpos.y + tsize.y;
        data[id+17] = pos.z;
      }

      void Draw(){
        glEnableClientState( GL_VERTEX_ARRAY );
        glVertexPointer(3, GL_FLOAT, sys::VERTEX_BITE_SIZE, data);
        
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glTexCoordPointer(2, GL_FLOAT, sys::VERTEX_BITE_SIZE, &data[3] );
        
        glEnable(GL_MULTISAMPLE);
        glDrawArrays(GL_QUADS, 0, max_quads_number * sys::QUAD_VERTEXES_NUMBER );
      }

      int max_quads_number, quads_number;
      float data[sys::MAX_QUAD_NUMBER*sys::QUAD_ARRAY_SIZE];
  };

}

#endif
