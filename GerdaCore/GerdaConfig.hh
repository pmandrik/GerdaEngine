// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GERDA_CONFIG_HH 
#define GERDA_CONFIG_HH 1

namespace ge {

  using namespace pm;
  using namespace std;
  using namespace tinyxml2;

  struct ConfigItem {
    /// Store data to create Gerda classes, something like:
    /// type, parameters -> item = new type(parameters['name1'], parameters['name2'], ... )
    /// item.field[0] = data['filed'][0] etc
    string type;
    map<string, string> attributes;
    map<string, vector< ConfigItem> > data;
    bool valid = true;

    void Print( int n_tabs = 0 ) const {
      string ntabs = string(n_tabs, ' ');
      string ntabs2 = string(n_tabs+2, ' ');
      msg(ntabs, __PFN__);
      msg(ntabs,"attributes:");
      for( auto iter : attributes )
        msg(ntabs2, iter.first, iter.second );

      msg(ntabs, "child data attributes:");
      for( auto iter : data ){
        msg(ntabs2, iter.first, ":");
        for( auto item : iter.second ) item.Print( n_tabs + 2 );
      }
    }

    /// Add Attribute & Data
    void AddAttribute(string name, string value){
      attributes[ name ] = value;
    }

    void AddData(string name, ConfigItem value){
      data[ name ].push_back( value );
msg("!!!");
Print(10);
    }

    bool Merge( ConfigItem & other ){
      for( auto iter = other.attributes.begin(); iter != other.attributes.end(); ++iter )
        AddAttribute( iter->first, iter->second );
      
      for( auto iter = other.data.begin(); iter != other.data.end(); ++iter ){
        for( auto data_iter = iter->second.begin(); data_iter != iter->second.begin(); ++data_iter )
          AddData( iter->first, *data_iter );
      }
    }

    /// Get Attribute as string, int, float
    string GetAttribute(string name, string def = "") const {
      return pm::map_get( attributes, name, def );
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

    /// Get ConfigItem from Data
    vector<ConfigItem> GetData(string name) const {
      return pm::map_get( data, name, vector<ConfigItem>() );
    }

    vector<string> GetVectorOverData(string name, string attr) const {
      /// Get ConfigItem from Data
      vector<string> answer;
      vector<ConfigItem> rels = GetData( name );
      for( auto rel : rels ){
        answer.push_back( rel.GetAttribute( attr ) );
      }
      return answer;
    }
  };

  std::string get_from_configs(ConfigItem a, std::string name_a, ConfigItem b, std::string name_b){
    std::string a_attr = a.GetAttribute( name_a );
    if( a_attr.size() ) return a_attr;
    std::string b_attr = b.GetAttribute( name_b );
    return b_attr;
  }

  class Config : public BaseClass {
    /// Define interface to load config from filesystem.
    /// 
    map<string, string> type_vs_id;
    map< string, vector<ConfigItem> > items;
    ConfigItem sys;

    protected:
    map<string, vector<string> > load_attributes_req;
    map<string, vector<string> > load_attributes_extra;
  
    public:
    vector<ConfigItem> & GetItems(std::string type){
      return items[ type ];
    }

    ConfigItem & GetSysItem(){ return sys; }

    string GetSysAttribute( string name ){
      return GetSysItem().GetAttribute( name );
    }

    int GetSysAttributeI( string name, int def = 0 ){
      return GetSysItem().GetAttributeI( name );
    }

    int GetSysAttributeF( string name, int def = 0 ){
      return GetSysItem().GetAttributeF( name );
    }

    void AddItem( ConfigItem & item ){
      MSG_DEBUG(__PFN__, "add item of type = ", item.type);
      items[ item.type ].push_back( item );
    }

    void RemoveItems( string type ){
      MSG_DEBUG(__PFN__, "remove items of type = ", type);
      items[ type ] = vector<ConfigItem>();
    }

    void SetLoadAttributes( string name, vector<string> attrs, vector<string> attrs_extra = {} ){
      load_attributes_req[ name ] = attrs;
      load_attributes_extra[ name ] = attrs_extra;
    }

    void ParseCFG(){ //FIXME move away from code
      /// Define specification to load Images, Textures, Shader etc
      /// and define attributes to load
      LoadItems("sys", {}, {"screen_width", "screen_height", "image_imp", "shader_imp"} );
      if( items.find("sys") != items.end() ){   
        auto sys_items = items.find("sys")->second;
        if( sys_items.size() ) sys = sys_items[0];
        for( int i = 1; i < sys_items.size(); ++i )
          sys.Merge( sys_items[1] );
      }

      /// per Item we have many internal items with attributes defined by SetLoadAttributes
      SetLoadAttributes( "var", {"value"}, {} );
      SetLoadAttributes( "relation", {"value"}, {} );
      SetLoadAttributes( "dqd", {"pos_x", "pos_y", "size_x", "size_y"}, {"tid", "tpos_x", "tpos_y", "tsize_x", "tsize_y", "angle", "flip_x", "flip_y"} );

      /// class Image, "name" - string id, "path" to local file
      LoadItems("image", {"id", "path"}, {"imp"} );
      /// class Texture, "name" - string id, "image" - id of image to use, "image_path" - load and create Image using this path if not loaded
      LoadItems("texture", {"id", "image_id"}, {"image_path", "image_imp"} );
      /// class Shader, "vert" - path to vertex shader, "frag" - path to fragment shader, "var" - shader parameters (floats)
      LoadItems("shader", {"id", "vert", "frag"}, {"imp", "kind", "time"});
      /// class "drawer"
      LoadItems("drawer", {"id", "texture_id"}, {"imp", "array_size"});
      /// class "slaFB", "slaQD"
      LoadItems("slaFB", {"id"}, {"shader_id"});
      LoadItems("slaQD", {"id", "drawer_id"}, {"shader_id"});
      LoadItems("slaTD", {"id", "texture_id"}, {"shader_id"});
      LoadItems("slaChain", {"id"}, {});
      
      /// init unic id attribute name used to check duplicates      
      type_vs_id["image"]  = "id";
      type_vs_id["texture"] = "id";
      type_vs_id["shader"] = "id";
      type_vs_id["qdrawer"] = "id";

      CheckQuality();
    }

