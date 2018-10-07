#ifndef _Motor_h
#define _Motor_h

#include "Arduino.h"

class Motor
{
private:
  // 使うピンの定義
  int IN1 = 25;
  int IN2 = 26;
  int IN3 = 16;
  int IN4 = 17;

  // チャンネルの定義
  int CHANNEL_0 = 0;
  int CHANNEL_1 = 1;
  int CHANNEL_2 = 2;
  int CHANNEL_3 = 3;

  int LEDC_TIMER_BIT = 8;   // PWMの範囲(8bitなら0〜255、10bitなら0〜1023)
  int LEDC_BASE_FREQ = 490; // 周波数(Hz)
  int VALUE_MAX = 255;      // PWMの最大値

public:
  Motor();
  void run_motor(int state, uint32_t left_pwm, uint32_t right_pwm, int time);
  void forward(uint32_t left_pwm, uint32_t right_pwm);
  void back(uint32_t left_pwm, uint32_t right_pwm);
  void right(uint32_t pwm);
  void left(uint32_t pwm);
  void brake();
  void coast();
};

#endif
