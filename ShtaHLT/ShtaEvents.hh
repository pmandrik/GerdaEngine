// P.~Mandrik, 2023, https://github.com/pmandrik/ShtaEngine
#ifndef SHTA_EVENTS_HH 
#define SHTA_EVENTS_HH 1

#include <map>
#include <vector>
#include <string>
#include <list>
#include <memory>
#include <functional>

#include "pmlib_other.hh"
#include "pmlib_msg.hh"
#include "pmlib_v2d.hh"
#include "pmlib_math.hh"

namespace shta {

  using namespace std;

  // @@@ Decoupled from ShtaEngine @@@
  // Event defined as event_code -> to be used to identify handler
  //                  queue_code -> to be used to identify queue
  //                  data       -> data to process event, handler basically has all access to the event caller, so we move complex
  //                                event action logic into EventHandler implementation, so, data expected to contain only some tips for handler
  //
  // Event is something happend as response to action
  // e.g. event is notification about action => one action - one event
  // but one events can trigger different handlers, e.g. button press -> change texture, play sound, trigger internal events
  // such actions are not well connected, we combine everything on bu side
  struct Event {
    string event_code;
    string queue_code;
    int priority = 0;
    int weight   = 0;
    
    Event(){}
    Event(string event_code, string queue_code, int priority = 0, int weight = 0){
      this->event_code = event_code;
      this->queue_code = queue_code;
      this->priority = priority;
      this->weight = weight;
    }
    
    map<string, string> data;
  };
  
  class EventHandler {
    public:
    EventHandler( const string & code ){ this->code = code; }
    EventHandler( const string & code, function<void(shared_ptr<Event> event)> func ){
      this->code = code;
      this->func = func;
    }

    virtual void Tick( shared_ptr<Event> event ){
      func( event );
    }
    
    function<void(shared_ptr<Event> event)> func;
    string code;
  };
  
  class EventQueue {
    public:
    EventQueue(){}
    EventQueue( const string & code ){ this->code = code; }
    
    virtual void Tick(){}
    virtual shared_ptr<Event> Get(){ return events.front(); }
    virtual void Pop(){ events.pop_front(); }
    virtual bool Empty(){ return events.empty(); }
    virtual void Push(shared_ptr<Event> event){ events.push_back(event); }
    virtual bool Blocked(){ return block; }
    virtual void Press(const int & weight){}
    
    bool active = true;
    bool block  = false;
    string code;
    std::list<std::shared_ptr<Event>> events;
  };

  class EventQueueBasic : public EventQueue {
    public:
    EventQueueBasic(){}
    EventQueueBasic( const string & code ) : EventQueue( code ){}
    
    virtual void Tick(){
      block = false;
      events_limit = events_limit_th;
    }
    
    virtual void Press(const int & weight){
      events_limit -= weight;
      if( events_limit < 0 ) block = true;
    }
    
    virtual void Push(shared_ptr<Event> event){ 
      if( not Empty() ){
        shared_ptr<Event> top_event = Get();
        if( top_event->priority < event->priority ){
          events.push_front(event);
          return;
        }
      }
      events.push_back(event);
    }
    
    int events_limit_th = 100;
    int events_limit = 0;
  };
  
  class EventBus : public pm::PmMsg {
    private:
    map<string, shared_ptr<EventQueue>> queues_map;
    vector<shared_ptr<EventQueue>> queues_vec;
    multimap<string, shared_ptr<EventHandler>> handlers;
    
    public:
    void AddQueue(shared_ptr<EventQueue> queue){
      string code = queue->code;
      auto it = queues_map.find( code );
      if( it != queues_map.end() ){ 
        PMSG_WARNING(code, "already in EventBus, ignore new");
        return;
      }
      queues_map[ code ] = queue;
      queues_vec.push_back( queue );
    }
    
    void AddHandler(string event_code, shared_ptr<EventHandler> handler){
      handlers.insert( std::pair<string,shared_ptr<EventHandler>>(event_code, handler) );
    }
    
    void PopHandler(string event_code, string handler_code){
      auto ret = handlers.equal_range( event_code );
      for (auto it=ret.first; it!=ret.second; ++it){
        shared_ptr<EventHandler> handler = it->second;
        if (handler->code != handler_code) continue;
        handlers.erase( it );
        PMSG_DEBUG("remove", event_code, handler_code, "ok");
        return;
      }
      PMSG_WARNING("remove", event_code, handler_code, "can't find");
    }
    
    void ProcessEvent( shared_ptr<Event> event ){
      const string & event_code = event->event_code;
      PMSG_DEBUG("event_code =", event_code);
      // get handler, give him event data and call
      auto ret = handlers.equal_range(event_code);
      for (auto it=ret.first; it!=ret.second; ++it){
        shared_ptr<EventHandler> handler = it->second;
        handler->Tick( event );
      }
    }
    
    int ProcessQueue( shared_ptr<EventQueue> queue ) {
      PMSG_DEBUG("queue->code =", queue->code);
      queue->Tick();
      int events = 0;
      while( not queue->Empty() ){
        if( queue->Blocked() ) break;
        shared_ptr<Event> event = queue->Get();
        queue->Press( event->weight );
        ProcessEvent( event );
        queue->Pop();
        events++;
      }
      return events;
    }
    
    void Tick(){
      for(int i = 0, i_max = queues_vec.size(); i < i_max; ++i){
        shared_ptr<EventQueue> queue = queues_vec[i];
        if( not queue->active ) continue;
        int events_processed = ProcessQueue( queue );
      }
    }
    
    bool AddEvent( shared_ptr<Event> event ){
      const string & code = event->queue_code;
      auto it = queues_map.find( code );
      if( it == queues_map.end() ){ 
        PMSG_WARNING(code, "no such queue");
        return false;
      }
      it->second->Push( event );
      return true;
    }
  };
  
  class EventBusFactory {
  
    bool AddEventQueue( shared_ptr<EventBus> bus, string name, string type){
      shared_ptr<EventQueue> que;
      if( type == "EventQueueBasic" ){
          que = make_shared<EventQueueBasic>( name );
      } else return false;
      bus->AddQueue( que );
      return true;
    }
    
    shared_ptr<EventBus> BuildEventBus(  ){
      
    }
    
  };
  
}

#endif







