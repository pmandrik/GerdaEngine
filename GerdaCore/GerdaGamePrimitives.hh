#ifndef GERDA_GAME_PRIMITIVES_HH 
#define GERDA_GAME_PRIMITIVES_HH 1

/// in this file we define some classes that could be usefull for the GAME mechanic directly

namespace ge {
  
  using namespace pm;
  using namespace std;
  using namespace tinyxml2;

  class TrackMaster {
    /// it is a set of Tracks with <string> namespace
    /// 
    vector<Track> tracks;
    vector<string> tracks_names;
    string name;
    
    int GetTrackIndex(std::string name){
      if( not name.size() ) return -1;
      for(int i = 0; i < tracks.size(); ++i){
        if( tracks_names[i] == name ) return i;
      }
      return -1;
    }
    
    public:
    
    void Draw(){
    }
    
    void Pop(){
      if( not tracks.size() ) return;
      tracks.pop_back();
      tracks_names.pop_back();
    }
    
    void StartNewTrack( string name = "" ){
      if( not name.size() ) name = to_string( tracks.size() );
      tracks.push_back( Track() );
      tracks_names.push_back( name );
    }
    
    void ResetTrack( string name = "" ){
      if( not tracks.size() ) return;
      int index = GetTrackIndex( name );
      if( index == -1 ){
        tracks[ tracks.size() - 1 ].Clear();
        return;
      }
      tracks[ index ].Clear();
    }
    
    void RemoveLastPoint(string name = ""){
      if( not tracks.size() ) return;
      int index = GetTrackIndex( name );
      if( index == -1 ) index = tracks.size() - 1;
      tracks[ index ].Pop();
    }
    
    void AddPoint( v2 pos, string name = "" ){
      if( not tracks.size() ) return;
      int index = GetTrackIndex( name );
      if( index != -1 )
        tracks[ index ].Add( pos );
      else 
        tracks[ tracks.size() - 1 ].Add( pos );
    }
    
    void DumpTrack(const Track & track){
      for(int i = 0; i < track.points.size(); i++){
        msg( track.points[i] );
      }
    }
    
    void Dump(){
      msg("DUMP TRACKS ... ");
      for( int i = 0; i < tracks.size(); ++i){
        msg( "TRACK", tracks_names[i] );
        DumpTrack( tracks[i] );
      }
    }
    
    void Load(){
    }
  };
  
};
  
#endif
