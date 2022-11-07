// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GerdaXML_H
#define GerdaXML_H 1

namespace ge {

  using namespace pm;
  using namespace std;
  using namespace tinyxml2;

  // ======= some extra functions to work with XML using tinyxml ====================================================================
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

}//namespace
#endif  
