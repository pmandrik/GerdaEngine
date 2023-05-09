// P.~Mandrik, 2023, https://github.com/pmandrik/ShtaEngine

#ifndef GERDA_DIALOGS_HH 
#define GERDA_DIALOGS_HH 1

#include <map>
#include <vector>
#include <string>

namespace ge {

  using namespace std;

  // @@@Decoupled from ShtaEngine@@@
  // Dialog() machine
  //  - in general we are working with abstract "dialog entry" = <DialogEntry id="" data=... next_id="" mark=""> and <DialogAnswer id="" data="" next_id="" mark=""/>
  //  e.g dialog data is string, dialog answers and vector<string>
  //  - D GetData() -> return dialog data
  //  - vector<A> GetAnswers() -> return dialog answers data
  //  - PutAnswer(int id) -> give answer to dialog, switching dialog to another "dialog entry", switching state to "new"
  //  - bool MoveTo(string id) -> move to dialog entry with id
  //  - GetState() -> 
  //      "none" when no "dialog entry" available
  //      "ready" - before first call Start()
  //      "active" - has dialog entry available
  //      "end" - no more entries available in the dialog tree
  //  - vector <string> GetRecord() -> return record of all events as vector< pair<string,string> > of "dialog entry" ids, answer ids 
  //  - GetMarks() -> return record of all events as vector<string> of "dialog answers" marks, where mark is from answer
  //  - Start(string id) -> Reset(), 
  //  - Reset() -> clear recors and marks, MoveTo() id used with Start call
  
  // DialogFactory() 
  //  - return Dialog() class filled with DialogEntry() items from input format
  //  - e.g. from XMLs:
  //      <DialogEntry id="" data="" next_id="" mark="">
  //        <DialogAnswer id="" data="" next_id="" mark=""/>
  //      </DialogEntry>
  //    in general, DialogEntry.data and DialogAnswer.data may have different ay types D & A
  //
  // 
  
  template<class A>
  class DialogAnswer{
    string id, next_id, mark;
    A data;
  };

  template<class D>
  class DialogEntry {
    string id, next_id, mark;
    D data;
    vector<string> answer_ids;
  };

  template<class A, class D>
  class Dialog {
    map<string, DialogAnswer<A>> answers;
    map<string, DialogEntry<D>>  entries;
    DialogEntry<D> active_entry;
    
    vector<string> records;
    void Record(string event){
      records.push_back( event );
    }
    void Record(const DialogAnswer<A> & answer){
      records.push_back( "answer:" + answer.id );
      marks += answer.mark;
    }
    void Record(const DialogEntry<D> & entry){
      records.push_back( "entry:" + entry.id );
      marks += entry.mark;
    }
  
    public:
    string state = "none", entry_id, marks;
    
    const string GetState() const { return state; }
    const vector<string> GetRecord() const { return records; }
    const string GetMarkRecord() const { return marks; }
    
    void AddAnswer(string entry_id, string id, A & data, string next_id, string mark){
      answers[ id ] = {
        .id = id,
        .next_id = next_id,
        .mark = mark,
        .data = data
      };
      auto de = entries.find( entry_id );
      if( de == entries.end() ) entries.answer_ids.push_back( id );
    }
    
    void AddEntry(string id, D & data, string next_id, string mark){
      if( state == "none" ) state = "ready";
      entries[ id ] = {
        .id = id,
        .next_id = next_id,
        .mark = mark,
        .data = data
      };
    }
    
    D GetData(string id) const { return entries[id].data; };
    A GetAnswerData(string id) const { return answers[id].data; };
    
    vector<A> GetAnswers(string id) const {
      vector<A> answers;
      vector<string> answer_ids = entries[id].answer_ids;
      for(auto aid : answer_ids){
        answers.push_back( GetAnswerData(aid) );
      }
      return answers;
    }
    
    D GetData() const { return active_entry.data; }
    vector<A> GetAnswers() const { return GetAnswers( active_entry.id ); }
    
    void PutAnswer(unsigned int index){
      if( index >= active_entry.answer_ids.size() ) return;
      string id = active_entry.answer_ids[index];
      
      auto answer = answers[id];
      Record( answer );
      
      // check if have default next dialog entry
      if( MoveTo( answer.next_id ) ) return;
      
      // check if have default next dialog entry
      if( MoveTo( active_entry.next_id ) ) return;
      
      // end of dialog
      End();
    }
    
    bool MoveTo(string id) const {
      auto it = entries.find( id );
      if( it == entries.end() ) return false;
      active_entry = it->second;
      Record( id );
      state = "active";
      return true;
    }
    
    void Start(string entry_id_){
      entry_id = entry_id_;
      Reset();
    }
    
    void Reset(){
      records.clear();
      marks = "";
      MoveTo( entry_id );
    }
    
    void End(){
      Record("end");
      active_entry = DialogEntry<D>();
      state = "done";
    }
    
  };

}  

#endif