    virtual void LoadItems(string name, vector<string> attributes, vector<string> attributes_extra = {}){
      /// load items as SLaConfigItem from cfg, check mandatory attributes, extra attributes and one deep level attributes
    }

    virtual void ReadCFG(){
      /// read cfg file from file system
    }

    void CheckQuality(){
      map<string, string> type_vs_id;
      for(auto it = type_vs_id.begin(); it != type_vs_id.end(); ++it){
        vector<ConfigItem> & slas = items[it->first];
        string key = it->second;
        map<string, ConfigItem> counter;
        for(auto sla : slas){
          auto attr_it = sla.attributes.find( key );
          if( attr_it == sla.attributes.end() ){
          }
          auto val = attr_it->second;
          
          auto counter_iter = counter.find( val );
          if(counter_iter != counter.end()){
            MSG_WARNING(__PFN__, "found duplicate of ConfigItem type \"" + it->first + "\", with key attribute \"" + key + "\" = \"" + val + "\"" );
            continue;
          }
          counter[ val ] = sla;
        }
      }
    }

    void Merge( std::shared_ptr<Config> other ){
      for(auto it = other->items.begin(); it != other->items.end(); ++it){
        vector<ConfigItem> & v = items[it->first];
        v.insert(v.begin(), it->second.begin(), it->second.end());
      }
      CheckQuality();
    }
  };

  class XMLConfig : public Config {
    /// GerdaConfig implementation using XML imput parced with tinyxml
    public:
    std::string cfg_path;
    int state = 0;

    XMLConfig( string path ) : Config() {
      ReadCfg( path );
      ParseCFG();
    }

    XMLDocument doc;
    bool ReadCfg( string cfg_path_ ){
      cfg_path = cfg_path_;
      MSG_DEBUG(__PFN__, "load", cfg_path);
      XMLError error = doc.LoadFile( cfg_path.c_str() );
      if( error ){
        check_tinyxml_error(error, cfg_path);
        state = 1;
        return false;
      }
    }

    ConfigItem LoadConfigItem( string name, vector<string> & attributes, vector<string> & attributes_extra, XMLElement * shaders_el ){
      /// Load single Items pointed by XMLElement *
      ConfigItem item;
      item.type = name;

      for(auto attr : attributes){
        const char * value = shaders_el->Attribute( attr.c_str() );
        if(not value) {
          msg_err(__PFN__, "find " + name + " without \"" + attr + "\" Attribute");
          item.valid = false;
          continue;
        } MSG_DEBUG(__PFN__, "find attribute", attr);
        item.AddAttribute(attr, value);
      }

      for(auto attr : attributes_extra){
        const char * value = shaders_el->Attribute( attr.c_str() );
        if(not value) continue;
        MSG_DEBUG(__PFN__, "find attribute", attr);
        item.AddAttribute(attr, value);
      }

      return item;
    }

    void LoadItems( string name, vector<string> attributes, vector<string> attributes_extra = {} ){
      /// Load Items from XML
      MSG_DEBUG(__PFN__, "load items of type", name);
      for(XMLElement* shaders_el = doc.FirstChildElement( name.c_str() ); shaders_el; shaders_el = shaders_el->NextSiblingElement( name.c_str() )){
        ConfigItem item = LoadConfigItem( name, attributes, attributes_extra, shaders_el );

        for( auto attr_req_iter : load_attributes_req ){
          string attr = attr_req_iter.first;
          vector<string> & attrs_req = attr_req_iter.second;
          auto attr_extra_iter = load_attributes_extra.find( attr );
          vector<string> & attrs_extra = attr_extra_iter->second;

          for(XMLElement* var_el = shaders_el->FirstChildElement( attr.c_str() ); var_el; var_el = var_el->NextSiblingElement( attr.c_str() )){
            MSG_DEBUG(__PFN__, "found child element", attr);
            ConfigItem child_item = LoadConfigItem( attr, attrs_req, attrs_extra, var_el );
            MSG_DEBUG(__PFN__, "add child element", attr);
            item.AddData( attr, child_item );
          }
        }

        AddItem( item );
      }
    }
  }; 

}
#endif
