// P.~Mandrik, 2023, https://github.com/pmandrik/ShtaEngine

#include "iostream"
#include "pmlib_msg.hh"
// #include "ShtaUI.hh"

using namespace std;
//using namespace shta;

int main(){

  pm::DEFAULT_VERBOSE_LEVEL = pm::verbose::VERBOSE;
  cout << "=========================== Test 0. " << endl;
  
  
  return 0;
}

// sudo apt install lcov
// cmake ../GerdaEngine  
// ctest --output-on-failure -C
// lcov -c -d CMakeFiles/TestShtaDialogsHLT.dir/Tests/. -o lcov.info
// genhtml lcov.info
