// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#ifndef GERDA_DRAW_HH 
#define GERDA_DRAW_HH 1

namespace ge {

  using namespace pm;
  using namespace std;
  using namespace tinyxml2;

  // ======= Basic drawing classes ====================================================================
  struct TexTile {
    /// TexTile store the info about position & size of part of image in Texture normalized to 1. space
    /// and original position & size of 
    TexTile() {}
    TexTile(v2 p, v2 s) : pos(p), size(s) {
		  tsize = v2(1,1);
      tpos = v2();      
    }
    TexTile(v2 p, v2 s, v2 asize) : pos(p), size(s) {
		  tsize = size / asize;
      tpos = pos / asize;
    }

    v2 pos, size, tpos, tsize;
  };

    class DrawableQuadData {
      /// store data we need to know to draw quad with texure
      public:
        DrawableQuadData(){}
        DrawableQuadData(v2 & pos_, v2 & size_, v2 & flip_, float & angle_, std::shared_ptr<TexTile> ttile_){
          Set(pos_, size_, flip_, angle_, ttile_);
        }

        void Set(v2 & pos_, v2 & size_, v2 & flip_, float & angle_, std::shared_ptr<TexTile> ttile_){
          pos = pos_;
          size = size_;
          flip = flip_;
          angle = angle_;
          ttile = ttile_;
        }

        float angle;
        v2 pos, size, flip;
        std::shared_ptr<TexTile> ttile;      
    };

  // ======= Texture interface ====================================================================
  class Texture : public BaseClass {
    /// Texture is wraped around Image and know how to draw it.
    /// 
    public:
    Texture():image(nullptr){}
    Texture(std::shared_ptr<Image> img){};
    
    virtual void Bind()  { }
    virtual void Unbind(){ }

    virtual void DrawFast(v2 pos, v2 size, std::shared_ptr<TexTile> ttile, float z_pos=0){}
    virtual void Draw(v2 pos=sys::WV2, v2 size=sys::WV2, v2 tpos=v2(0., 0.), v2 tsize=v2(1., 1.), float z_pos=0,  const float & angle=0, bool flip_x=false, bool flip_y=false){}
    
    void Draw(v2 pos, v2 size, std::shared_ptr<TexTile> ttile, float z_pos=0, const float & angle=0, bool flip_x=false, bool flip_y=false){
      Draw(pos, size, ttile->tpos, ttile->tsize, z_pos, angle, flip_x, flip_y);
    }

    void Draw( DrawableQuadData & dqd ){
      Draw( dqd.pos, dqd.size, dqd.ttile, dqd.pos.z, dqd.angle, dqd.flip.x, dqd.flip.y );
    }

    TexTile * GetTexTile(const string & key){ // TODO maybe Texture Atlas should be separated from Texture ...
      auto ptr = atlas.find(key);
      if(ptr == atlas.end()) return nullptr;
      return &(ptr->second);
    }

    void AddTexTile(const string & key, const v2 & p, const v2 & s){ 
      if(GetTexTile(key)) return;
      atlas[ key ] = TexTile(p, s, size);
    }

    string GenerateAtlasGridName(int x, int y) const { // TODO set interface for this
      return "X" + to_string(x) + "Y" + to_string(y);
    };
    
    string GenerateAtlasName(string name, int x, int y) const {
      return name + "_" + to_string(x) + "_" + to_string(y);
    };

    map<string, TexTile> atlas;
    v2 size;
    double w,h;
    std::shared_ptr<Image> image;
  };
  
  // ======= draw parts of texute ===================================================================
  class QuadsDrawer : public BaseClass {
    protected:
    std::shared_ptr<Texture> texture;
    public:
    void SetTexture(std::shared_ptr<Texture> text){
      texture = text;
    }
    virtual void Draw(){}
    virtual void Clean(){}
    virtual void Remove( unsigned int id ){}
    virtual unsigned int AddDummy(){}
    virtual unsigned int Add(v2 & pos, v2 & size, float & angle, v2 & flip, std::shared_ptr<TexTile> ttile){};
    virtual void Change( unsigned int id, v2 & pos, v2 & size, float & angle, v2 & flip, std::shared_ptr<TexTile> ttile ){};
    virtual void ChangeSafe( unsigned int id, v2 & pos, v2 & size, float & angle, v2 & flip, std::shared_ptr<TexTile> ttile ){};
  };

  // ======= draw parts of texute ===================================================================
  class TextureDrawer : public QuadsDrawer { 
    public:
      std::map<unsigned int, DrawableQuadData> drawables;
      unsigned int id_counter;
      
