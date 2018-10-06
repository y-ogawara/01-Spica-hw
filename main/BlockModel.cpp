#include "Arduino.h"
#include "BlockModel.h"

BlockModel::BlockModel(){}
BlockModel::BlockModel(int block_state, int right_speed, int left_speed, int time)
{
    set_block_state(block_state);
    set_right_speed(right_speed);
    set_left_speed(left_speed);
    set_time(time);
}

int BlockModel::get_block_state()
{
    return this->block_state;
}

void BlockModel::set_block_state(int block_state)
{
    this->block_state = block_state;
}

int BlockModel::get_right_speed()
{
    return this->right_speed;
}

void BlockModel::set_right_speed(int right_speed)
{
    this->right_speed = right_speed;
}

int BlockModel::get_left_speed()
{
    return this->left_speed;
}

void BlockModel::set_left_speed(int left_speed)
{
    this->left_speed = left_speed;
}

int BlockModel::get_time()
{
    return this->time;
}

void BlockModel::set_time(int time)
{
    this->time = time;
}

int BlockModel::get_loop_count()
{
    return this->loop_count;
}

void BlockModel::set_loop_count(int loop_count)
{
    this->loop_count = loop_count;
}
