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
    v3 force;

    inline bool CheckType(const string & type_){ return type_ == type; }
  };

  class PlatformerObject : public Object{
    /// class to wrap platformer physic logic used in the game machanic
    bool flag_on_ground = false;
    bool flag_under_ground = false;
    bool flag_on_ladder  = false;

    /// check wall near object  
    bool flag_near_wall  = false;
    bool flag_wall_ahead  = false;
    bool flag_wall_behind = false;

    /// check ground/holes near object   
    bool flag_ground_ahead  = false;
    bool flag_ground_behind = false;

    /// check if player is near  
    int flag_player = 0;
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
