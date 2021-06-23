// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine
#ifndef GERDA_INPUT_DEV_H
#define GERDA_INPUT_DEV 1

namespace ge {
  
  // ======= MOUSE ====================================================================
  class Mouse{
    public:
      void Tick(){
        left_up   = false;
        right_up  = false;
        middle_up = false;
        left_down   = false;
        right_down  = false;
        middle_down = false;

        locker = false;
        wheel = 0;
      }

      void ApplyEvent(const SDL_Event & game_event){
        if(locker) return;
        // msg(SDL_BUTTON(SDL_BUTTON_LEFT), SDL_BUTTON(SDL_BUTTON_MIDDLE), SDL_BUTTON(SDL_BUTTON_RIGHT), SDL_BUTTON(game_event.button.button));
        if( game_event.type == SDL_MOUSEBUTTONDOWN ){
          if(game_event.button.button == SDL_BUTTON_LEFT  ) {left_down   = true; left_hold = true; }
          if(game_event.button.button == SDL_BUTTON_MIDDLE) {middle_down = true; middle_hold = true; }
          if(game_event.button.button == SDL_BUTTON_RIGHT ) {right_down  = true; right_hold = true; }
        }
        if( game_event.type == SDL_MOUSEBUTTONUP ){
          if(game_event.button.button == SDL_BUTTON_LEFT  ) {left_up   = true; left_hold = false; }
          if(game_event.button.button == SDL_BUTTON_MIDDLE) {middle_up = true; middle_hold = false; }
          if(game_event.button.button == SDL_BUTTON_RIGHT ) {right_up  = true; right_hold = false; }
        }
        locker = true; // lock mouse for any other mouse events in this tick
      }

      // ALLEGRO_MOUSE_STATE state;
      Uint32 state;
      int x, y, wheel;
      bool left_hold, right_hold, middle_hold;
      bool left_down, right_down, middle_down;
      bool left_up,   right_up,   middle_up;
      bool locker;
  };

  ostream & operator << (ostream & out, const Mouse & m){
    cout << "ge::Mouse pos(" << m.x << ", " << m.y << ") ";
    cout << "hold(" << m.left_hold << ", " << m.right_hold << ", " << m.middle_hold << ") ";
    cout << "up(" << m.left_up << ", " << m.right_up << ", " << m.middle_up << ") ";
    cout << "down(" << m.left_down << ", " << m.right_down << ", " << m.middle_down << ") wheel(" << m.wheel << ")";
    return out;
  };

