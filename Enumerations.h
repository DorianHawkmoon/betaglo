#include "Arduino.h"

#ifndef ENUMERATION
#define ENUMERATION
//enum state button/click
enum ClickButton{
  normal_click,
  long_click,
  no_click
};

//enum of states/commands to sent to computer
enum Commands {
  button_right = (1 << 0), //los tres primeros dedos
  button_left = (1 << 1),
  button_middle = (1 << 2),
  button_right_strong = (1 << 3), //los tres primeros dedos mas fuerte
  button_left_strong = (1 << 4),
  button_middle_strong = (1 << 5),

  axis_x_forward = (1 << 6),//flexion dos dedos
  axis_x_back = (1 << 7),

  axis_y_forward = (1 << 8), //flexion otros dos dedos
  axis_y_back = (1 << 9),
};

#endif
