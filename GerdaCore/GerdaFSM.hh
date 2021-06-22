// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GERDA_FSM_HH 
#define GERDA_FSM_HH 1

//TODO remove this remove this remove this remove this remove thisremove thisremove this unstable 

namespace ge {

  // states
  // states functions : tick, begin, end

  typedef std::function< void( void ) > state_func;

  struct State {
    State() : tick(NULL), begin(NULL), end(NULL), name("") {}
    State(string n) : tick(NULL), begin(NULL), end(NULL), name(n) {}

    string name;
    state_func tick;
    state_func begin;
    state_func end;

    bool on, novice, remove_on_time;
    Timer tick_timer;
  };

  class FSM {
    public:
    FSM(){}

    void AddStateDefinition( State state ){
      if(not state.name.size()){
        msg("FSM : add_state_definition() : empty state name");
        return;
      }
      states[state.name] = state;
    }

    template<typename... Args>
    void AddStateDefinition( State state, Args... args ){
      AddStateDefinition(state);
      AddStateDefinition(args...);
    }

    bool AddState(string name, int tick_time = 0, bool remove_on_time = false, bool call_begin = true){
      auto find = states.find( name );
      if(find == states.end()){
        if(name.size()) msg("FMS : add_state() : wrong state name ", name);
        return false;
      }
      State state = (*find).second;
      if( state.begin and call_begin ) state.begin();
      state.on = true;
      state.novice = true;
      state.remove_on_time = remove_on_time;
      state.tick_timer = Timer(tick_time);
      active_states.push_back( state );
      return true;
    }

    State * GetActiveState(const string & name){
      for(auto it = active_states.begin(); it != active_states.end(); it++){
        State * state = &(*it);
        if( state->name == name) return state;
      }
      return nullptr;
    }

    bool RemoveState(string name, bool call_end = true){
      for(auto it = active_states.begin(); it != active_states.end(); it++){
        State * state = &(*it);
        if(state->name == name){ 
          if( state->end and call_end ) state->end();
          state->on = false; 
          return true;
        }
      }
      return false;
    }

    void SwitchState(string name_remove, string name_add, bool call_end = true, bool call_begin = true){
      RemoveState(name_remove, call_end);
      AddState(name_add, call_begin);
    }

    void Tick(){
      for(auto it = active_states.begin(); it != active_states.end(); it++){
        State * state = &(*it);
        if(not state->on){ it = active_states.erase( it ); it--; continue; }
        if(state->novice){ state->novice = false; continue;                } 
        if(state->tick)  { state->tick();}

        state->tick_timer.Tick();
        // msg( state->tick_timer.itime , state->tick_timer.mtime  );
        if(state->remove_on_time and not state->tick_timer.itime) { state->end(); state->on = false; };
      }
    }

    void PrintDebug(){
      for(auto state : active_states){
        msg(state.name, state.tick_timer.itime);
      }
    }

    map <string, State> states;
    list<State> active_states;
  };

}

#endif







