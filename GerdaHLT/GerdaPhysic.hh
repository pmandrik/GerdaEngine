// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GerdaPhysic_H
#define GerdaPhysic_H 1  

namespace ge {
  // Fields
  class Field : public BaseClass {
    public:
    Field(string name_, int index_ = 0){
      name = name_;
      SetBitIndex(index_);
    }

    int bit_index = 0;
    string name;
    bool active = 1;

    void SetBitIndex(int index){
      bit_index = 1u << index;
      MSG_INFO(__PFN__, "set index to", bit_index);
    }

    void AddBit(Object * obj){ obj->field_bits |= bit_index; }
    void RemoveBit(Object * obj){ obj->field_bits &= ~bit_index; }

    void SetChargeBitP(Object * obj){ obj->field_charge_bits |=  bit_index; }
    void SetChargeBitM(Object * obj){ obj->field_charge_bits &= ~bit_index; }

    virtual bool CheckBit(Object * obj){ return bit_index & obj->field_bits; }
    virtual int GetChargeBit(Object * obj){ return 2 * (bit_index & obj->field_charge_bits) - 1; }
    virtual void Apply(Object * obj){};
  };

  class FieldConstant : public Field {
    v3 force;
    virtual void Apply(Object * obj){
      obj->force += force * GetChargeBit( obj );
    };

    void Flip(){ force *= -1; };
  };

  // Physics
  class Physic : public BaseClass{
    public:
    vector<Object*> ge_objects;
    vector<Field*> fields;
    std::vector< std::pair<int, int> > interacted_object_x_object_pairs;
    std::vector< std::pair<int, int> > interacted_bullet_x_object_pairs;

    /// ForceLaw - rules to obtain object speed from force
    void ForceLawDef(Object* obj){
      obj->physic_body->speed = obj->force * obj->physic_body->mass;
    }
  };

  class PhysicPlatformer : public Physic, public pmPhysicContainer {
    public:
      PhysicPlatformer(TiledMap * tiled_map){
        SetMap( tiled_map );
      }
      
      void SetMap(TiledMap * map_){
        pmPhysicContainer::msize_layers = map_->msize_layers;
        pm_physic.SetMap( (pmTiledMap*)map_ );
        tiled_map = map_;
      }

      virtual void ApplyForceLaw(Object * obj){
        ForceLawDef( obj );
      }

      virtual void UpdatePlatformerObjectFlags(PlatformerObject * obj, PlatformerPlayerObject * player){
        obj->flag_on_ground    = tiled_map->IsSolidD( obj->GetLowerBodyPoint() );
        obj->flag_under_ground = tiled_map->IsSolidU( obj->GetUpperBodyPoint() );
        obj->flag_on_ladder    = false; // FIXME

        /// check wall near object  
        obj->flag_wall_ahead  = tiled_map->IsSolid( obj->GetCentralBodyPos() + v2(obj->dir * tiled_map->tsize, 0) );
        obj->flag_wall_behind = tiled_map->IsSolid( obj->GetCentralBodyPos() - v2(obj->dir * tiled_map->tsize, 0) );
        obj->flag_near_wall   = obj->flag_wall_ahead or obj->flag_wall_behind;

        /// check ground/holes near object (just next tiles, if the speed of the object is > tile_size, it can jump out without checks)
        obj->flag_ground_ahead  = tiled_map->IsSolid( obj->GetLowerBodyPoint() + v2(obj->dir * tiled_map->tsize, 0) );
        obj->flag_ground_behind = tiled_map->IsSolid( obj->GetLowerBodyPoint() - v2(obj->dir * tiled_map->tsize, 0) );

        /// check if player is near
        obj->flag_near_player   = obj->IsNearToPlayer(player->physic_body->pos);
        obj->flag_player_ahead  = false;
        obj->flag_player_behind = false;
      }
    
      void Tick(){
        /// call pmPhysic.Tick()
        interacted_object_x_object_pairs.clear();
        interacted_bullet_x_object_pairs.clear();
        pm_physic.Tick( objects, particles, interacted_object_x_object_pairs, interacted_bullet_x_object_pairs);

        /// Loop over objects
        for(Object* objj : ge_objects){
          PlatformerObject * obj = (PlatformerObject *) objj;
          if(not obj->tick_locked) obj->Tick( player ); /// call obj->Tick( player )
          for(Field* field : fields){
            if(not field->active) continue;
            field->Apply( obj ); /// update force on the object
          }
          ApplyForceLaw( obj ); /// update speed of the object

          /// update flags
          UpdatePlatformerObjectFlags( obj, player );
        }
      }

      PlatformerPlayerObject * player;
      
      TiledMap * tiled_map;
      pmPhysic pm_physic;
  };
};

#endif
