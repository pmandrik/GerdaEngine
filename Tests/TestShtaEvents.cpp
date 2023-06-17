// P.~Mandrik, 2023, https://github.com/pmandrik/ShtaEngine

#include "iostream"
#include "ShtaEvents.hh"

using namespace std;
using namespace shta;

int main(){

  pm::DEFAULT_VERBOSE_LEVEL = pm::verbose::VERBOSE;
  EventBus bus;
  
  shared_ptr<EventQueueBasic> q1 = std::make_shared<EventQueueBasic>("q1");
  shared_ptr<EventQueueBasic> q2 = std::make_shared<EventQueueBasic>("q2");
  bus.AddQueue( q1 );
  bus.AddQueue( q2 );
  
  shared_ptr<EventQueueBasic> q1 = std::make_shared<EventQueueBasic>("q1");
  string event_code, shared_ptr<EventHandler> handler
  bus.AddHandler( "e1",  );
  
  bus.Tick();
  
  return 0;
}

// sudo apt install lcov
// cmake ../GerdaEngine  
// ctest --output-on-failure -C
// lcov -c -d CMakeFiles/TestShtaDialogsHLT.dir/Tests/. -o lcov.info
// genhtml lcov.info
