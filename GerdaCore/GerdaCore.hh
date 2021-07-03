// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#ifndef GERDA_CORE_HH 
#define GERDA_CORE_HH 1

#define __PFN__ __PRETTY_FUNCTION__

#include <GerdaInputDev.hh>
#include <GerdaSystem.hh>
#include <GerdaOther.hh>
#include <GerdaInput.hh>
#include <GerdaFSM.hh>

namespace ge {

  struct Event;
  typedef std::function< void( Event* mother, const map<string, void*> * parameters ) > EventActionFunction;
  struct Event : public BaseClass {
    /// union of EventActionFunctions, status, name and counter of executions.
    /// Raise(parameters, force) -> call raise_action(this, parameters), remember parameters for Tick and End; return true if event can be executed
    /// Tick(force) -> call tick_action(this, parameters), do this at most timer max_time times; return false at the end if the timer
    /// End(force) -> call end_action(this, parameters); void
    /// if status <= 0 do not executed any Raise action if not force==true
    Event() : raise_action(NULL), tick_action(NULL), end_action(NULL), status(1), counter(0), name("") {}
    Event(string name_) : raise_action(NULL), tick_action(NULL), end_action(NULL), status(1), counter(0), name(name_) {}
    EventActionFunction raise_action, tick_action, end_action;
    int status, counter;
    string name;
    map<string, void*> * params;
    Timer timer;

    inline void InterruptTick(){ timer.End(); }

    bool Raise(map<string, void*> * parameters = nullptr, const bool & force=false){
      if(status <= 0 and not force){
        MSG_DEBUG("Event.Raise(): skip", name);
        return false;
      }
      MSG_DEBUG("Event.Raise():", name);
      counter++;
      params = parameters;
      if(not raise_action) return true;
      raise_action( this, params );
      return true;
    }

    bool Tick(){
      if(timer.Tick()) return false;
      MSG_DEBUG("Event.Tick():", name);
      if(not tick_action) return true;
      tick_action( this, params );
      return true;
    }

    void End(){
      MSG_DEBUG("Event.End():", name);
      if(not end_action) return;
      end_action( this, params );
    }
  };

  class EventSystem : public BaseClass {
    /// check SDL events + handle custom Events
    public:
      EventSystem(){
        mouse    = sys::mouse;
        keyboard = sys::keyboard;
      }

      void AddEvent(string & key, Event * event){
        /// add event to the system if the key is not already in use
        events.emplace( key, event);
      }

      void AddEvent(Event * event){
        /// add event to the system using event.name as key
        AddEvent( event->name, event);
      }

      Event * GetEvent(const string & name){
        auto find = events.find( name );
        if(find != events.end()) return find->second;
        return nullptr;
      }

      bool RaiseEvent(const string & name, map<string, void*> * parameters = nullptr, const bool & force=false){
        Event * event = GetEvent( name );
        if(not event){
          MSG_WARNING("EventSystem.RaiseEvent(): can't find event", name);
          return false;
        }
        bool status = event->Raise(parameters, force);
        if( status and (event->tick_action or event->end_action) ){
          ticking_events.push_back( event );
        }
        return status;
      }

      bool InterruptEventTick(const string & name){ 
        Event * event = GetEvent( name );
        if(not event){
          MSG_WARNING("EventSystem.InterruptEventTick(): can't find event", name);
          return false;
        }
        event->InterruptTick(); 
      }

      void Tick(){
        /// check SDL events
        while(SDL_PollEvent(&game_event)){
          if(game_event.type == SDL_MOUSEMOTION){
            mouse->x = game_event.motion.x;
            mouse->y = game_event.motion.y;
          }
          else if(game_event.type == SDL_MOUSEBUTTONDOWN or game_event.type == SDL_MOUSEBUTTONUP) mouse->ApplyEvent(game_event);
          else if(game_event.type == SDL_MOUSEWHEEL) mouse->wheel = game_event.wheel.y;
          //else if(game_event.type == SDL_KEYDOWN or game_event.type == SDL_KEYUP) keyboard->ApplyEvent(game_event);
        }

        /// check custom Events
        for(auto it = ticking_events.begin(); it != ticking_events.end(); ){
          bool is_active = (*it)->Tick();
          if(not is_active){
            (*it)->End();
            it = ticking_events.erase( it );
          } else {
            ++it;
          }
        }
      }

      SDL_Event game_event;
      Mouse * mouse;
      Keyboard * keyboard;

      map <string, Event*> events;
      list<Event*> ticking_events;
  };

}

#endif







