// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GerdaObject_H
#define GerdaObject_H 1

namespace ge {
  class Object {
    public:
    /// something we expect to draw (sprite_body) and able to interact with tiled map and other objects (physic_body)  
    Sprite   * sprite_body;
    pmObject * physic_body;
    string name, type;

    /// field_bits - to check interaction with fields
    int field_bits = 0;
    int field_charge_bits = (0 & 0);

    v2 force;
    v2 force_internal;

    inline bool CheckType(const string & type_){ return type_ == type; }
    virtual int IsNearToPlayer(const v2 & pos){ return false; }
    virtual v2 GetLowerBodyPoint(){ return physic_body->pos - v2(0, physic_body->size.y);}
    virtual v2 GetUpperBodyPoint(){ return physic_body->pos + v2(0, physic_body->size.y);}
    virtual v2 GetCentralBodyPos(){ return physic_body->pos;}
    virtual void Tick(){};
    bool tick_locked = false;
  };

  class PlatformerPlayerObject;
  class PlatformerObject : public Object{
    public:
    /// class to wrap platformer physic logic used in the game machanic
    bool flag_on_ground = false;
    bool flag_under_ground = false;
    bool flag_on_ladder  = false;

    /// flags check wall near object  
    bool flag_near_wall  = false;
    bool flag_wall_ahead  = false;
    bool flag_wall_behind = false;

    /// flags check ground/holes near object   
    bool flag_ground_ahead  = false;
    bool flag_ground_behind = false;

    /// flags check if player is near  
    int flag_near_player = 0;
    bool flag_player_ahead  = false;
    bool flag_player_behind = false;
  
    /// head direction
    int dir;

    virtual int IsNearToPlayer(const v2 & pos){
      flag_near_player = 0;
      if( abs(physic_body->pos.x - pos.x) > interact_distance ) return 0;
      if( abs(physic_body->pos.y - pos.y) > interact_distance ) return 0;
      if(physic_body->pos.x - pos.x==0) return 1;
      return physic_body->pos.x - pos.x;
    }

    float interact_distance = 0;
    virtual void Tick(PlatformerPlayerObject * player){};
  };

  class PlatformerPlayerObject : public PlatformerObject {
    /// class to wrap also keyboard input expected for the platformer  
    void Tick(){
      desired_dir = v2(sys::keyboard->hor, sys::keyboard->ver);
    }

    v2 desired_dir;
  };


};

#endif 