      TextureDrawer() {}

      unsigned int AddDummy(){
        drawables[ ++id_counter ] = DrawableQuadData();
        return id_counter-1;
      }
  
      unsigned int Add(v2 & pos, v2 & size, float & angle, v2 & flip, std::shared_ptr<TexTile> ttile){
        drawables[ ++id_counter ] = DrawableQuadData(pos, size, flip, angle, ttile);
        return id_counter-1;
      }

      void Change( unsigned int id, v2 & pos, v2 & size, float & angle, v2 & flip, std::shared_ptr<TexTile> ttile ){
        auto iter = drawables.find( id );
        iter->second.Set(pos, size, flip, angle, ttile);
      }

      void ChangeSafe( unsigned int id, v2 & pos, v2 & size, float & angle, v2 & flip, std::shared_ptr<TexTile> ttile ){
        auto iter = drawables.find( id );
        if( iter == drawables.end() ){
          MSG_WARNING(__PFN__, "can't find id", id);
          return;
        }
        iter->second.Set(pos, size, flip, angle, ttile);
      }

      virtual void Clean(){
        drawables.clear();
      }

      void Remove( unsigned int id ){
        bool result = drawables.erase( id );
        if( result ) return;
        MSG_WARNING(__PFN__, "can't find id", id);
      }
  };

  // ======= draw array of quads ====================================================================
  namespace sys{
    const unsigned int   MAX_QUAD_NUMBER      = 100000;
    const unsigned int   MAX_QUAD_NUMBER10    = MAX_QUAD_NUMBER/10;
    const unsigned int   QUAD_VERTEXES_NUMBER = 4;
    const unsigned int   VERTEX_ARRAY_SIZE    = 5;
    const unsigned int   QUAD_ARRAY_SIZE      = QUAD_VERTEXES_NUMBER * VERTEX_ARRAY_SIZE;
    const unsigned int   VERTEX_BITE_SIZE     = VERTEX_ARRAY_SIZE*sizeof(GL_FLOAT);
    const float PERSPECTIVE_EDGE     = -1000000.;
    const float HIDE_EDGE            = PERSPECTIVE_EDGE/2;
  };

  class ArrayQuadsDrawer : public QuadsDrawer { 
    public :
      ArrayQuadsDrawer(unsigned int max_size = sys::MAX_QUAD_NUMBER) {
        max_quads_number = min(max_size, sys::MAX_QUAD_NUMBER);
        Clean();
      }

      unsigned int FindFreePosition(){
        if( free_positions.size() ) {
          unsigned int answer = free_positions.top();
          free_positions.pop();
          return answer;
        }
        for(unsigned int i = quads_number; i < max_quads_number; i++){
          if(data[i*sys::QUAD_ARRAY_SIZE+2] <= sys::PERSPECTIVE_EDGE){ quads_number = min(i+1, sys::MAX_QUAD_NUMBER); return i; }
        }
        for(unsigned int i = 0; i < quads_number; i++){
          if(data[i*sys::QUAD_ARRAY_SIZE+2] <= sys::PERSPECTIVE_EDGE){ quads_number = min(i+1, sys::MAX_QUAD_NUMBER); return i; }
        }
        MSG_WARNING("ArrayQuadsDrawer.FindFreePosition(): warning, cant find free position");
        return (quads_number < max_quads_number ? quads_number : 0);
      }

      unsigned int AddDummy(){
        /// reserve ID without updating of the vertex array data
        unsigned int id = FindFreePosition() * sys::QUAD_ARRAY_SIZE;
        return id;
      }

      unsigned int Add(v2 pos, v2 size, v2 tpos, v2 tsize){
        int id = FindFreePosition() * sys::QUAD_ARRAY_SIZE;
        Change(id, pos, size, tpos, tsize, 0, false);
        return id;
      }

      void Remove(const int & id){
        data[id+2]  = sys::PERSPECTIVE_EDGE;
        data[id+7]  = sys::PERSPECTIVE_EDGE;
        data[id+12] = sys::PERSPECTIVE_EDGE;
        data[id+17] = sys::PERSPECTIVE_EDGE;
        if( free_positions.size() < sys::MAX_QUAD_NUMBER10 ) free_positions.push( id );
      }

      virtual void Clean(){
        for(int id = 0, id_max = max_quads_number*sys::QUAD_ARRAY_SIZE; id < id_max; id+=sys::QUAD_ARRAY_SIZE){
          data[id+2]  = sys::PERSPECTIVE_EDGE;
          data[id+7]  = sys::PERSPECTIVE_EDGE;
          data[id+12] = sys::PERSPECTIVE_EDGE;
          data[id+17] = sys::PERSPECTIVE_EDGE;
        }
      }