  // ======= KEYBOARD ====================================================================
  namespace key {
    enum{
      A  = SDL_SCANCODE_A ,
      B  =     SDL_SCANCODE_B ,
      C  =     SDL_SCANCODE_C ,
      D  =     SDL_SCANCODE_D ,
      E  =     SDL_SCANCODE_E ,
      F  =     SDL_SCANCODE_F ,
      G  =     SDL_SCANCODE_G ,
      H  =     SDL_SCANCODE_H ,
      I  =     SDL_SCANCODE_I ,
      J  =     SDL_SCANCODE_J ,
      K  =     SDL_SCANCODE_K ,
      L  =     SDL_SCANCODE_L ,
      M  =     SDL_SCANCODE_M ,
      N  =     SDL_SCANCODE_N ,
      O  =     SDL_SCANCODE_O ,
      P  =     SDL_SCANCODE_P ,
      Q  =     SDL_SCANCODE_Q ,
      R  =     SDL_SCANCODE_R ,
      S  =     SDL_SCANCODE_S ,
      T  =     SDL_SCANCODE_T ,
      U  =     SDL_SCANCODE_U ,
      V  =     SDL_SCANCODE_V ,
      W  =     SDL_SCANCODE_W ,
      X  =     SDL_SCANCODE_X ,
      Y  =     SDL_SCANCODE_Y ,
      Z  =     SDL_SCANCODE_Z ,
      K_1  =     SDL_SCANCODE_1 ,
      K_2  =     SDL_SCANCODE_2 ,
      K_3  =     SDL_SCANCODE_3 ,
      K_4  =     SDL_SCANCODE_4 ,
      K_5  =     SDL_SCANCODE_5 ,
      K_6  =     SDL_SCANCODE_6 ,
      K_7  =     SDL_SCANCODE_7 ,
      K_8  =     SDL_SCANCODE_8 ,
      K_9  =     SDL_SCANCODE_9 ,
      K_0  =     SDL_SCANCODE_0 ,
      RETURN  =     SDL_SCANCODE_RETURN ,
      ESCAPE  =     SDL_SCANCODE_ESCAPE ,
      BACKSPACE  =     SDL_SCANCODE_BACKSPACE ,
      TAB  =     SDL_SCANCODE_TAB ,
      SPACE  =     SDL_SCANCODE_SPACE ,
      MINUS  =     SDL_SCANCODE_MINUS ,
      EQUALS  =     SDL_SCANCODE_EQUALS ,
      LEFTBRACKET  =     SDL_SCANCODE_LEFTBRACKET ,
      RIGHTBRACKET  =     SDL_SCANCODE_RIGHTBRACKET ,
      BACKSLASH  =     SDL_SCANCODE_BACKSLASH ,
      NONUSHASH  =     SDL_SCANCODE_NONUSHASH ,
      SEMICOLON  =     SDL_SCANCODE_SEMICOLON ,
      APOSTROPHE  =     SDL_SCANCODE_APOSTROPHE ,
      GRAVE  =     SDL_SCANCODE_GRAVE ,
      COMMA  =     SDL_SCANCODE_COMMA ,
      PERIOD  =     SDL_SCANCODE_PERIOD ,
      SLASH  =     SDL_SCANCODE_SLASH ,
      CAPSLOCK  =     SDL_SCANCODE_CAPSLOCK ,
      F1  =     SDL_SCANCODE_F1 ,
      F2  =     SDL_SCANCODE_F2 ,
      F3  =     SDL_SCANCODE_F3 ,
      F4  =     SDL_SCANCODE_F4 ,
      F5  =     SDL_SCANCODE_F5 ,
      F6  =     SDL_SCANCODE_F6 ,
      F7  =     SDL_SCANCODE_F7 ,
      F8  =     SDL_SCANCODE_F8 ,
      F9  =     SDL_SCANCODE_F9 ,
      F10  =     SDL_SCANCODE_F10 ,
      F11  =     SDL_SCANCODE_F11 ,
      F12  =     SDL_SCANCODE_F12 ,
      PRINTSCREEN  =     SDL_SCANCODE_PRINTSCREEN ,
      SCROLLLOCK  =     SDL_SCANCODE_SCROLLLOCK ,
      PAUSE  =     SDL_SCANCODE_PAUSE ,
      INSERT  =     SDL_SCANCODE_INSERT ,
      HOME  =     SDL_SCANCODE_HOME ,
      PAGEUP  =     SDL_SCANCODE_PAGEUP ,
      DELETE  =     SDL_SCANCODE_DELETE ,
      END  =     SDL_SCANCODE_END ,
      PAGEDOWN  =     SDL_SCANCODE_PAGEDOWN ,
      RIGHT  =     SDL_SCANCODE_RIGHT ,
      LEFT  =     SDL_SCANCODE_LEFT ,
      DOWN  =     SDL_SCANCODE_DOWN ,
      UP  =     SDL_SCANCODE_UP ,
      NUMLOCKCLEAR  =     SDL_SCANCODE_NUMLOCKCLEAR ,
      KP_DIVIDE  =     SDL_SCANCODE_KP_DIVIDE ,
      KP_MULTIPLY  =     SDL_SCANCODE_KP_MULTIPLY ,
      KP_MINUS  =     SDL_SCANCODE_KP_MINUS ,
      KP_PLUS  =     SDL_SCANCODE_KP_PLUS ,
      KP_ENTER  =     SDL_SCANCODE_KP_ENTER ,
      KP_1  =     SDL_SCANCODE_KP_1 ,
      KP_2  =     SDL_SCANCODE_KP_2 ,
      KP_3  =     SDL_SCANCODE_KP_3 ,
      KP_4  =     SDL_SCANCODE_KP_4 ,
      KP_5  =     SDL_SCANCODE_KP_5 ,
      KP_6  =     SDL_SCANCODE_KP_6 ,
      KP_7  =     SDL_SCANCODE_KP_7 ,
      KP_8  =     SDL_SCANCODE_KP_8 ,
      KP_9  =     SDL_SCANCODE_KP_9 ,
      KP_0  =     SDL_SCANCODE_KP_0 ,
      KP_PERIOD  =     SDL_SCANCODE_KP_PERIOD ,
      NONUSBACKSLASH  =     SDL_SCANCODE_NONUSBACKSLASH ,
      APPLICATION  =     SDL_SCANCODE_APPLICATION ,
      POWER  =     SDL_SCANCODE_POWER ,
      KP_EQUALS  =     SDL_SCANCODE_KP_EQUALS ,
      F13  =     SDL_SCANCODE_F13 ,
      F14  =     SDL_SCANCODE_F14 ,
      F15  =     SDL_SCANCODE_F15 ,
      F16  =     SDL_SCANCODE_F16 ,
      F17  =     SDL_SCANCODE_F17 ,
      F18  =     SDL_SCANCODE_F18 ,
      F19  =     SDL_SCANCODE_F19 ,
      F20  =     SDL_SCANCODE_F20 ,
      F21  =     SDL_SCANCODE_F21 ,
      F22  =     SDL_SCANCODE_F22 ,
      F23  =     SDL_SCANCODE_F23 ,
      F24  =     SDL_SCANCODE_F24 ,
      EXECUTE  =     SDL_SCANCODE_EXECUTE ,
      HELP  =     SDL_SCANCODE_HELP ,
      MENU  =     SDL_SCANCODE_MENU ,
      SELECT  =     SDL_SCANCODE_SELECT ,
      STOP  =     SDL_SCANCODE_STOP ,
      AGAIN  =     SDL_SCANCODE_AGAIN ,
      UNDO  =     SDL_SCANCODE_UNDO ,
      CUT  =     SDL_SCANCODE_CUT ,
      COPY  =     SDL_SCANCODE_COPY ,
      PASTE  =     SDL_SCANCODE_PASTE ,
      FIND  =     SDL_SCANCODE_FIND ,
      MUTE  =     SDL_SCANCODE_MUTE ,
      VOLUMEUP  =     SDL_SCANCODE_VOLUMEUP ,
      VOLUMEDOWN  =     SDL_SCANCODE_VOLUMEDOWN ,
      KP_COMMA  =     SDL_SCANCODE_KP_COMMA ,
      KP_EQUALSAS400  =     SDL_SCANCODE_KP_EQUALSAS400 ,
      INTERNATIONAL1  =     SDL_SCANCODE_INTERNATIONAL1 ,
      INTERNATIONAL2  =     SDL_SCANCODE_INTERNATIONAL2 ,
      INTERNATIONAL3  =     SDL_SCANCODE_INTERNATIONAL3 ,
      INTERNATIONAL4  =     SDL_SCANCODE_INTERNATIONAL4 ,
      INTERNATIONAL5  =     SDL_SCANCODE_INTERNATIONAL5 ,
      INTERNATIONAL6  =     SDL_SCANCODE_INTERNATIONAL6 ,
      INTERNATIONAL7  =     SDL_SCANCODE_INTERNATIONAL7 ,
      INTERNATIONAL8  =     SDL_SCANCODE_INTERNATIONAL8 ,
      INTERNATIONAL9  =     SDL_SCANCODE_INTERNATIONAL9 ,
      LANG1  =     SDL_SCANCODE_LANG1 ,
      LANG2  =     SDL_SCANCODE_LANG2 ,
      LANG3  =     SDL_SCANCODE_LANG3 ,
      LANG4  =     SDL_SCANCODE_LANG4 ,
      LANG5  =     SDL_SCANCODE_LANG5 ,
      LANG6  =     SDL_SCANCODE_LANG6 ,
      LANG7  =     SDL_SCANCODE_LANG7 ,
      LANG8  =     SDL_SCANCODE_LANG8 ,
      LANG9  =     SDL_SCANCODE_LANG9 ,
      ALTERASE  =     SDL_SCANCODE_ALTERASE ,
      SYSREQ  =     SDL_SCANCODE_SYSREQ ,
      CANCEL  =     SDL_SCANCODE_CANCEL ,
      CLEAR  =     SDL_SCANCODE_CLEAR ,
      PRIOR  =     SDL_SCANCODE_PRIOR ,
      RETURN2  =     SDL_SCANCODE_RETURN2 ,
      SEPARATOR  =     SDL_SCANCODE_SEPARATOR ,
      OUT  =     SDL_SCANCODE_OUT ,
      OPER  =     SDL_SCANCODE_OPER ,
      CLEARAGAIN  =     SDL_SCANCODE_CLEARAGAIN ,
      CRSEL  =     SDL_SCANCODE_CRSEL ,
      EXSEL  =     SDL_SCANCODE_EXSEL ,
      KP_00  =     SDL_SCANCODE_KP_00 ,
      KP_000  =     SDL_SCANCODE_KP_000 ,
      THOUSANDSSEPARATOR  =     SDL_SCANCODE_THOUSANDSSEPARATOR ,
      DECIMALSEPARATOR  =     SDL_SCANCODE_DECIMALSEPARATOR ,
      CURRENCYUNIT  =     SDL_SCANCODE_CURRENCYUNIT ,
      CURRENCYSUBUNIT  =     SDL_SCANCODE_CURRENCYSUBUNIT ,
      KP_LEFTPAREN  =     SDL_SCANCODE_KP_LEFTPAREN ,
      KP_RIGHTPAREN  =     SDL_SCANCODE_KP_RIGHTPAREN ,
      KP_LEFTBRACE  =     SDL_SCANCODE_KP_LEFTBRACE ,
      KP_RIGHTBRACE  =     SDL_SCANCODE_KP_RIGHTBRACE ,
      KP_TAB  =     SDL_SCANCODE_KP_TAB ,
      KP_BACKSPACE  =     SDL_SCANCODE_KP_BACKSPACE ,
      KP_A  =     SDL_SCANCODE_KP_A ,
      KP_B  =     SDL_SCANCODE_KP_B ,
      KP_C  =     SDL_SCANCODE_KP_C ,
      KP_D  =     SDL_SCANCODE_KP_D ,
      KP_E  =     SDL_SCANCODE_KP_E ,
      KP_F  =     SDL_SCANCODE_KP_F ,
      KP_XOR  =     SDL_SCANCODE_KP_XOR ,
      KP_POWER  =     SDL_SCANCODE_KP_POWER ,
      KP_PERCENT  =     SDL_SCANCODE_KP_PERCENT ,
      KP_LESS  =     SDL_SCANCODE_KP_LESS ,
      KP_GREATER  =     SDL_SCANCODE_KP_GREATER ,
      KP_AMPERSAND  =     SDL_SCANCODE_KP_AMPERSAND ,
      KP_DBLAMPERSAND  =     SDL_SCANCODE_KP_DBLAMPERSAND ,
      KP_VERTICALBAR  =     SDL_SCANCODE_KP_VERTICALBAR ,
      KP_DBLVERTICALBAR  =     SDL_SCANCODE_KP_DBLVERTICALBAR ,
      KP_COLON  =     SDL_SCANCODE_KP_COLON ,
      KP_HASH  =     SDL_SCANCODE_KP_HASH ,
      KP_SPACE  =     SDL_SCANCODE_KP_SPACE ,
      KP_AT  =     SDL_SCANCODE_KP_AT ,
      KP_EXCLAM  =     SDL_SCANCODE_KP_EXCLAM ,
      KP_MEMSTORE  =     SDL_SCANCODE_KP_MEMSTORE ,
      KP_MEMRECALL  =     SDL_SCANCODE_KP_MEMRECALL ,
      KP_MEMCLEAR  =     SDL_SCANCODE_KP_MEMCLEAR ,
      KP_MEMADD  =     SDL_SCANCODE_KP_MEMADD ,
      KP_MEMSUBTRACT  =     SDL_SCANCODE_KP_MEMSUBTRACT ,
      KP_MEMMULTIPLY  =     SDL_SCANCODE_KP_MEMMULTIPLY ,
      KP_MEMDIVIDE  =     SDL_SCANCODE_KP_MEMDIVIDE ,
      KP_PLUSMINUS  =     SDL_SCANCODE_KP_PLUSMINUS ,
      KP_CLEAR  =     SDL_SCANCODE_KP_CLEAR ,
      KP_CLEARENTRY  =     SDL_SCANCODE_KP_CLEARENTRY ,
      KP_BINARY  =     SDL_SCANCODE_KP_BINARY ,
      KP_OCTAL  =     SDL_SCANCODE_KP_OCTAL ,
      KP_DECIMAL  =     SDL_SCANCODE_KP_DECIMAL ,
      KP_HEXADECIMAL  =     SDL_SCANCODE_KP_HEXADECIMAL ,
      LCTRL  =     SDL_SCANCODE_LCTRL ,
      LSHIFT  =     SDL_SCANCODE_LSHIFT ,
      LALT  =     SDL_SCANCODE_LALT ,
      LGUI  =     SDL_SCANCODE_LGUI ,
      RCTRL  =     SDL_SCANCODE_RCTRL ,
      RSHIFT  =     SDL_SCANCODE_RSHIFT ,
      RALT  =     SDL_SCANCODE_RALT ,
      RGUI  =     SDL_SCANCODE_RGUI ,
    };

