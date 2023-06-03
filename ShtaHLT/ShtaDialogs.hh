// P.~Mandrik, 2023, https://github.com/pmandrik/ShtaEngine

#ifndef GERDA_DIALOGS_HH 
#define GERDA_DIALOGS_HH 1

#include <map>
#include <vector>
#include <string>

#include "pmlib_other.hh"
#include "pmlib_msg.hh"
#include "pmlib_v2d.hh"

namespace ge {

  using namespace std;

  // @@@Decoupled from ShtaEngine@@@
  // Dialog() machine
  //  - in general we are working with abstract "dialog Speech" = <DialogSpeech id="" data=... next_id="" mark=""> and <DialogAnswer id="" data="" next_id="" mark=""/>
  //  e.g dialog data is string, dialog answers and vector<string>
  //  - D GetData() -> return dialog data
  //  - vector<A> GetAnswers() -> return dialog answers data
  //  - PutAnswer(int id) -> give answer to dialog, switching dialog to another "dialog Speech", switching state to "new"
  //  - bool MoveTo(string id) -> move to dialog Speech with id
  //  - GetState() -> 
  //      "none" when no "dialog Speech" available
  //      "ready" - before first call Start()
  //      "active" - has dialog Speech available
  //      "end" - no more entries available in the dialog tree
  //  - vector <string> GetRecord() -> return record of all events as vector< pair<string,string> > of "dialog Speech" ids, answer ids 
  //  - GetMarks() -> return record of all events as vector<string> of "dialog answers" marks, where mark is from answer
  //  - Start(string id) -> Reset(), 
  //  - Reset() -> clear recors and marks, MoveTo() id used with Start call
  
  // DialogFactory() TODO
  //  - return Dialog() class filled with DialogSpeech() items from input format
  //  - e.g. from XMLs:
  //      <Dialog id="">
  //       <DialogSpeech id="" data="" next_id="" mark="">
  //         <DialogAnswer id="" data="" next_id="" mark=""/>
  //         <DialogAnswer id="" data="" next_id="" mark=""/>
  //         <DialogAnswer id="" data="" next_id="" mark=""/>
  //       </DialogSpeech>
  //       <DialogSpeech id="" data="" next_id="" mark="">
  //         <DialogAnswer id="" data="" next_id="" mark=""/>
  //         <DialogAnswer id="" data="" next_id="" mark=""/>
  //         <DialogAnswer id="" data="" next_id="" mark=""/>
  //       </DialogSpeech>
  //      </Dialog>
  //    in general, DialogSpeech.data and DialogAnswer.data may have different ay types D & A
  
  // DialogProxy(): simple class to 
  //  1. giving the size of the data calculate what is amount of data to show at this point of time if timing per data unit (e.g. letter) is giving
  //  2. catch arrows actions and switch active answer, then when Z action is pressed -> return this answer, also, 
  // Possible usage example:
  // Dialog<string, string> dialog;
  // DialogProxy proxy;
  // ...
  // string data  = d.GetData();
  // data_wrapped = pm::wrap_into_width( data, box_w, letter_w ); <- also add answers data later
  // ...
  // DialogProxy proxy;
  // proxy.SetDialog( data_wrapped.size(), d.GetAnswers().size() );
  // proxy.Tick( false, v2() );
  // data_timed   = data_wrapped.substr( 0, int( data_wrapped.size() * proxy.GetDataFraction()) );
  // ShowOnScreen( data_timed )
  
  // ======= Dialog ====================================================================
  struct DialogEntry {
    DialogEntry(){};
    DialogEntry(string id_, string next_id_, string mark_) : id(id_), next_id(next_id_), mark(mark_) {};
    string id, next_id, mark;
  };
  
  template<class A>
  struct DialogAnswer : public DialogEntry {
    DialogAnswer(){};
    DialogAnswer(string id_, string next_id_, string mark_) : DialogEntry( id_,  next_id_,  mark_) {};
    A data;
  };

  template<class D>
  struct DialogSpeech : public DialogEntry {
    DialogSpeech(){};
    DialogSpeech(string id_, string next_id_, string mark_) : DialogEntry( id_,  next_id_,  mark_) {};
    D data;
    vector<string> answer_ids;
  };