      void Move(const int & id, const v2 & shift){
        for(int i = id; i < id+sys::QUAD_ARRAY_SIZE; i+=sys::VERTEX_ARRAY_SIZE){
          data[i+0] += shift.x;
		      data[i+1] += shift.y;
        }
      }

      void ChangeFast(const int & id, const v2 & pos, const v2 & size){
        data[id+0]  = pos.x - size.x;
		    data[id+1]  = pos.y + size.y;

		    data[id+5]  = pos.x + size.x;
		    data[id+6]  = pos.y + size.y;

		    data[id+10] = pos.x + size.x;
		    data[id+11] = pos.y - size.y;

		    data[id+15] = pos.x - size.x;
		    data[id+16] = pos.y - size.y;
      }

      void Change(const int & id, const v2 & pos, v2 size, const v2 & tpos, const v2 & tsize, const int & angle, const v2 & flip){
        if(flip.x) size.x *= -1;
        if(flip.y) size.y *= -1;
        
        v2 perp = v2(-size.x, size.y);
        if(angle){
          size = size.Rotated(angle);
          perp = perp.Rotated(angle);
          // perp = v2(-size.x, size.y);
        }

        data[id+0]  = pos.x - size.x; data[id+3]  = tpos.x;
		    data[id+1]  = pos.y + size.y; data[id+4]  = tpos.y;
        data[id+2]  = pos.z;

		    data[id+5]  = pos.x - perp.x; data[id+8]  = tpos.x + tsize.x;
		    data[id+6]  = pos.y + perp.y; data[id+9]  = tpos.y;
        data[id+7]  = pos.z;

		    data[id+10] = pos.x + size.x; data[id+13] = tpos.x + tsize.x;
		    data[id+11] = pos.y - size.y; data[id+14] = tpos.y + tsize.y;
        data[id+12] = pos.z;

		    data[id+15] = pos.x + perp.x; data[id+18] = tpos.x;
		    data[id+16] = pos.y - perp.y; data[id+19] = tpos.y + tsize.y;
        data[id+17] = pos.z;
      }

      void ChangeSafe(const int & id, const v2 & pos, const v2 & size, const v2 & tpos, const v2 & tsize, const int & angle, const v2 & flip){
        if(id < 0 or id >= sys::MAX_QUAD_NUMBER) { MSG_WARNING(__PFN__, "can't update data, incorrect index/id", id); return; }
        Change(id, pos, size, tpos, tsize, angle, flip);
      }

      void Print(){
        msg(__PFN__);
        for(int i = 0; i < max_quads_number; i++){
          if(data[i*sys::QUAD_ARRAY_SIZE+2] > sys::PERSPECTIVE_EDGE){
            int id = i*sys::QUAD_ARRAY_SIZE;
            msg( i ); 
            for(int v = 0; v < 20; v+=5)
              msg( "[(",data[id+0+v],data[id+1+v],data[id+2+v], "),(", data[id+3+v], data[id+4+v], ")]");
          }
        }
      }

      int max_quads_number, quads_number = 0;
      float data[sys::MAX_QUAD_NUMBER*sys::QUAD_ARRAY_SIZE];
      std::stack<int> free_positions;
  };

  // ==================================================================== Draw realm primitives
    class DQImp : public BaseClass {
      /// Base class to draw quads. We provide info to the drawer, later drawer draw all at once.
      public:
      virtual void UpdateDrawer(unsigned  int draw_id, v2 & pos, v2 & size, v2 & flip, float & angle, std::shared_ptr<TexTile> ttile ){}
      virtual void UpdateDrawerSafe( unsigned int draw_id, v2 & pos, v2 & size, v2 & flip, float & angle, std::shared_ptr<TexTile> ttile ){}
      virtual void UpdateDrawerFast( unsigned int draw_id, v2 & pos, v2 & size, v2 & flip, float & angle, std::shared_ptr<TexTile> ttile ){}
      virtual unsigned int  PrepareDrawer( ){ return 0; }
    };

    class TextureDQImp : public DQImp {
      std::shared_ptr<Texture> drawer; 
      TextureDQImp( std::shared_ptr<Texture> drawer_ ){
        drawer = drawer_;
      }

