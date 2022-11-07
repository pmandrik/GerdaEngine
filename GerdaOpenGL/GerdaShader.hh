// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GerdaShader_H
#define GerdaShader_H 1

namespace ge {

  using namespace pm;
  using namespace std;
  using namespace tinyxml2;

  class ShaderGLImp : public ShaderImp {
    public:
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

    virtual void LoadFromStringVert( const string & text){ LoadFromString( GL_VERTEX_SHADER, text ); };
    virtual void LoadFromStringFrag( const string & text){ LoadFromString( GL_FRAGMENT_SHADER, text ); };

    virtual void CreateProgram(){
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

    virtual int GetUniform(const string & name){
      first_run = false;
      map <string, int>::iterator it = uniforms.find(string(name));
      if(it == uniforms.end()){
        if(first_run) MSG_ERROR("Shader.GetUniform(): wrong uniform variable name \"", string(name), "\"");
        return 0;
      }
      return it->second;
    }

    virtual void UpdateUniformTexture(int index, const string & name){
        GLint id = GetUniform(name);
        if( index == 0 ){
          glActiveTexture(GL_TEXTURE0);
          glUniform1i(id, 0);
        } else  if( index == 1 ){
          glActiveTexture(GL_TEXTURE1);
          glUniform1i(id, 1);
        } else if( index == 2 ){
          glActiveTexture(GL_TEXTURE2);
          glUniform1i(id, 2);
        }
    }

    virtual void UpdateUniform1f(const string & name, const float & val){
      glUniform1f( (GLint) GetUniform( name.c_str() ), val );
    }

    virtual void AddUniform(const string name){
      const GLchar * name_cstr = (const GLchar *) name.c_str();
      Bind();
      if(not program_id){ 
        MSG_WARNING("Shader.AddUniform():", shader_name, "don't have program", program_id);
        return;
      }
      GLint position = glGetUniformLocation( program_id, name_cstr );
      if(position == -1){
        MSG_WARNING("Shader.AddUniform(): glGetUniformLocation() return -1 for variable name \""+string(name)+"\" for shader", shader_name);
        return;
      }
      uniforms[ name ] = position;
      MSG_DEBUG("Shader.AddUniform():", shader_name," add uniform() ", string(name), "to", position);
      Unbind();
    }

    virtual void Bind(){   glUseProgram( program_id ); };
    virtual void Unbind(){ glUseProgram( 0 ); };

    map <string, GLint> uniforms;

    GLuint shader_vert, shader_frag;
    GLuint program_id;
    bool first_run;
  };

/*
  FrameShader * load_frame_shader(string name, string path_vert, string path_frag, int n_vars, int n_texts = 1){
    if(not path_vert.size()) path_vert = sys::default_shader_vert_path;
    if(not path_frag.size()) { path_frag = sys::default_shader_frag_path; n_vars=0; n_texts=1; }
    FrameShader* shader = new FrameShader(n_vars, n_texts);
    shader->Load(path_vert, path_frag);
    shader->shader_name = name;
    return shader;
  }
*/

  //=============================================================================================================
  /*
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
  };*/

  //=============================================================================================================
/*
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
*/

}//namespace
#endif 
