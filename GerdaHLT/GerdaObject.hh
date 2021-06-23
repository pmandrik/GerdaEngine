// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GerdaObject_H
#define GerdaObject 1

namespace ge { 
  class Object {
    public:
      Object() {
        flip  = 0;
        angle = 0;
        size  = v2();
        pos   = v2();
        pos.z = 1 * 0.1;
        ttile = nullptr;
        drawer  = nullptr;
        draw_id = -1;
      }

      Object( TiledMapObject* obj, TexTile * tile ) {
        // TiledMapObject
        flip  = obj->flip;
        angle = obj->angle;
        size  = obj->size * 0.5;
        pos   = obj->pos;
        // pos.y *= -1;
        // pos  += v2(size.x, size.y).rotated( -angle );
    
        //pos.z = obj->pos.y / 10000.; // (0 -> 10000) -> (0., 0.1)
        pos.z = 0.5 + obj->level * 0.1;
        // TextTile
        ttile = tile;

        // Other
        drawer  = nullptr;
        draw_id = -1;
      }

      void SetDrawer(ArrayQuadsDrawer * dr){
        if(drawer) drawer->Remove(draw_id);
        drawer  = dr;
        draw_id = drawer->AddDummy();
        if(ttile) SendChanges();
      }

      void SendChanges(){
        drawer->Change(draw_id, pos, size, ttile->tpos, ttile->tsize, angle, flip);
      }

      void SendChangesFast(){
        drawer->ChangeFast(draw_id, pos, size);
      }

      void HideMove(){
        pos = sys::hide_position;
      }

      int draw_id, angle;
      v2 pos, size, flip;
      TexTile * ttile;
      ArrayQuadsDrawer * drawer;
  };
}

#endif