      /// TextureDQImp's drawer is Texture, draw immidiently. We bind texture every Draw call (expensive).
      virtual void UpdateDrawer( v2 & draw_id, v2 & pos, v2 & size, v2 & flip, float & angle, std::shared_ptr<TexTile> ttile ){
        // void Draw(v2 pos=sys::WV2, v2 size=sys::WV2, v2 tpos=v2(0., 0.), v2 tsize=v2(1., 1.), float z_pos=0, bool flip_x=false, bool flip_y=false, const float & angle=0)
        drawer->Bind();
        drawer->Draw(pos, size, ttile, pos.z, angle, flip.x, flip.y);
        drawer->Unbind();
      }

      virtual void UpdateDrawerSafe( unsigned int draw_id, v2 & pos, v2 & size, v2 & flip, float & angle, std::shared_ptr<TexTile> ttile ){
        drawer->Bind();
        drawer->Draw(pos, size, ttile, pos.z, angle, flip.x, flip.y);
        drawer->Unbind();
      }

      virtual void UpdateDrawerFast( unsigned int draw_id, v2 & pos, v2 & size, v2 & flip, float & angle, std::shared_ptr<TexTile> ttile ){
        drawer->Bind();
        drawer->DrawFast(pos, size, ttile);
        drawer->Unbind();
      }

      virtual void Hide( ){}
    };

    class AQDDQImp : public DQImp {
      /// Drawer is ArrayQuadsDrawer. Every Draw call we only fill information inside AQD.
      /// To actual drawing we need to call AQD drawing method later, bliting all at once.
      std::shared_ptr<ArrayQuadsDrawer> drawer; 
      AQDDQImp( std::shared_ptr<ArrayQuadsDrawer> drawer_ ){
        drawer = drawer_;
      }

      virtual unsigned int PrepareDrawer( ){
        /// reserve id in AQD array
        return drawer->AddDummy();
      }

      virtual void UpdateDrawer( unsigned int draw_id, v2 & pos, v2 & size, v2 & flip, float & angle, std::shared_ptr<TexTile> ttile ){
        /// unsafe update of the pos+size data in the drawer vertex array 
        drawer->Change(draw_id, pos, size, ttile->tpos, ttile->tsize, angle, flip);
      }
      virtual void UpdateDrawerFast( unsigned int draw_id, v2 & pos, v2 & size, v2 & flip, float & angle, std::shared_ptr<TexTile> ttile ){
        /// unsafe update of the pos+size data in the drawer vertex array ignoring changes in the size of quad and texture vertexes
        drawer->ChangeFast(draw_id, pos, size);
      }
      virtual void UpdateDrawerSafe( unsigned int draw_id, v2 & pos, v2 & size, v2 & flip, float & angle, std::shared_ptr<TexTile> ttile ){
        /// safe update of the pos+size data in the drawer vertex array
        if(not drawer) {MSG_WARNING(__PFN__, "no drawer, cant send changes"); return; }
        if(not ttile)  {MSG_WARNING(__PFN__, "no texture tile, cant send changes"); return; }
        drawer->ChangeSafe(draw_id, pos, size, ttile->tpos, ttile->tsize, angle, flip);
      }
    };

    class TDDQImp : public DQImp {
      /// Drawer is ArrayQuadsDrawer. Every Draw call we only fill information inside AQD.
      /// To actual drawing we need to call AQD drawing method later, bliting all at once.
      std::shared_ptr<TextureDrawer> drawer; 
      TDDQImp( std::shared_ptr<TextureDrawer> drawer_ ){
        drawer = drawer_;
      }

      virtual unsigned int PrepareDrawer(){
        /// reserve id in AQD array
        return drawer->AddDummy();
      }

      virtual void UpdateDrawer( unsigned int draw_id, v2 & pos, v2 & size, v2 & flip, float & angle, std::shared_ptr<TexTile> ttile ){
        /// unsafe update of the pos+size data in the drawer vertex array 
        drawer->Change(draw_id, pos, size, angle, flip, ttile);
      }
      virtual void UpdateDrawerFast( unsigned int draw_id, v2 & pos, v2 & size, v2 & flip, float & angle, std::shared_ptr<TexTile> ttile ){
        /// unsafe update of the pos+size data in the drawer vertex array ignoring changes in the size of quad and texture vertexes
        drawer->Change(draw_id, pos, size, angle, flip, ttile);
      }
      virtual void UpdateDrawerSafe( unsigned int draw_id, v2 & pos, v2 & size, v2 & flip, float & angle, std::shared_ptr<TexTile> ttile ){
        /// safe update of the pos+size data in the drawer vertex array
        if(not drawer) {MSG_WARNING(__PFN__, "no drawer, cant send changes"); return; }
        if(not ttile)  {MSG_WARNING(__PFN__, "no texture tile, cant send changes"); return; }
        drawer->ChangeSafe(draw_id, pos, size, angle, flip, ttile);
      }
    };

