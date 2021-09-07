// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GerdaPhysic_H
#define GerdaPhysic_H 1  

namespace ge {
  class Field : public BaseClass {
    public:
    Field(string name_, int index_ = 0){
      name = name_;
      SetBitIndex(index_);
    }

    int bit_index = 0;
    string name;

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

  class ConstantField : public Field {
    v3 force;
    virtual void Apply(Object * obj){
      obj->force += force * GetChargeBit( obj );
    };

    void Flip(){ force *= -1; };
  };
};

#endif
