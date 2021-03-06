// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef  GerdaShader_H
#define GerdaShader_H 1

namespace ge { 

  class Shader : public BaseClass {
    public:
    Shader(){
      program_id  = 0;
      shader_vert = 0;
      shader_frag = 0;

      first_run = true;
      shader_name = "unnamed";
    }
    ~Shader(){ TurnOff(); }

    void TurnOff(){
      msg_err( "Shader TurnOff() not implemented yet" );
    }

    void LoadFromString(GLenum type, const string & text){
      GLuint id = glCreateShader( type );

      if(not id){
        MSG_WARNING("Shader.load_from_string(): wrong shader type, bad return glCreateShader(),", type, id);
        gl_check_error("Shader.load_from_string():");
        return;
      }

      const char * ctext = text.c_str();
      glShaderSource(id, 1, &ctext, NULL);

      GLint status;
      glCompileShader(id);
      glGetShaderiv(id, GL_COMPILE_STATUS, &status);

      if(status == GL_FALSE){
        GLint lenght;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
        GLchar * log = new GLchar[lenght];
        glGetShaderInfoLog(id, lenght, NULL, log);
        MSG_ERROR("Shader.load_from_string(): shader compile error log :", log);
        delete [] log;
      }
      else MSG_DEBUG("Shader.load_from_string(): shader compile ok", id);
    
      
      if( type == GL_VERTEX_SHADER)   { 
        if(shader_vert) MSG_WARNING("Shader.load_from_string(): override existed GL_VERTEX_SHADER shader", shader_name); 
        shader_vert = id; 
      }
      if( type == GL_FRAGMENT_SHADER) { 
        if(shader_frag) MSG_WARNING("Shader.load_from_string(): override existed GL_FRAGMENT_SHADER shader", shader_name); 
        shader_frag  = id; 
      }
    }

    void CreateProgram(){
      program_id = glCreateProgram();
      if( shader_vert ) glAttachShader(program_id, shader_vert);
      if( shader_frag ) glAttachShader(program_id, shader_frag);
      if(not shader_vert and not shader_frag) MSG_WARNING("Shader.CreateProgram():", shader_name, "do not any attached shaders"); 

      GLint status;
      glLinkProgram(program_id);
      glGetProgramiv(program_id, GL_LINK_STATUS, &status);
      if(status == GL_FALSE){
        GLint lenght;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &lenght);
        GLchar * log = new GLchar[lenght];
        glGetShaderInfoLog(program_id, lenght, NULL, log);
        MSG_ERROR("Shader.CreateProgram():", shader_name, " program link error log :", log);
        delete [] log;
      }
      else MSG_DEBUG("Shader.CreateProgram():", shader_name," shaders program link ok", program_id);
    
      glDeleteShader( shader_vert );
      glDeleteShader( shader_frag );

      int n_uniforms;
      glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &n_uniforms); // glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count);
      MSG_DEBUG("Shader.CreateProgram():", shader_name," shaders program has", program_id);