    class DrawableQuad : public DrawableQuadData, public BaseClass {
      /// Quad is defined by pos (x,y,z), size (w,h), angle. We also have boolean to flip/mirror it over x, y axis.
      /// Finally, what we actually draw is Texture (OpenGL) defined by TexTile object (relative coordinates in texture).
      /// Drawer - class where we actually draw this.
      /// Drawer Implementation - class where we define how we draw this.
      public:
        bool updated = true;
        unsigned int draw_id;
        std::shared_ptr<DQImp> drawer_imp;

        void Print(){ msg(__PFN__, draw_id, pos, size); }
        void PrintAll(){ msg(__PFN__, draw_id, pos, size, angle, flip, ttile); }

        void SetDrawerImp(std::shared_ptr<DQImp> drawer_imp_){
          /// Define what kind of drawer implementation will be used
          drawer_imp = drawer_imp_;
          draw_id = drawer_imp->PrepareDrawer();
        }

        void Hide(){
          pos.z = sys::HIDE_EDGE;
        }

        void UpdateDrawer(){
          /// default version of drawing
          drawer_imp->UpdateDrawer( draw_id, pos, size, flip, angle, ttile );
        }

        void UpdateDrawerFast(){
          /// version of drawing for high performance
          drawer_imp->UpdateDrawerFast( draw_id, pos, size, flip, angle, ttile );
        }

        void UpdateDrawerSafe(){
          /// version of drawing with extra safety checks
          if(not drawer_imp){
            MSG_WARNING(__PFN__, "no draw implementation, can't draw, return. Next line is my info:");
            PrintAll();
            return;
          }
          drawer_imp->UpdateDrawerSafe( draw_id, pos, size, flip, angle, ttile );
        }
    };

  // ==================================================================== Shader
  class ShaderImp {
    public:
    //! load vertex shader from string
    virtual void LoadFromStringVert( const string & text) = 0;
    //! load fragment shader  from string
    virtual void LoadFromStringFrag( const string & text) = 0;
    //! create shader program
    virtual void CreateProgram() = 0;

    //! add unifrom with name to shader
    virtual void AddUniform(const string name) = 0;
    //! return uniform id
    virtual int  GetUniform(const string name) = 0;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    //! update float unifrom value
    virtual void UpdateUniform1f(const string & name, const float & val) = 0;

    //! tell that some binded texture will be used under the 'name'
    virtual void UpdateUniformTexture(int index, const string & name) = 0;

    string shader_name;
  };

  class Shader : public BaseClass {
    public:
    std::shared_ptr<ShaderImp> shader_imp;

    void Bind() { shader_imp->Bind(); } 
    void Unbind() { shader_imp->Bind(); } 

    void LoadFromStringVert( const string text){ shader_imp->LoadFromStringVert( text ); } 
    void LoadFromStringFrag( const string text){ shader_imp->LoadFromStringFrag(  text ); } 

    void CreateProgram(){ shader_imp->CreateProgram(); };

    void UpdateUniform1f(const string id, const float val){ shader_imp->UpdateUniform1f(id, val); } 
    void UpdateUniformTexture(int index, const string id){ shader_imp->UpdateUniformTexture(index, id); } 

    ~Shader(){}

    virtual void Load(string path_vert_, string path_frag_){
      path_vert = path_vert_;
      path_frag = path_frag_;

      MSG_INFO( __PFN__, sys::file_input, path_vert, path_frag );
      string vert_txt = sys::file_input->read_text_files( path_vert );
      string frag_txt = sys::file_input->read_text_files( path_frag );

      MSG_INFO( vert_txt );
      MSG_INFO( frag_txt );

      ltrim(vert_txt);
      ltrim(frag_txt);
      rtrim(vert_txt);
      rtrim(frag_txt);

      LoadFromStringVert( vert_txt );
      LoadFromStringFrag( frag_txt );
      CreateProgram();
    }

    string path_vert, path_frag;
    bool first_run = true;
  };
  
  // ==================================================================== Realms  
  class Realm { };

  class DrawRealm : public Realm {
    public:
    DrawRealm(){}

    void Tick(){
      /// Execute chain
    }
  };

/*
  At game step:
    iterate over game objects and update drawable objects variables

  At draw step:
    pass drawable objects variable to drawer and reserve for drawing

    in defined order following may happend
      - drawer draw (Texture, other) 
      - 
*/
}
#endif







