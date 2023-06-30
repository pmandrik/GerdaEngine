// P.~Mandrik, 2023, https://github.com/pmandrik/ShtaEngine

#include "iostream"
#include "ShtaDialogs.hh"

using namespace std;
using namespace ge;

int main(){

  pm::DEFAULT_VERBOSE_LEVEL = pm::verbose::VERBOSE;
  cout << "=========================== Test 0. Dialog<string, string> constructor & functions" << endl;
  Dialog<string, string> dialog;
  
  //===========================
  if( dialog.IsStateNone() != true ){
    cout << "initial state must be \"none\"" << endl;
    return 1;
  }
  cout << "constructor ok" << endl;
  
  //===========================
  dialog.AddSpeech("A", "A", "", "{A}");
  dialog.AddSpeech("B", "B", "", "{B}");
  dialog.AddSpeech("C", "C", "A", "{C}");
  dialog.AddSpeech("D", "D", "", "{D}");
  
  dialog.AddAnswer("A", "A->B", "A->B", "B", "{A->B}");
  dialog.AddAnswer("A", "A->C", "A->C", "C", "{A->C}");
  
  dialog.AddAnswer("B", "B->D", "B->D", "D", "{B->D}");
  dialog.AddAnswer("B", "B->C", "B->C", "C", "{B->C}");
  
  cout << "setup ok" << endl;
  
  //===========================
  if( dialog.IsStateReady() != true ){
    cout << "next state must be \"ready\"" << endl;
    return 1;
  }
  cout << "setup state ok" << endl;
  
  //===========================
  dialog.Start("A");
  string data = "";
  data += dialog.GetSpeechDataNow();
  data += dialog.GetSpeechAnswersNow()[0];
  data += dialog.GetSpeechAnswersNow()[1];
  
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
  if( dialog.GetMarks() != "{A}{A->B}{B}{B->C}{C}{A}{A->B}{B}{B->D}{D}" ){
    cout << "marks are incoreect = " << dialog.GetMarks() << endl;
    
    const vector<string> records = dialog.GetRecords();
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
  cout << "=========================== Test 1. DialogTypeDefault & DialogProxy" << endl;
  int tick_per_data_unit = 10;
  int freeze_time = 10;
  DialogProxy dproxy(tick_per_data_unit, freeze_time);
  dproxy.SetDialog(5, 5); // 5 speach symbols, 5 answers // 5 * 10 = 50 ticks
  for(int i = 0; i < 25; i++) dproxy.Tick( false, pm::v2());
  float fanswer = dproxy.GetDataFraction();
  if( abs(fanswer - 25./tick_per_data_unit/5.) > 0.000001 ){
    cout << "DialogProxy.GetDataFraction (1) not correct = " << fanswer << endl;
    return 1;
  }
  cout << "DialogProxy state ok = " << fanswer << endl;
  
  dproxy.Tick( true, pm::v2()); // now must be 1.
  fanswer = dproxy.GetDataFraction();
  if( abs(fanswer - 1.) > 0.000001 ){
    cout << "DialogProxy.GetDataFraction (2) not correct = " << fanswer << endl;
    return 1;
  }
  cout << "DialogProxy state ok = " << fanswer << endl;
  
  if( dproxy.GetAnswer() != 0 ){    
    cout << "DialogProxy.GetAnswer() not correct == " << dproxy.GetAnswer() << endl;
    return 1;
  }
  for(int i = 0; i < 25; i++) dproxy.Tick( false, pm::v2());
  
  dproxy.Tick( true, pm::v2(0, 1));
  dproxy.Tick( true, pm::v2(0, 1));
  if( dproxy.GetAnswer() != 2 ){    
    cout << "DialogProxy.GetAnswer() not correct == " << dproxy.GetAnswer() << endl;
    return 1;
  }
  
  Dialog<dialog_data, dialog_data> dialog_dtf;
  dialog_dtf.AddSpeech("A", dialog_data("some long long long text A"), "", "{A}");
  dialog_dtf.AddSpeech("B", dialog_data("some long long long text text B"), "", "{B}");
  dialog_dtf.AddSpeech("C", dialog_data("some long long long text text text C"), "A", "{C}");
  dialog_dtf.AddSpeech("D", dialog_data("some long long long text text text text D"), "", "{D}");
  
  dialog_dtf.AddAnswer("A", "A->B", dialog_data("some long long answer A->B"), "B", "{A->B}");
  dialog_dtf.AddAnswer("A", "A->C", dialog_data("some long long answer answer answer A->C"), "C", "{A->C}");
  dialog_dtf.AddAnswer("B", "B->D", dialog_data("some long long answer B->D"), "D", "{B->D}");
  dialog_dtf.AddAnswer("B", "B->C", dialog_data("some long long answer answer answer B->C"), "C", "{B->C}");
  
  int line_width = 20;
  string def_width = string(line_width, '-');
  cout << def_width << endl;
  DialogTypeDefault dtf( line_width );
  dtf.dialog = dialog_dtf;
  dtf.proxy = dproxy;
  dtf.Reset();
  
  cout << "constructors ok" << endl;
  for(int i = 0; i < 35; i++) dtf.Tick( false, pm::v2() );
  if( dtf.GetTextWrapped() != string("some long long long text A").substr(0, 35/tick_per_data_unit) ){
    cout << "dtf.GetTextWrapped() not correct = " << dtf.GetTextWrapped() << endl;
    return 1;
  }
  cout << "check 0 ok" << endl;
  
  dtf.Tick( true, pm::v2(0, 1) );
  for(int i = 0; i < freeze_time*2; i++) dtf.Tick( false, pm::v2() );
  dtf.Tick( true, pm::v2(0, 0) );
  for(int i = 0; i < 9999; i++) dtf.Tick( false, pm::v2() );
  dtf.Tick( true, pm::v2() );
  for(int i = 0; i < 9999; i++) dtf.Tick( false, pm::v2() );
  dtf.Tick( true, pm::v2() );
  for(int i = 0; i < 9999; i++) dtf.Tick( false, pm::v2() );
  dtf.Tick( true, pm::v2() );
  for(int i = 0; i < 9999; i++) dtf.Tick( false, pm::v2() );
  dtf.Tick( true, pm::v2() );
  //dtf.Tick( true, pm::v2() );
  
  if( dtf.dialog.GetMarks() != "{A}{A->C}{C}{A}{A->B}{B}{B->D}{D}"){
    cout << "check 1 not ok, incorrect dialog marks " << dtf.dialog.GetMarks() << endl;
    return 1;
  }
  cout << "check 1 ok" << endl;
  
  return 0;
}

// sudo apt install lcov
// cmake ../GerdaEngine  
// ctest --output-on-failure -C
// lcov -c -d CMakeFiles/TestShtaDialogsHLT.dir/Tests/. -o lcov.info
// genhtml lcov.info