  template<class A, class D>
  class Dialog : public pm::PmMsg {
    map<string, DialogAnswer<A>> answers;
    map<string, DialogSpeech<D>> speechs;
    DialogSpeech<D> active_speech;
    
    vector<string> records;
    void Record( string event ){
      PMSG_DEBUG(event);
      records.push_back( event );
    }
    void Record(const DialogAnswer<A> & answer){
      Record( answer_record_mark + answer.id );
      marks += answer.mark;
    }
    void Record(const DialogSpeech<D> & speech){
      Record( speech_record_mark + speech.id );
      marks += speech.mark;
    }
  
    public:
    Dialog(){};
    
    string answer_record_mark = "answer:";
    string speech_record_mark = "speech:";
    string state = "none", active_speech_id, marks, def_start_speech_id;
    
    void SetStartingSpeech(string id){ def_start_speech_id = id; }
    const string GetState() const { return state; }
    const vector<string> GetRecords() const { return records; }
    const string GetMarks() const { return marks; }
    bool IsStateActive(){ return state == "active"; }
    bool IsStateEnd(){ return state == "end"; }
    bool IsStateNone(){ return state == "none"; }
    bool IsStateReady(){ return state == "ready"; }
    
    void AddAnswer(string speech_id, string id, A data, string next_id="", string mark=""){
      auto de = speechs.find( speech_id );
      if( de == speechs.end() ) {
        PMSG_WARNING("can't find speech_id = ", speech_id, "to assign answer, return");
        return;
      }
      
      auto da = answers.find( id );
      if( da != answers.end() ) {
        PMSG_WARNING("answer with id = ", id, "already exist, return");
        return;
      }
      
      de->second.answer_ids.push_back( id );
      DialogAnswer<A> ans(id, next_id, mark);
      ans.data = data;
      answers[ id ] = ans;
    }
    
    void AddSpeech(string id, D data, string next_id="", string mark=""){
      auto de = speechs.find( id );
      if( de != speechs.end() ) {
        PMSG_WARNING("speech with id = ", id, "already exist, return");
        return;
      }
    
      DialogSpeech<D> ent(id, next_id, mark);
      ent.data = data;
      speechs[ id ] = ent;
      
      /// if it is first speech to add set it as starting entry
      if( state == "none" ) {
        state = "ready";
        SetStartingSpeech(id);
      }
    }
    
    D GetSpeechData(string id) const { 
      auto de = speechs.find( id );
      if( de == speechs.end() ) {
        PMSG_WARNING("can't find speech id = ", id, ", return empty");
        return D();
      }
      return de->second.data; 
    };
    
    A GetAnswerData(string id) const { 
      auto de = answers.find( id );
      if( de == answers.end() ) {
        PMSG_WARNING("can't find answer id = ", id, ", return empty");
        return A();
      }
      return de->second.data; 
    };
    
    vector<A> GetSpeechAnswers(string id) const {
      vector<A> answers;
      auto de = speechs.find( id );
      if( de == speechs.end() ) {
        PMSG_WARNING("can't find speech id = ", id, ", return empty");
        return answers;
      }
      const vector<string> & answer_ids = de->second.answer_ids;
      for(auto aid : answer_ids){
        answers.push_back( GetAnswerData(aid) );
      }
      return answers;
    }
    
    D GetSpeechDataNow() const { return active_speech.data; }
    vector<A> GetSpeechAnswersNow() const { return GetSpeechAnswers( active_speech.id ); }
    
    void PutAnswer(unsigned int index){
      if( index < active_speech.answer_ids.size() ) {
        string id = active_speech.answer_ids[index];
      
        DialogAnswer<A> & answer = answers[id];
        Record( answer );
      
        // will check if have next dialog speech
        if( MoveTo( answer.next_id ) ) return;
      }
      
      // check if have default next dialog speech
      Record( "answer:default" );
      if( MoveTo( active_speech.next_id ) ){
        return;
      }
      
      // end of dialog
      End();
    }
    
    bool MoveTo(string id) {
      PMSG_DEBUG("id", id);
      auto it = speechs.find( id );
      if( it == speechs.end() ) return false;
      active_speech = it->second;
      Record( active_speech );
      state = "active";
      return true;
    }
    
    void Start(string speech_id = ""){
      if(not speech_id.size() ) active_speech_id = def_start_speech_id;
      else                      active_speech_id = speech_id;
      Reset();
    }
    
