// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GerdaSLa_H
#define GerdaSLa_H 1  

namespace ge{

  using namespace pm;
  using namespace std;
  using namespace tinyxml2;

  class FrameBuffer;

  // ==================================================================== SLa shader
  class SLaShader : public Shader {
    /// special case of shader with up to 3 textures and up to 10 variables + 'time'
    public:
    SLaShader(const int & vn = 0, const int & tn = 0) : Shader() {
      var_number = vn;
      text_number = tn;
      for(int i = 0; i < 10; i++) vars_def[i] = 0.0;
      ResetVars();
      ResetTime();
    }

    void Print(string format = "cpp"){
      /// search for following substrings in format string:
      /// "cpp", "xml" - as used in SLa
      if( format.find("cpp") != string::npos ){
        msg( "//", name );
        msg( "//", path_vert );
        msg( "//", path_frag );
        for(int i = 0; i < var_number; i++)
          msg("shader->vars[", i, "] =", vars[i], ";");
      } 
      if ( format.find("xml") != string::npos ) {
        msg_nll( "<shader name=\"" + name + "\"" );
        msg_nll( "vert=\"" + path_vert + "\"" );
        msg_nll( "frag=\"" + path_frag + "\"" );
        msg_nll( ">\n" );
        for(int i = 0; i < var_number; i++)
          msg_nll( "<var value=\"" + to_string( vars[i] ) + "\"/>" );
        msg_nll( "\n</shader>\n" );
      }
      if ( format.find("vector") != string::npos ) {
        msg_nll( "vector<float> vars_" + name + " = {" );
        for(int i = 0; i < var_number; i++)
          msg_nll( to_string( vars[i] ) + "," );
        msg_nll( "};\n" );
      }
    }

    void Reload(){
      // this->TurnOff();
      this->Load(path_vert, path_frag);
    }

    void ResetVars(){ for(int i = 0; i < 10; i++) vars[i] = vars_def[i]; }
    void ResetTime(){ time = time_def; }
    void SetDefVars(const vector<float> & vars_v){ 
      for(int i = 0; i < vars_v.size(); i++) vars_def[i] = vars_v.at(i); 
      var_number = vars_v.size();
    }
    void SetDefTime(const float & t){ time_def = t; }
    void SetVars(const vector<float> & vars_v){ for(int i = 0; i < vars_v.size(); i++) vars[i] = vars_v.at(i); }
    void SetTime(const float & t){ time = t; }

    string GetVarName(const int & index){
      return "var_" + to_string(index) ;
    }

    void UpdateUniforms(){
      // texture
      if(text_number >= 3) UpdateUniformTexture(2, "text_2");
      if(text_number >= 2) UpdateUniformTexture(1, "text_1");
      if(text_number >= 1) UpdateUniformTexture(0, "text_0");

      // variables
      UpdateUniform1f("time", time);
      for( int i = 0; i < var_number; i++){
        string var_name = GetVarName(i);
        UpdateUniform1f(var_name, vars[i]);
      }
    }

    void SetupDraw(){
      UpdateUniforms();
    }

    float time, time_def;
    float vars[10], vars_def[10];
    int var_number, text_number;
  };
  
  // ==================================================================== SLa
  enum sla {
    sla = 1,
    slaQD,
    slaTD,
    slaFB,
  };

  class SLa {
    /// SLa basic class
    public:
    std::shared_ptr<FrameBuffer> target, source;
    std::shared_ptr<SLaShader> shader;
    std::string name = "undefined";
    unsigned int kind = sla::sla;
    bool active = true;

    SLa( int a ) {}
    SLa(string n, std::shared_ptr<SLaShader> s) : name(n), shader(s) {}
    virtual void SetTarget( std::shared_ptr<FrameBuffer> target_ ) { target = target_; }
    virtual void SetSource( std::shared_ptr<FrameBuffer> source_ ) { source = source_; }
    virtual std::shared_ptr<FrameBuffer> GetSource( std::vector<string> & arguments ) { return source; }
    virtual bool LoadArguments( std::vector<string> & arguments ){ return true; }
    virtual void Tick(){ msg("SLa TICK");};
    virtual void Clean(){};
    virtual bool Init(){ return true; };

