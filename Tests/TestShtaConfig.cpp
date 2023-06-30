// P.~Mandrik, 2023, https://github.com/pmandrik/ShtaEngine

#include "iostream"
#include "ShtaConfig.hh"

using namespace std;
using namespace shta;

int main(){

  pm::DEFAULT_VERBOSE_LEVEL = pm::verbose::VERBOSE;
  ConfigElement item1, item2;
  
  item1.AddAttribute("attr_1", "value_1");
  item2.AddAttribute("attr_2", "value_2");
  item1.AddChild("data", item2);
  
  item2.Print();
  item1.Print();
  
  item2.Merge( item1 );
  item1.Merge( item2 );
  
  item2.Print();
  item1.Print();
  
  if( item1.attributes.size() != 2 ) return 1;
  if( item1.childs.size() != 1 ) return 1;
  if( item1.childs.begin()->second.size() != 2 ) return 1;
  
  return 0;
}

// sudo apt install lcov
// cmake ../GerdaEngine  
// ctest --output-on-failure -C
// lcov -c -d CMakeFiles/TestShtaDialogsHLT.dir/Tests/. -o lcov.info
// genhtml lcov.info
