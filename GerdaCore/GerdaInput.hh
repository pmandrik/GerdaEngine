// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GERDA_INPUT_HH 
#define GERDA_INPUT_HH 1

namespace ge {
  // ======= Functions for load XML Tiled Map ====================================================================
  static const long int MAGIC_TILED_NUMBER = 1073741824;

  void check_tinyxml_error(XMLError error, string start_msg = string("")){
    if(not error) return;
    switch(error){
      case XMLError::XML_ERROR_FILE_NOT_FOUND : 
        msg_err( start_msg, " : XMLError::XML_ERROR_FILE_NOT_FOUND");
        break;
      case XMLError::XML_ERROR_FILE_COULD_NOT_BE_OPENED : 
        msg_err( start_msg, " : XMLError::XML_ERROR_FILE_COULD_NOT_BE_OPENED");
        break;
      case XMLError::XML_ERROR_FILE_READ_ERROR :
        msg_err( start_msg, " : XMLError::XML_ERROR_FILE_READ_ERROR");
        break;
      default :
        msg_err( start_msg, " : code ", error);
    }
  }

  struct TiledMapObject {
    TiledMapObject(v2 p, v2 s, v2 f, string n, float a, int l, bool t = false, string gname = "") : pos(p), size(s), flip(f), name(n), angle(a), level(l), is_tile(t), group_name(gname) {};

    v2 pos, size, flip;
    string name, group_name;
    float angle;
    int level;

    bool is_tile; // can be sprite/image or tile
  };