    bool on = true;
    bool clean = false;
    bool once  = false;
  };

  class SLaTD : public SLa {
    /// to blit single (or part of) Texture + Shader
    public:
    SLaTD(string n, std::shared_ptr<SLaShader> s, shared_ptr<Texture> t, DrawableQuadData dqd_) : SLa(n, s) { texture = t ; dqd = dqd_; }

    shared_ptr<Texture> texture;
    DrawableQuadData dqd;

    void Tick(){
      draw_texture_to_fb_or_screen( dqd, texture, target, shader );
    }
  };
  
  class SLaQD : public SLa {
    /// to blit array of quads with texture using QuadsDrawer + Shader
    public:
    SLaQD(string n, std::shared_ptr<SLaShader> s, std::shared_ptr<QuadsDrawer> drawer_ ) : SLa(n, s) { drawer = drawer_; }

    std::shared_ptr<QuadsDrawer> drawer;
  };
  
  class SLaFB : public SLa {
    /// to blit FrameBuffer + Shader
    public:
    SLaFB( string n, std::shared_ptr<SLaShader> s ) : SLa(n, s) {}

    void Tick(){
      draw_fb_to_fb_or_screen( source, target, shader );
    }
  };

  class SLaFBLoop : public SLa {
    /// to blit FB applying shader multiple times
    public:
    SLaFBLoop( string n, std::shared_ptr<SLaShader> s ) : SLa(n, s) {}
    std::shared_ptr<FrameBuffer> buffer;

    bool clean_buffer = true;
    int loops = 1;

    virtual void Tick(){
      if(loops <= 1){
        draw_fb_to_fb_or_screen( source, target, shader );
        return;
      }

      for(int counter = 1; counter < loops; counter++){
        if(counter % 2) draw_fb_to_fb_or_screen( source, buffer, shader );
        else            draw_fb_to_fb_or_screen( buffer, source, shader );
      }
      if(loops%2==0) swap(source, buffer);
      draw_fb_to_fb_or_screen( source, target, shader ); 
      if( clean_buffer ) buffer->Clean();
    }
  };

  
  // ==================================================================== sla chain
  struct SLaChainLink{
    std::string name;
    std::vector<string> arguments;
    SLaChainLink(string n, vector<string> & a){ name = n; arguments = a; }
  };
  
  class SLaChain : public BaseClass {
    public:
    SLaChain( string name_ ){
      name = name_;
    }

    std::vector< std::shared_ptr<SLa> > slas ;
    std::vector< std::pair<SLaChainLink, SLaChainLink > > links;
    
    vector< std::shared_ptr<SLaShader> > GetShaders(){
      set< std::shared_ptr<SLaShader> > shaders;
      for(int i = 0, i_max = slas.size(); i < i_max; ++i){
        std::shared_ptr<SLa> sla = slas[ i ];
        if(not sla->shader) continue;
        shaders.insert( sla->shader );
      }

      vector< std::shared_ptr<SLaShader> > answer;
      for(auto item : shaders) answer.push_back( item );
      return answer;
    }

    SLaChainLink ParceRelation(string item){
      /// parce "fname(arg=2, arg = 3)" into "fname", {"arg=2", "arg=3"}
      string sla_name;
      vector<string> sla_arguments;
      parce_string_function(item, sla_name, sla_arguments);
      MSG_DEBUG(__PFN__, item, "->", sla_name);

      /// remove spaces from arguments
      /*
      for(int i = 0; i < sla_arguments.size(); i++){
        string str = sla_arguments[i];
        std::string::iterator end_pos = std::remove(str.begin(), str.end(), ' ');
        str.erase(end_pos,str.end());
        sla_arguments[i] = str;
        MSG_DEBUG(__PFN__, "argument", i, "-", str);
      }
      */
      return SLaChainLink(sla_name, sla_arguments);
    }

