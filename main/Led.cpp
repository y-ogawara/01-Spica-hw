#include "Led.h"
#include "Arduino.h"

Led::Led() {
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
}

void Led::green_led_flash(int pattern) {
  switch (pattern) {
    case 1:
      for (int i = 0; i < 10; i++) {
        green_led_on();
        delay(100);
        green_led_off();
        delay(100);
      }
      break;
    case 2:
      for (int i = 0; i < 5; i++) {
        green_led_on();
        delay(500);
        green_led_off();
        delay(500);
      }
      break;
    case 3:
      green_led_on();
      delay(1000);
      green_led_off();
      break;
    case 4:
      break;
  }
}
void Led::red_led_flash(int pattern) {
  switch (pattern) {
    case 1:
      for (int i = 0; i < 10; i++) {
        red_led_on();
        delay(100);
        red_led_off();
        delay(100);
      }
      break;
    case 2:
      for (int i = 0; i < 5; i++) {
        red_led_on();
        delay(500);
        red_led_off();
        delay(500);
      }
      break;
    case 3:
      red_led_on();
      delay(1000);
      red_led_off();
      break;
    case 4:

      break;
  }
}

void Led::green_led_on() { digitalWrite(GREEN_LED, LOW); }
void Led::red_led_on() { digitalWrite(RED_LED, LOW); }
void Led::green_led_off() { digitalWrite(GREEN_LED, HIGH); }
void Led::red_led_off() { digitalWrite(RED_LED, HIGH); }