// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine 
#ifndef  GerdaDrawPrimitives_H
#define GerdaDrawPrimitives_H 1

namespace ge {

  using namespace pm;
  using namespace std;
  using namespace tinyxml2;

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
  class ArrayQuadsDrawerGL : public ArrayQuadsDrawer { 
    public :
      void Draw(){
        texture->Bind();
        glEnableClientState( GL_VERTEX_ARRAY );
        glVertexPointer(3, GL_FLOAT, sys::VERTEX_BITE_SIZE, data);
        
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glTexCoordPointer(2, GL_FLOAT, sys::VERTEX_BITE_SIZE, &data[3] );
        
        glEnable(GL_MULTISAMPLE);
        glDrawArrays(GL_QUADS, 0, max_quads_number * sys::QUAD_VERTEXES_NUMBER );
        texture->Unbind();
      }
  };

  class TextureDrawerGL : public TextureDrawer {
    public:
      void Draw(){
        texture->Bind();
        for(auto it = drawables.begin(); it != drawables.end(); ++it){
          DrawableQuadData & dqd = it->second;
          //     void Draw(v2 pos, v2 size, std::shared_ptr<TexTile> ttile, float z_pos=0, const float & angle=0, bool flip_x=false, bool flip_y=false){
          texture->Draw( dqd );
        }
        texture->Unbind();
      }
  };

}//namespace
#endif
