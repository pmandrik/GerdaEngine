// P.~Mandrik, 2023, https://github.com/pmandrik/ShtaEngine

#include "iostream"
#include "ShtaDialogs.hh"

using namespace std;
using namespace ge;

int main(){

  Dialog<string, string> dialog;
  
  //===========================
  if( dialog.GetState() != "none" ){
    cout << "initial state must be \"none\"" << endl;
    return 1;
  }
  cout << "constructor ok" << endl;
  
  //===========================
  dialog.AddEntry("A", "A", "", "{A}");
  dialog.AddEntry("B", "B", "", "{B}");
  dialog.AddEntry("C", "C", "A", "{C}");
  dialog.AddEntry("D", "D", "", "{D}");
  
  dialog.AddAnswer("A", "A->B", "A->B", "B", "{A->B}");
  dialog.AddAnswer("A", "A->C", "A->C", "C", "{A->C}");
  
  dialog.AddAnswer("B", "B->D", "B->D", "D", "{B->D}");
  dialog.AddAnswer("B", "B->C", "B->C", "C", "{B->C}");
  
  cout << "setup ok" << endl;
  
  //===========================
  if( dialog.GetState() != "ready" ){
    cout << "next state must be \"ready\"" << endl;
    return 1;
  }
  cout << "setup state ok" << endl;
  
  //===========================
  dialog.Start("A");
  string data = "";
  data += dialog.GetData();
  data += dialog.GetAnswers()[0];
  data += dialog.GetAnswers()[1];
  
  if( data != "AA->BA->C" ){
    cout << "dialog data incorrect " << data << endl;
    return 1;
  }
  cout << "dialog data ok" << endl;
  
  //===========================
  dialog.PutAnswer(0); // move to "eB"
  dialog.PutAnswer(1); // move to "eC"
  dialog.PutAnswer(999); // move to "eA"
  
  //===========================
  if( dialog.GetState() != "active" ){
    cout << "end state must be \"active\" != " << dialog.GetState() << endl;
    return 1;
  }
  cout << "active state ok" << endl;

  //===========================
  dialog.PutAnswer(0); // move to "eB"
  dialog.PutAnswer(0); // move to "eD"
  dialog.PutAnswer(0); // move to "end"

  //===========================
  if( dialog.GetMarkRecord() != "{A}{A->B}{B}{B->C}{C}{A}{A->B}{B}{B->D}{D}" ){
    cout << "marks are incoreect = " << dialog.GetMarkRecord() << endl;
    
    const vector<string> records = dialog.GetRecord();
    cout << "records:" << endl;
    for( auto rec : records ) cout << rec << endl;
  
    return 1;
  }
  cout << "marks are ok" << endl;
  
  //===========================
  if( dialog.GetState() != "end" ){
    cout << "end state must be \"end\" != " << dialog.GetState() << endl;
    return 1;
  }
  cout << "end state ok" << endl;
  
  //===========================
  
  return 0;
}

// sudo apt install lcov
// cmake ../GerdaEngine  
// ctest --output-on-failure -C
// lcov -c -d CMakeFiles/TestShtaDialogsHLT.dir/Tests/. -o lcov.info
// genhtml lcov.info