    void AddRelation(string src, string tgt){
      MSG_DEBUG( __PFN__, src, "&", tgt );
      links.push_back( make_pair(ParceRelation(src), ParceRelation(tgt)) );
    }

    void AddRawRelation(string relation){
      /// expect input as "A->B->C,D->E,E->C"
      /// at first split into "A->B->C", "D->E", "E->C"
      std::vector<std::string> value_cpp_parts;
      pm::split_string_strip(relation, value_cpp_parts, ",");
      if( not value_cpp_parts.size() ){
        msg_err(__PFN__, "skip empty relation");
        return;
      } MSG_DEBUG(__PFN__, "find relation with N parts = ", value_cpp_parts.size());

      /// then split "A->B->C" split into "A", "B", "C"
      /// and add relations as AddRelation("A","B"), AddRelation("B","C")
      for(auto value_cpp_part : value_cpp_parts){
        std::vector<std::string> conn_parts;
        pm::split_string_strip(value_cpp_part, conn_parts, "->");
        if( conn_parts.size() < 2 ){
          msg_err(__PFN__, "skip relation without \"->\" separated parts");
          continue;
        } MSG_DEBUG(__PFN__, "find relation with N in connection = ", conn_parts.size());

        for(int i = 0; i < conn_parts.size()-1; ++i){
          string src = conn_parts[i];
          string tgt = conn_parts[i+1];
          AddRelation( src , tgt );
        }
      }
    }

    void AddRawRelations( vector<string> & relations ){
      for( auto rel : relations ) AddRawRelation( rel );
    }

    /*
    void Tick(){
      // sys::camera->LoadFBDefault();
      for(int i = 0, i_max = slas.size(); i < i_max; ++i){
        std::shared_ptr<SLa> sla = slas[ i ];
        if(not sla->on) continue;
        if(not sla->target) continue;
        if(sla->clean) sla->target->Clean();
        sla->Tick();
        if(sla->once) sla->on = false;
      }

      // sys::camera->LoadDefault();
      for(int i = 0, i_max = screen_slas.size(); i < i_max; ++i){
        std::shared_ptr<SLa> sla = screen_slas[ i ];
        if(not sla->on) continue;
        sla->TickScreen();
        if(sla->once) sla->on = false;
      }
    }
    */

    void Init( std::shared_ptr<DataContainer> dc ){
      slas.clear();

      if(dc == nullptr) {
        msg_err(__PFN__, "nullptr DataContainer, return");
        return;
      }

      for(int i = 0; i < links.size(); ++i){
        std::shared_ptr<SLa> src = dc->GetSLa( links[i].first.name );
        if(src == nullptr) {
          msg_err(__PFN__, "can't find", links[i].first.name, " in DataContainer", dc->name);
          continue;
        }
        if( not src->LoadArguments( links[i].first.arguments ) ){
          msg_err(__PFN__, "can't Init()", links[i].first.name, " in DataContainer", dc->name);
          continue;
        }

        if( links[i].second.name == "screen"){
          slas.push_back( src );
          continue;
        }

        std::shared_ptr<SLa> tgt = dc->GetSLa( links[i].second.name );
        if(tgt == nullptr) {
          msg_err(__PFN__, "can't find", links[i].second.name, " in DataContainer", dc->name);
          continue;
        }
        if( not tgt->LoadArguments( links[i].second.arguments ) ){
          msg_err(__PFN__, "can't Init()", links[i].second.name, " in DataContainer", dc->name);
          continue;
        }

        src->SetTarget( tgt->GetSource( links[i].second.arguments ) );
        slas.push_back( src );
      }
    }
  };

  // ==================================================================== sla other
  class SLaFactory : public BaseClass {
    /// define how we create objects from SLaConfigItem stored in SLaConfig
    public:
  };

}
#endif
