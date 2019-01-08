#ifndef _Led_h
#define _Led_h
#include "Arduino.h"

class Led {
 private:
  // TODO 使うピンの定義
  int GREEN_LED = 22;
  int RED_LED = 23;

 public:
  Led();
  void green_led_flash(int pattern);
  void red_led_flash(int pattern);
  void green_led_on();
  void red_led_on();
  void green_led_off();
  void red_led_off();
};

#endif