// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GerdaScene_H
#define GerdaScene_H 1 

namespace ge {
  class Scene {
    /// scene is used to separate different part of the workflow actions defined in Tick()
    /// when IsEnd()==true the active scene is changed to GetNextScene()
    /// if GetTransitionScene()!="", then this Scene is changed to GetTransitionScene() and then to GetNextScene()
    /// Scene learns history using Scene * prev_scene
    public:
    string name, next_scene_name, transition_scene_name;

    virtual string GetTransitionScene(){ return transition_scene_name; };
    virtual string GetNextScene(){ return next_scene_name; };
    virtual bool IsEnd(){ return false; }
    virtual void Tick() = 0;
    virtual bool IsTransition(){ return false; }
    
    Scene * prev_scene = nullptr;
  };

  class SceneTransition : public Scene {
    /// special case of Scene to be used for transition between scenes, propogating Scene * prev_scene to the final state
    virtual bool IsTransition(){ return true; }
  };

  class SceneMachine : public BaseClass {
    public:
    /// basic machine to switch between scenes 
    map<string, Scene*> scenes;
    Scene* active_scene;

    void Tick(){
      active_scene->Tick();
      if( active_scene->IsEnd() ){
        // find transition scene if exist
        Scene * transition_scene = nullptr;
        string transition_scene_name = active_scene->GetTransitionScene();
        if( transition_scene_name.size() ){
          auto it_tr = scenes.find(transition_scene_name);
          if( it_tr == scenes.end() )
            MSG_WARNING(__PFN__, "can't fint Scene transition", transition_scene_name, "skip");
          else transition_scene = it_tr->second;
        }
        // find true next scene
        string next_scene_name = active_scene->GetNextScene();
        auto it = scenes.find(next_scene_name);
        if( it == scenes.end() ){ 
          MSG_WARNING(__PFN__, "can't fint Scene", next_scene_name, "return");
          return;
        }
        Scene * next_scene = it->second;
        // set scenes
        if(not transition_scene){
          if( active_scene->IsTransition() ) next_scene->prev_scene = active_scene->prev_scene;
          else                               next_scene->prev_scene = active_scene;
          active_scene = next_scene;
        } else {
          transition_scene->prev_scene = active_scene;
          transition_scene->next_scene_name = next_scene_name;
          active_scene = transition_scene;
        }
      }
    }
  };
}

#endif 