    void Reset(){
      records.clear();
      marks = "";
      if( not active_speech_id.size() ) active_speech_id = def_start_speech_id;
      MoveTo( active_speech_id );
    }
    
    void End(){
      Record("end");
      active_speech = DialogSpeech<D>();
      active_speech_id = "";
      state = "end";
    }
  };
  
  // ======= DialogFactory ====================================================================
  
  // ======= DialogProxy ====================================================================
  class DialogProxy {
    int tick_per_data_unit = 5, freeze_time = 60;
    int number_of_answers = 0, answer_index = 0;
    pm::Timer freeze_timer, data_timer;
    
    public:
    DialogProxy(){}
    DialogProxy( int tick_per_data_unit_, int freeze_time_ ){
      freeze_time = freeze_time_;
      tick_per_data_unit = tick_per_data_unit_;
      freeze_timer = pm::Timer( freeze_time );
    }
    
    bool Tick( bool Z_action, pm::v2 arrows_actions ){
      bool not_finished = data_timer.TickOneShoot();
      if( freeze_timer.TickOneShoot() ) return false;
    
      // switch active answer
      if( number_of_answers ){
        answer_index += pm::check_sign(arrows_actions.y);
        answer_index = pm::wrap_around(answer_index, 0, number_of_answers);
      }

      // check Z action
      if( Z_action ){
        if( not not_finished ) return true;
        
        data_timer.End();
        freeze_timer.Reset(); 
      }
      
      return false;
    }
    
    int GetAnswer(){
      return answer_index;
    }
    
    float GetDataFraction(){
      return data_timer.ftime;
    }
    
    void SetDialog( int number_of_data_units, int number_of_answers_ ){
      number_of_answers = number_of_answers_;
      answer_index = 0;
      data_timer = pm::Timer( tick_per_data_unit * number_of_data_units );
      freeze_timer.Reset();
    }
  };
  
  // ======= DialogTypeDefault ====================================================================
  struct dialog_data {
    string str, state;
    map<string, string> meta;
    string GetText(){ return str; }
    
    dialog_data(){};
    dialog_data( string str_ ){ str = str_; }
  };
  
  class DialogTypeDefault {
    int box_w;
    
    public:
    string state;
    Dialog<dialog_data, dialog_data> dialog;
    DialogProxy proxy;
    
    DialogTypeDefault( int box_w_ ){
      box_w = box_w_;
    }
    
    string WrapSpeechText( string text ){
      return text + "\n";
    }
    
    string WrapAnswerText( string text, bool active ){
      if( active ) return " > " + text + "\n";
      return "   " + text + "\n";
    }
    
    string GetTextFull(){
      if( not dialog.IsStateActive() ) return "";
      dialog_data data = dialog.GetSpeechDataNow();
      string text = WrapSpeechText( data.GetText() );
      vector<dialog_data> answers = dialog.GetSpeechAnswersNow();
      int active_answer = proxy.GetAnswer();
      for( int i = 0; i < answers.size(); i++ ){ 
        text += WrapAnswerText( answers[i].GetText(), i == active_answer );
      }
      return text;
    }
    
    string GetTextWrapped(){
      string text = GetTextFull();
      string text_wrapped = pm::wrap_into_width( text, box_w, 1 );
      string text_timed = text_wrapped.substr( 0, int( text_wrapped.size() * proxy.GetDataFraction()) );
      return text_timed;
    }
    
    void UpdateData(){
      if( not dialog.IsStateActive() ){
        proxy.SetDialog( 0, 0 );
        return;
      }
    
      string text = GetTextFull();
      vector<dialog_data> answers = dialog.GetSpeechAnswersNow();
      proxy.SetDialog( text.size(), answers.size() );
    }
    
    void Reset(){
      dialog.Reset();
      UpdateData();
      state = dialog.state;
    }
    
    void Start(){
      dialog.Start();
      UpdateData();
      state = dialog.state;
    }
    
    void Tick( bool Z_action, pm::v2 arrows_actions ){
      bool is_answer = proxy.Tick( Z_action, arrows_actions );
      if( is_answer ){
        cout << "Tick() = " << Z_action << " " << is_answer << " " << proxy.GetAnswer() << endl;
        dialog.PutAnswer( proxy.GetAnswer() );
        UpdateData();
        state = dialog.state;
      }
    }
  };
}  

#endif







