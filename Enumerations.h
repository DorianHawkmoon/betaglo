#include "Arduino.h"

#ifndef ENUMERATION
#define ENUMERATION
//enum state button/click
enum ClickButton{
  no_click = 0,
  normal_click,
  long_click
};

//enum of states/commands to sent to computer
enum Commands {
  button_right = 1, //los tres primeros dedos
  button_left = 2,
  button_middle = 3,
  button_right_strong = 4, //los tres primeros dedos mas fuerte
  button_left_strong = 5,
  button_middle_strong = 6,

  axis_x_forward = 7,//flexion dos dedos
  axis_x_back = 8,

  axis_y_forward = 9, //flexion otros dos dedos
  axis_y_back = 10,
};

enum States {
  mouse,
  keyboard,
  patterns
};

enum StatesKeyboard {
  minusculas,
  mayusculas,
  numeros,
  simbolos
};

#endif
