#ifndef _BlockModel_h
#define _BlockModel_h

#include "Arduino.h"

class BlockModel
{
private:
  int block_state;
  int left_speed;
  int right_speed;
  int time;
  int loop_count;

public:
  BlockModel();
  BlockModel(int block_state, int right_speed, int left_speed, int time);
  int get_block_state();
  void set_block_state(int block_state);
  int get_right_speed();
  void set_right_speed(int right_speed);
  int get_left_speed();
  void set_left_speed(int left_speed);
  int get_time();
  void set_time(int time);
  int get_loop_count();
  void set_loop_count(int loop_count);
};

#endif
