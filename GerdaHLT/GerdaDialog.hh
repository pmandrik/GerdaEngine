// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GerdaDialogs_H
#define GerdaDialogs_H 1 

struct DialogAnswer{
  /// answer text + event string name
  DialogAnswer(string text_, string event_name_) : text(text_), event_name(event_name_) {}
  string text;
  string event_name;
};

struct DialogAtom{
  /// dialog text + several DialogAnswer answers 
  DialogAtom(string dialog_text_) : dialog_text(dialog_text_) {}
  DialogAtom(string dialog_text_, vector<DialogAnswer> dialog_answers_) : dialog_text(dialog_text_), dialog_answers(dialog_answers_) {}
  string dialog_text;
  vector<DialogAnswer> dialog_answers;
};

class DialogLine{
  /// several DialogAtom's
  public:
  DialogLine(string name_="") : current(-1), name(name_) {}

  DialogAtom * Next(){
    /// switch to next DialogAtom replic and return it
    current++;
    return Get();
  }

  DialogAtom * Get(){
    /// return current DialogAtom or nullptr
    if(current >= dialog_atoms.size() ){
      current = -1;
      return nullptr;
    }
    return & dialog_atoms.at( current );
  }

  void AddAtom(string dialog_text, const vector<string> & dialog_answers_text, const vector<string> & dialog_answers_event){
    DialogAtom atom (dialog_text);
    for(int i = 0; i < dialog_answers_text.size(); ++i){
      string text  = dialog_answers_text.at( i );
      string event = dialog_answers_event.at( i );
      atom.dialog_answers.emplace_back( DialogAnswer(text, event) );
    }
    dialog_atoms.emplace_back( atom );
  }

  int current;
  vector<DialogAtom> dialog_atoms;
  string name;
};

#endif