      for(GLuint i = 0; i < n_uniforms; i++){
        // glGetActiveAttrib(program, (GLuint)i, bufSize, &length, &size, &type, name);
        char name[100];
        GLint size;
        GLenum type = GL_ZERO;
        glGetActiveUniform(program_id, i, sizeof(name)-1, NULL, &size, &type, name);
        MSG_DEBUG("Shader.CreateProgram():", shader_name," shader use uniform", type, name);
        if( string(name).substr(0, 3) == "gl_" ) continue;
        AddUniform( name );
      }
    }

    void AddUniform(const GLchar * name){
      Bind();
      if(not program_id){ 
        MSG_WARNING("Shader.AddUniform():", shader_name, "don't have program", program_id);
        return;
      }
      GLint position = glGetUniformLocation( program_id, name );
      if(position == -1){
        MSG_WARNING("Shader.AddUniform(): glGetUniformLocation() return -1 for variable name \""+string(name)+"\" for shader", shader_name);
        return;
      }
      uniforms[string(name)] = position;
      MSG_DEBUG("Shader.AddUniform():", shader_name," add uniform() ", string(name), "to", position);
      Unbind();
    }

    GLint GetUniform(const char * name){
      first_run = false;
      map <string, GLint>::iterator it = uniforms.find(string(name));
      if(it == uniforms.end()){
        if(first_run) MSG_ERROR("Shader.GetUniform(): wrong uniform variable name \"",string(name),"\"");
        return 0;
      }
      return it->second;
    }

    void Bind(){   glUseProgram( program_id ); };
    void Unbind(){ glUseProgram( 0 ); };

    map <string, GLint> uniforms;

    string shader_name;
    GLuint shader_vert, shader_frag;
    GLuint program_id;
    bool first_run;
  };


  // special case of shader for framebuffer with up to 3 textures and up to 10 variables + 'time'
  class FrameShader : public Shader {
    public:
    FrameShader(const int & vn = 0, const int & tn = 0){
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
        msg( "//", shader_name );
        msg( "//", path_vert );
        msg( "//", path_frag );
        for(int i = 0; i < var_number; i++)
          msg("shader->vars[", i, "] =", vars[i], ";");
      } 
      if ( format.find("xml") != string::npos ) {
        msg_nll( "<shader name=\"" + shader_name + "\"" );
        msg_nll( "vert=\"" + path_vert + "\"" );
        msg_nll( "frag=\"" + path_frag + "\"" );
        msg_nll( ">\n" );
        for(int i = 0; i < var_number; i++)
          msg_nll( "<var value=\"" + to_string( vars[i] ) + "\"/>" );
        msg_nll( "\n</shader>\n" );
      }
      if ( format.find("vector") != string::npos ) {
        msg_nll( "vector<float> vars_" + shader_name + " = {" );
        for(int i = 0; i < var_number; i++)
          msg_nll( to_string( vars[i] ) + "," );
        msg_nll( "};\n" );
      }
    }

    void Load(string path_vert_, string path_frag_){
      path_vert = path_vert_;
      path_frag = path_frag_;

      string vert_txt = read_text_files( path_vert );
      string frag_txt = read_text_files( path_frag );

      ltrim(vert_txt);
      ltrim(frag_txt);
      rtrim(vert_txt);
      rtrim(frag_txt);

      MSG_DEBUG( frag_txt );
      LoadFromString(GL_VERTEX_SHADER,   vert_txt.c_str());
      LoadFromString(GL_FRAGMENT_SHADER, frag_txt.c_str());
      CreateProgram();
    }

    void Reload(){
      this->TurnOff();
      this->Load(path_vert, path_frag);
    }

    void ResetVars(){ for(int i = 0; i < 10; i++) vars[i] = vars_def[i]; }
    void ResetTime(){ time = time_def; }
    void SetDefVars(const vector<float> & vars_v){ for(int i = 0; i < vars_v.size(); i++) vars_def[i] = vars_v.at(i); }
    void SetDefTime(const float & t){ time_def = t; }
    void SetVars(const vector<float> & vars_v){ for(int i = 0; i < vars_v.size(); i++) vars[i] = vars_v.at(i); }
    void SetTime(const float & t){ time = t; }

    string GetVarName(const int & index){
      return "var_" + to_string(index) ;
    }

    void UpdateUniforms(){
      // texture
      if(text_number >= 3){
        glActiveTexture(GL_TEXTURE2);
        glUniform1i(GetUniform("text_2"), 2);
      }
      if(text_number >= 2){
        glActiveTexture(GL_TEXTURE1);
        glUniform1i(GetUniform("text_1"), 1);
      }
      if(text_number >= 1){
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(GetUniform("text_0"), 0);
      }

      // variables
      glUniform1f(GetUniform( "time" ), time);
      for( int i = 0; i < var_number; i++){
        string var_name = GetVarName(i);
        glUniform1f(GetUniform( var_name.c_str() ), vars[i]);
      }
    }

    float time, time_def;
    float vars[10], vars_def[10];
    int var_number, text_number;
    string path_vert, path_frag;
  };

  FrameShader * load_frame_shader(string name, string path_vert, string path_frag, int n_vars, int n_texts = 1){
    if(not path_vert.size()) path_vert = sys::default_shader_vert_path;
    if(not path_frag.size()) { path_frag = sys::default_shader_frag_path; n_vars=0; n_texts=1; }
    FrameShader* shader = new FrameShader(n_vars, n_texts);
    shader->Load(path_vert, path_frag);
    shader->shader_name = name;
    return shader;
  }

  //=============================================================================================================
  class ShaderInterpolation {
    public:
    /// simple class to update FrameShader vars based on Timer ftime variable 
    map<string, vector<VecInTime> > items;
    string algo = "pow1";

    void Add(string name, float time, vector<float> vals){
      auto points = items.find( name ); 
      if( points == items.end() ){
        items[ name ] = vector<VecInTime>();
        points = items.find( name ); 
      }

      points->second.push_back( VecInTime(time, vals) );
    }

    bool UpdateShader(FrameShader * shader, float time){
      auto points = items.find( shader->shader_name ); 
      if( points == items.end() ) return false;
      vector<float> values = interpolation_algo( points->second, time, algo );
      shader->SetVars( values );
      return true;
    }
  };

  //=============================================================================================================
  class StdinShader{
    /// class to update shaders variables from keyboard on the fly and dump shader states into console
    public:
    StdinShader(){
      shaders_iter = 0;
      dir = 1;
      shader_prev = nullptr;
      shader_print_mode = "xml,cpp,vector";
    }

    void ReloadShaders(){
      for(auto shader : shaders) shader->Reload();      
    }

    void PrintShaders(){
      for(auto shader : shaders) shader->Print( shader_print_mode );
    }
  
    void Tick(){
      if( shaders.size() == 0) return;
      if(sys::keyboard->Pressed(key::TAB) and sys::keyboard->Holded(key::LCTRL)) shaders_iter--;
      else if(sys::keyboard->Pressed(key::TAB)) shaders_iter++;
      if( shaders.size() <= shaders_iter ) shaders_iter = 0;
      if( shaders_iter < 0 ) shaders.size() - 1;
      FrameShader* shader = shaders.at( shaders_iter );
      if(not shader) msg(__PFN__, "NULL shader at index", shaders_iter);
      if(shader_prev != shader) msg(__PFN__, ": switch to ", shader->shader_name);
  
      if(sys::keyboard->Pressed(key::Q)) {dir *= -1; msg(dir);};
      float dvar = (0.1 + sys::keyboard->Holded(key::W) + 10*sys::keyboard->Holded(key::E)) * dir;
      if(sys::keyboard->Pressed(key::K_1)) { shader->vars[0] += dvar; msg(shader->vars[0]); }
      if(sys::keyboard->Pressed(key::K_2)) { shader->vars[1] += dvar; msg(shader->vars[1]); }
      if(sys::keyboard->Pressed(key::K_3)) { shader->vars[2] += dvar; msg(shader->vars[2]); }
      if(sys::keyboard->Pressed(key::K_4)) { shader->vars[3] += dvar; msg(shader->vars[3]); }
      if(sys::keyboard->Pressed(key::K_5)) { shader->vars[4] += dvar; msg(shader->vars[4]); }
      if(sys::keyboard->Pressed(key::K_6)) { shader->vars[5] += dvar; msg(shader->vars[5]); }
      if(sys::keyboard->Pressed(key::K_7)) { shader->vars[6] += dvar; msg(shader->vars[6]); }
      if(sys::keyboard->Pressed(key::K_8)) { shader->vars[7] += dvar; msg(shader->vars[7]); }
      if(sys::keyboard->Pressed(key::K_9)) { shader->vars[8] += dvar; msg(shader->vars[8]); }
      if(sys::keyboard->Pressed(key::K_0)) { shader->ResetVars(); }
      if(sys::keyboard->Pressed(key::SPACE)) PrintShaders();
      if(sys::keyboard->Pressed(key::R))     ReloadShaders();
      shader_prev = shader;
    }
    int shaders_iter, dir;
    string shader_print_mode;
    vector<FrameShader*> shaders;
    FrameShader* shader_prev;
  };

}

#endif 
