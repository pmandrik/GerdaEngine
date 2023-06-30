// P.~Mandrik, 2023, https://github.com/pmandrik/ShtaEngine

#include "iostream"
#include "ShtaEvents.hh"

using namespace std;
using namespace shta;

int main(){

  pm::DEFAULT_VERBOSE_LEVEL = pm::verbose::VERBOSE;
  EventBus bus;
  
  string answer;
  
  shared_ptr<EventQueueBasic> q1 = make_shared<EventQueueBasic>("q1");
  shared_ptr<EventQueueBasic> q2 = make_shared<EventQueueBasic>("q2");
  bus.AddQueue( q1 );
  bus.AddQueue( q2 );
  
  shared_ptr<EventHandler> h1 = make_shared<EventHandler>("h1", 
    [ & answer ](shared_ptr<Event> event) { 
      answer += "h1(e1)"; 
    }
  );
  bus.AddHandler( "e1", h1 );
  
  shared_ptr<EventQueueBasic> e1 = make_shared<EventQueueBasic>("q1");
  bus.AddEvent( make_shared<Event>("e1", "q1") );
  bus.AddEvent( make_shared<Event>("e1", "q1") );
  bus.Tick();
  bus.Tick();
  
  cout << "answer = " << answer << " h1(e1)h1(e1)" << endl;
  if( answer != "h1(e1)h1(e1)" ) return 1;
  
  return 0;
}

// sudo apt install lcov
// cmake ../GerdaEngine  
// ctest --output-on-failure -C
// lcov -c -d CMakeFiles/TestShtaDialogsHLT.dir/Tests/. -o lcov.info
// genhtml lcov.info