    int KSTART = 4;
    int KEND   = 231;
  };

  class Keyboard {
    public:
      Keyboard(){
        hor = 0; ver = 0;
        actz = false; actx = false; exit = false; screenshoot = false; hor_pressed = false; ver_pressed = false;
        for(int key = key::KSTART; key < key::KEND; key++){
          keyboard_pressed[key] = 0;
          keyboard_holded[key]  = 0;
          keyboard_raised[key]  = 0;
        }
      }

      void Tick(){
        // update keyboard
        kstate = SDL_GetKeyboardState(NULL);
        for(int key = key::KSTART; key < key::KEND; key++){
          if( kstate[key] ){
            if( not keyboard_holded[key] ){
              keyboard_pressed[key] = true;
              keyboard_holded[key] = true;
             }
             else keyboard_pressed[key] = false;
           }
           else{
            if( keyboard_holded[key] ){
              keyboard_holded[key] = false;
              keyboard_raised[key] = true;
              keyboard_pressed[key] = false;
            }
            else keyboard_raised[key] = false;
          }
        }
        // some regular event - this should be configuradable
        hor = check_sign( Holded(key::D) + Holded(key::RIGHT) - Holded(key::A) - Holded(key::LEFT));
        ver = check_sign( Holded(key::W) + Holded(key::UP) - Holded(key::S) - Holded(key::DOWN));
        actz = Pressed(key::Z) + Pressed(key::K) + Pressed(key::RETURN) + Pressed(key::SPACE);
        actx = Pressed(key::X) + Pressed(key::L);
        hor_pressed = check_sign( Pressed(key::D) + Pressed(key::RIGHT) - Pressed(key::A) - Pressed(key::LEFT)  );
        ver_pressed = check_sign( Pressed(key::W) + Pressed(key::UP) - Pressed(key::S) - Pressed(key::DOWN));
        exit = Pressed(key::ESCAPE);
        screenshoot = Holded(key::KP_0);
      }

      inline bool Pressed(int key){ return keyboard_pressed[key]; }
      inline bool Holded(int key) { return keyboard_holded[key];  }
      inline bool Raised(int key) { return keyboard_raised[key];  }

      const Uint8 * kstate;
      int hor, ver;
      bool actz, actx, exit, screenshoot, hor_pressed, ver_pressed;
      bool keyboard_pressed[232], keyboard_holded[232], keyboard_raised[232];
  };
}

#endif 




