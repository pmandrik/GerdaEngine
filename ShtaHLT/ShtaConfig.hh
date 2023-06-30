// P.~Mandrik, 2023, https://github.com/pmandrik/ShtaEngine
#ifndef SHTA_CONFIG_HH 
#define SHTA_CONFIG_HH 1

#include <map>
#include <vector>
#include <string>
#include <list>
#include <memory>
#include <functional>

#include "pmlib_other.hh"
#include "pmlib_msg.hh"
#include "pmlib_v2d.hh"
#include "pmlib_math.hh"

namespace shta {

  using namespace std;

  class ConfigElement : public pm::PmMsg {
    public:
    /// Store data to create Shta classes, something like:
    /// type, parameters -> item = new type(parameters['name1'], parameters['name2'], ... )
    /// item.field[0] = data['filed'][0] etc
    string type;
    map<string, string> attributes;
    map<string, vector< ConfigElement> > childs;
    bool valid = true;
    
    //! Add Attribute & Data
    void AddAttribute(string name, string value){
      PMSG_DEBUG( name, value );
      auto it = attributes.find( name );
      if( it != attributes.end() ){
        PMSG_WARNING( "overwrite Attribute", name, it->second, "with new vaue", value );
      }
      attributes[ name ] = value;
    }

    void AddChild(string name, ConfigElement value){
      PMSG_DEBUG( name );
      childs[ name ].push_back( value );
    }

    bool Merge( ConfigElement & other ){
      for( auto iter = other.attributes.begin(); iter != other.attributes.end(); ++iter )
        AddAttribute( iter->first, iter->second );
      
      for( auto iter = other.childs.begin(); iter != other.childs.end(); ++iter ){
        for( auto data_iter = iter->second.begin(); data_iter != iter->second.end(); ++data_iter )
          AddChild( iter->first, *data_iter );
      }
    }
    
    //! Get Attribute as string, int, float
    string GetAttribute(string name, string def = "") const {
      return pm::map_get( attributes, name, def );
    }

    string GetAttributeUpper(string name, string def = "") const {
      string answer = pm::map_get( attributes, name, def );
      return pm::upper_string(answer);
    }

    int GetAttributeI(string name, int def = 0) const {
      string attr = GetAttribute( name );
      if( not attr.size() ) return def;
      return atoi( attr.c_str() ); //FIXME
    }

    float GetAttributeF(string name, float def = 0.f) const {
      string attr = GetAttribute( name );
      if( not attr.size() ) return def;
      return atof( attr.c_str() ); //FIXME
    }
    
    //! Get ConfigElement from childs
    vector<ConfigElement> GetElement(string name) const {
      return pm::map_get( childs, name, vector<ConfigElement>() );
    }

    vector<string> GetChild(string name, string attr) const {
      /// Get ConfigElement from Childs
      vector<string> answer;
      vector<ConfigElement> rels = GetElement( name );
      for( auto rel : rels ){
        answer.push_back( rel.GetAttribute( attr ) );
      }
      return answer;
    }
    
    //! print config item content
    void Print( int n_tabs = 0, bool print_pfn = true ) const {
      string ntabs = string(n_tabs, ' ');
      string ntabs2 = string(n_tabs+2, ' ');
      if( print_pfn ) pm::msg(ntabs, __PFN__);
      pm::msg(ntabs,"attributes :", attributes.size() );
      for( auto iter : attributes )
        pm::msg(ntabs2, "<"+ iter.first + ",", iter.second+ ">" );

      pm::msg(ntabs, "childs :", childs.size() );
      for( auto iter : childs ){
        pm::msg(ntabs2, "<"+ iter.first+ ">", iter.second.size() );
        for( auto item : iter.second ) item.Print( n_tabs+2+2, false );
      }
    }
  };

  class Config : public pm::PmMsg {
    /// Define interface to load config from filesystem.
    map< string, vector<ConfigElement> > elements;
    
    public:
    vector<ConfigElement> & GetElements(std::string type){
      return elements[ type ];
    }
  };
  
  class ConfigDefault : public Config {
    ConfigElement sys;
    ConfigElement & GetSysElement(){ return sys; }

    string GetSysAttribute( string name, string def = "" ){
      return GetSysElement().GetAttribute( name, def );
    }
    string GetSysAttributeUpper( string name, string def = "" ){
      return GetSysElement().GetAttributeUpper( name, def );
    }
    int GetSysAttributeI( string name, int def = 0 ){
      return GetSysElement().GetAttributeI( name, def );
    }
    float GetSysAttributeF( string name, float def = 0 ){
      return GetSysElement().GetAttributeF( name, def );
    }
  };
}

#endif