  // FIXME unsafe, all code around is unsafe :c
  vector <TiledMapObject* > * load_xml_tiled_map(string filename, int verbose_lvl = 0){
    XMLDocument doc;
    XMLError error = doc.LoadFile( filename.c_str() );
    if( error ){
      check_tinyxml_error(error, "load_xml_map() error : " + filename);
      return nullptr;
    }

    XMLElement* map_node = doc.FirstChildElement( "map" );

    if(not map_node){
      msg_err( "load_xml_map() error : ", filename, " has not 'map' node");
      return nullptr;
    }

    map<long long int, string> id_to_key;

    /*
        What we get from Tiled map:
          I. "tileset" information:
            1. Tiled "id" + "firstgid"  for internal use
            2. Object image name/path -> ${TilesetName}"_"${X_Pos}"_"${Y_Pos}".png"
          II. objects information: position, scale, rotation, flip etc
    */
    XMLElement* tileset = map_node->FirstChildElement("tileset");
    while(tileset){
      int firstgid = tileset->IntAttribute( "firstgid" );
      XMLElement* tile = tileset->FirstChildElement("tile");
      while(tile){
        int id = tile->IntAttribute( "id" ) + firstgid;
        string source = string( tile->FirstChildElement("image")->Attribute( "source" ) );
        string width  = string( tile->FirstChildElement("image")->Attribute( "width"  ) );
        string height = string( tile->FirstChildElement("image")->Attribute( "height" ) );

        int start = source.find_last_of(string("/"));
        int end = source.find_last_of(string("."));
        source = source.substr(start+1, end-start-1) + "_W" + width + "_H" + height;

        id_to_key[id] = source;
        MSG_DEBUG( source );

        tile = tile->NextSiblingElement("tile");
      }

      tileset = tileset->NextSiblingElement("tileset");
    }

    vector<TiledMapObject* > * map_objects = new vector<TiledMapObject* >();
    XMLElement* objectgroup = map_node->FirstChildElement("objectgroup");
    int level = 0;
    while(objectgroup){
      XMLElement* object = objectgroup->FirstChildElement("object");
      string group_name  = objectgroup->Attribute( "name" );
      while(object){
        v2 pos = v2(object->FloatAttribute( "x" ), object->FloatAttribute( "y" ));
        v2 size = v2(object->FloatAttribute( "width" ), object->FloatAttribute( "height" ));
        float angle = object->FloatAttribute( "rotation" );
        const char * gid_str = object->Attribute( "gid" );// MAGIC_TILED_NUMBER
        v2 flip;
        string name;
        if( gid_str ){
          long long int gid = atoll( gid_str );
          flip = v2(gid / MAGIC_TILED_NUMBER / 2, gid / MAGIC_TILED_NUMBER % 2);
          gid %= MAGIC_TILED_NUMBER;
          name = id_to_key[ gid ];
        }
        map_objects->push_back( new TiledMapObject(pos, size, flip, name, angle, level, false, group_name) );
        object = object->NextSiblingElement("object");
      }
      objectgroup = objectgroup->NextSiblingElement("objectgroup");
      level++;
    }

    /*
      now we want to load map tiles
    */
    msg("load_xml_tiled_map(): load map tiles");
    int map_width  = atoi( map_node->Attribute( "width" ) );
    int map_height = atoi( map_node->Attribute( "height" ) );
    int tilewidth  = atoi( map_node->Attribute( "tilewidth" ) );
    int tileheight = atoi( map_node->Attribute( "tileheight" ) );

    // load tilesets
    map<long long int, string> tileid_to_key;

    tileset = map_node->FirstChildElement("tileset");
    while(tileset){
      XMLElement* image = tileset->FirstChildElement("image");
      if(not image) { 
        tileset = tileset->NextSiblingElement("tileset");
        continue;
      }
      string name = tileset->Attribute("name");
      int gid_first = atoi( tileset->Attribute("firstgid") );

      int x_tiles = (atoi(image->Attribute("width"))  - atoi(tileset->Attribute("margin"))) / (tilewidth  + atoi(tileset->Attribute("spacing")));
      int y_tiles = (atoi(image->Attribute("height")) - atoi(tileset->Attribute("margin"))) / (tileheight + atoi(tileset->Attribute("spacing")));
      for(int y = 0; y <= y_tiles; y++)
        for(int x = 0; x <= x_tiles; x++)
          tileid_to_key[ gid_first + x + x_tiles * y ] = name + "_X" + to_string(x) + "_Y" + to_string(y);

      tileset = tileset->NextSiblingElement("tileset");
    }

    // load and bind tiles
    level = 0;
    XMLElement* layer = map_node->FirstChildElement("layer");
    msg(map_width, map_height);
    while(layer){
      XMLElement* data = layer->FirstChildElement("data");
      string group_name = layer->Attribute( "name" );
      while(data){
        int x = 0, y = 0;

        XMLElement* tile = data->FirstChildElement("tile");
        while(tile){
          long long int tile_id = atoll( tile->Attribute( "gid" ) );
          if(tile_id){
            string name = tileid_to_key[ tile_id ];
            v2 pos = v2(x * tilewidth, map_height * tileheight - y * tileheight);
            if(not name.size()) msg("!!!", tile_id, v2(x, y), tile->Attribute( "gid" ));
            map_objects->push_back( new TiledMapObject(pos, v2(tilewidth, tileheight), false, name, 0, level, true, group_name) );
          }
          if( (++x) >= map_width ) { x = 0; y ++; }
          tile = tile->NextSiblingElement("tile");
        }
        data = data->NextSiblingElement("data");
      }
      layer = layer->NextSiblingElement("layer");
      level++;
    }

    msg("load_xml_tiled_map() ... done");
    return map_objects;
  }

  // ======= Functions for load and writing images ====================================================================
  // checker image, 4x4, GL_RED, GL_UNSIGNED_BYTE data.
  static float gl_test_image_data[] = { 1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 1.0f,
                                          0.0f, 0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f, 1.0f,
                                          1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 1.0f,
                                          0.0f, 0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f, 1.0f  };

  class Image : public BaseClass {
    public:
      Image(int width=0,int height=0,int comp_=4){ // comp=4 for RGBA
        w = width;
        h = height;
        comp = comp_;
        data = nullptr;
      }

      void LoadCheck(){
        w = 4;
        h = 4;
        type   = GL_FLOAT;
        format = GL_RGBA;
        data = (void*)gl_test_image_data;
      }

