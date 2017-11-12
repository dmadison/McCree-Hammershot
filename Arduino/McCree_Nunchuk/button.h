#ifndef button_h
#define button_h

struct button {
  const char key;
  boolean pressed = 0;
  
  button(char k) : key(k) {}

  void press(boolean state){
    if(state == pressed){ return; }
    state ?  Keyboard.press(key) : Keyboard.release(key);
    pressed = state;
  }
} ;

#endif
