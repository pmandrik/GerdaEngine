#ifndef GERDA_GUI_HH 
#define GERDA_GUI_HH 1

namespace ge {

  using namespace pm;
  using namespace std;
  using namespace tinyxml2;

  class KeyAction {
    bool off = true, active = true;
    int key = -1;
    char kind;

    std::function< void( void ) > func;

    public:
    KeyAction( char kind_, int key_, std::function< void( void ) > func_) : kind(kind_), key(key_), func(func_) {
    }

    void Tick(){
      if(not active) return;
      if( kind == 'S' ){
        if( sys::keyboard->Pressed( key ) ) off = not off;
        if( not off ) return;
        func();
      }
      if( kind == 'P' ){
        if( not sys::keyboard->Pressed( key ) ) return;
        func();
      }
    }
  };
  
  template<class T>
  class ActionTree{
    /// groups of <T> actions with string key are ticked
    /// if they are in active_actions map
    std::map<std::string, std::vector<T> > actions;
    std::map<std::string, bool> active_groups; 
    std::vector<std::string> top_groups;
    
    public:
    void Reset(){
      active_groups.clear();
      for(std::string action : top_groups) ActivateGroup( action );
    }
    
    void AddTopAction(std::string action){
      top_groups.push_back( action );
      ActivateGroup( action, true );
    }
    
    void Add(std::string group, T action){
      actions[ group ].push_back( action );
    }
    
    bool ActivateGroup(std::string group_name, bool on = true){
      auto it = actions.find( group_name ) ;
      if( it == actions.end() ) return false;
      msg( group_name );
      active_groups[ group_name ] = on;
    }
    
    bool SwitchGroups(std::string A, std::string B){
      ActivateGroup(A, false);
      ActivateGroup(B, true );
    }
    
    void Tick(){      
      for (auto it = active_groups.begin(); it != active_groups.end() ; ){
        if (not it->second ) {
          active_groups.erase(it++);
          continue;
        }
        ++it;
      }
      
      auto active_groups_ = active_groups;
      for (auto it = active_groups_.begin(); it != active_groups_.end() ; ++it){
        auto iter = actions.find( it->first );
        if( iter == actions.end() ){
          it->second = false;
        } else {
          for( auto action : iter->second ) action.Tick();
        }
      }
    }
    
  };
  
};
  
#endif