      void LoadPNG(const string & file_name, int type_ = GL_UNSIGNED_BYTE, int format_ = GL_RGBA){
        MSG_DEBUG("Image.LoadPNG(): try to load file ", file_name);
        data   = stbi_load(file_name.c_str(), &w, &h, &comp, 0);
        type   = type_;
        format = format_;
        MSG_DEBUG("Image.LoadPNG(): ", file_name, "(",w,"x",h,")", comp, "ok");

        for(int y = 0; y < h; y++){
          for(int x = 0; x < w; x++){
            int r = (int)((unsigned char*)data)[y*comp*w + x*comp + 0];
            int g = (int)((unsigned char*)data)[y*comp*w + x*comp + 1];
            int b = (int)((unsigned char*)data)[y*comp*w + x*comp + 2];
            if(r == 255 and g == 0 and b == 0) ((unsigned char*)data)[y*comp*w + x*comp + 3] = 0;
            if(r == 0 and g == 255 and b == 0) ((unsigned char*)data)[y*comp*w + x*comp + 3] = 0;
            // if(r == 255 and g == 0 and b == 255) ((unsigned char*)data)[y*comp*w + x*comp + 3] = 0.25;
          }
        }
        name = file_name;
      }

      void WritePNG(const string & file_name){
        MSG_DEBUG("Image.WritePNG(): ", file_name);
        // int stbi_write_png(char const *filename, int w, int h, int comp, const void *data, int stride_in_bytes);
        stbi_write_png(file_name.c_str(), w, h, comp, data, 0);
        name = file_name;
      }

      // super slow Mirror & Flip for RGBA
      void Mirror(){
        for(int y = 0; y < h; y++){
          for(int x = 0, max = w/2; x < max; x++){
            swap( ((unsigned char*)data)[y*comp*w + x*comp + 0], ((unsigned char*)data)[y*comp*w + (w-x)*comp + 0] );
            swap( ((unsigned char*)data)[y*comp*w + x*comp + 1], ((unsigned char*)data)[y*comp*w + (w-x)*comp + 1] );
            swap( ((unsigned char*)data)[y*comp*w + x*comp + 2], ((unsigned char*)data)[y*comp*w + (w-x)*comp + 2] );
          }
        }
      }

      void Flip(){
        for(int x = 0; x < w; x++){
          for(int y = 0, max = h/2; y < max; y++){
            swap( ((unsigned char*)data)[y*comp*w + x*comp + 0], ((unsigned char*)data)[(h-y)*comp*w + x*comp + 0] );
            swap( ((unsigned char*)data)[y*comp*w + x*comp + 1], ((unsigned char*)data)[(h-y)*comp*w + x*comp + 1] );
            swap( ((unsigned char*)data)[y*comp*w + x*comp + 2], ((unsigned char*)data)[(h-y)*comp*w + x*comp + 2] );
          }
        }
      }

      int w,h,comp,type,format;
      void *data;
      string name;
  };

  // ======= Functions for reading text files ====================================================================
  string read_text_files(const string & path){
    SDL_RWops *file = SDL_RWFromFile(path.c_str(), "r");
    if( file == NULL ) {
      msg_err("read_text_files(): cant open file", path, "return empty string");
      return "";
    }

    Sint64 res_size = SDL_RWsize(file);
    char* res = (char*)malloc(res_size + 1);

    Sint64 nb_read_total = 0, nb_read = 1;
    char* buf = res;
    while (nb_read_total < res_size && nb_read != 0) {
      nb_read = SDL_RWread(file, buf, 1, (res_size - nb_read_total));
      nb_read_total += nb_read;
      buf += nb_read;
    }
    SDL_RWclose(file);
    if (nb_read_total != res_size) {
      free(res);
      msg_err("read_text_files(): error during reading the file", path);
    }

    res[nb_read_total] = '\0';
    string str = string(res);
    free(res);
    return str;
  }
};

#endif















