// P.~Mandrik, 2023, https://github.com/pmandrik/ShtaEngine

#ifndef SHTA_DIALOGS_HH 
#define SHTA_DIALOGS_HH 1

#include <map>
#include <vector>
#include <string>
#include <memory>

#include "pmlib_other.hh"
#include "pmlib_msg.hh"
#include "pmlib_v2d.hh"
#include "pmlib_math.hh"

namespace shta {

  using namespace std;

  // @@@ Decoupled from ShtaEngine @@@
  // Menu & SubMenu  = {set of buttons, known how to switch between them, known how to switch to submenu???}
  // Progress bar
  // Menu context container e.g. popUp container
  // single choize itemize = {set of switchers, turn them when enable one}
  
  // Switcher Off <=> On
  // Button 
  // TextFiled to enter text
  // Polzunok
  
  // amount of menu items is small
  // amount of menu context container is small
  // => we can check IO collisions with menu context containers directly without optimisation
  //  1. we check if IO collide with active menu context
  //  2. then we check if IO collide with context item
  //  3. finally we apply actions to the menu item
  //  4. as a feedback meny item send event to event collector
  
  // classes to define geometry of UI item ======================================
  class UIAreaImp {
    public:
    virtual bool CheckHover(const float & x, const float & y) const { return false; }
  };
  
  class UIAreaImpRect : public UIAreaImp {
    public:
    pm::Rect rect;
    virtual bool CheckHover(const float & x, const float & y) const {
      return rect.Contain( pm::v2(x,y) );
    }
  };
  
  class UIActionImp {
    public:
    string state;
    virtual bool DoAction() { return false; }
    virtual string GetState() const { return state; }
  };
  
  class UIActionSwitcher : public UIActionImp {
    public:
    virtual string DoAction(string state) {
      if( state == "on" ) state = "off";
      else state = "on"; 
      return state;
    }
  };
  
  class UIActionButton : public UIActionImp {
    public:
    virtual bool DoAction() {
      // FIXME just create event on click
    }
  };
  
  class UIActionTextField : public UIActionImp {
    virtual bool DoAction() const {
      // FIXME state add symbols
      // FIXME just create event on click
    }
  };
  
  class UIActionSlider : public UIActionImp {
    public:
    virtual bool DoAction() const {
      // FIXME state add symbols
      // FIXME just create event on click
    }
  };
  
  // UI item ======================================
  // Button, menu etc can be constructed from this
  class UserInterface {
    public:
    string id, type, state;
    unique_ptr<UIAreaImp> area_imp;
    bool CheckHover(const float & x, const float & y) const {
      return area_imp->CheckHover(x, y);
    }
    
    unique_ptr<UIActionImp> action_imp;
    bool DoAction(){
      state = action_imp->DoAction(state);
    }
    
    string GetState(){
      return action_imp->GetState();
    }
  };
  
  class ContextUI {
    public:
    vector< shared_ptr<UserInterface> > interfaces;
    
    unique_ptr<UIAreaImp> area_imp;
    bool CheckHover(const float & x, const float & y) const {
      return area_imp->CheckHover(x, y);
    }
  };
  
  class FactoryUI {
    public:
    
  };
}

#endif







