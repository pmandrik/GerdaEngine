// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GerdaParticle_H
#define GerdaParticle_H 1

namespace ge {

  /// classical particles based on ArrayQuadsDrawer+glDrawArrays & shaders based

  class Particle{
    /// Particle is a Sprite with a fixed lifetime and properties related to the motion.
    /// Particles are not expected to interact with anything.
    /// TickPhysic calculate speed, pos = pos + speed
    public:
      Particle(ArrayQuadsDrawer * drawer){
        sprite.Hide();
        sprite.SetDrawer( drawer );
      }

      void Hide(){
        lifetimer = Timer( 0 );
      }

      virtual void TickPhysic(){
        speed = speed*speed_multiplicator + accel;
        sprite.pos += speed;
      }

      virtual void Init(const int & time_, const v2 & pos_, const v2 & speed_, TexTile * ttile_){
        lifetimer = Timer( time_ );
        sprite.pos   = pos_;
        sprite.pos.z = sys::z_def_value;
        speed        = speed_;
        sprite.ttile    = ttile_;
        sprite.size     = ttile_->size * 0.5;
        sprite.SendChanges();
        frozen = false;
        if( time_ < 0 ) frozen = true; 
      }

      bool Tick(){
        /// if not frozen call TickPhysic() and update position of the Sprite
        if(frozen) return true;
        if( not lifetimer.Tick() ){
          TickPhysic();
          sprite.SendChangesFast();
          return true;
        }
        sprite.Hide();
        return false;
      }

      v2 speed, accel, target_pos;
      Timer lifetimer;
      Sprite sprite;
      bool frozen;
      float speed_multiplicator = 1.;
  };

  class ParticleSystem : public BaseClass {
    public:
      ParticleSystem(ArrayQuadsDrawer * drawer_, int n_particles_){
        n_particles = n_particles_;
        drawer = drawer_;
        for(int i = 0; i < n_particles; i++){
          particles_waiting.push( new Particle( drawer ) );
        }
      }

      void Tick(){
        /// call Tick for particles in particles_active
        for(auto iter = particles_active.begin(); iter != particles_active.end(); ){
          if( (*iter)->Tick() ){
            iter++; 
            continue;
          }
          particles_waiting.push( (*iter) );
          iter = particles_active.erase( iter );
        }
      }

      Particle * Add(const int & time_, const v2 & pos_, const v2 & speed_, TexTile * ttile_){
        if(not particles_waiting.size()) return nullptr;
        Particle * particle = particles_waiting.top();
        particle->Init(time_, pos_, speed_, ttile_);
        particles_active.push_back( particle );
        particles_waiting.pop();
        MSG_DEBUG( __PFN__, ", particles used/available:", particles_active.size(), particles_waiting.size() );
        return particle;
      }

      int n_particles;
      list <Particle*> particles_active;
      stack<Particle*> particles_waiting;
      ArrayQuadsDrawer * drawer;
  };
}

#endif 
