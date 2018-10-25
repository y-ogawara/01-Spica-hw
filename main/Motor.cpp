#include "Arduino.h"
#include "Motor.h"

Motor::Motor()
{
    Serial.println("Motor constractor run");

    pinMode(IN1, OUTPUT); // IN1
    pinMode(IN2, OUTPUT); // IN2
    pinMode(IN3, OUTPUT); // IN3
    pinMode(IN4, OUTPUT); // IN4

    // ピンのセットアップ
    ledcSetup(CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
    ledcSetup(CHANNEL_1, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
    ledcSetup(CHANNEL_2, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
    ledcSetup(CHANNEL_3, LEDC_BASE_FREQ, LEDC_TIMER_BIT);

    // ピンのチャンネルをセット
    ledcAttachPin(IN1, CHANNEL_0);
    ledcAttachPin(IN2, CHANNEL_1);
    ledcAttachPin(IN3, CHANNEL_2);
    ledcAttachPin(IN4, CHANNEL_3);

    delay(100);
}

void Motor::run_motor(BlockModel blockModel) //int state, uint32_t left_pwm, uint32_t right_pwm, int time
{
    debug_show_command(blockModel);  //Serial.printでデバッグする関数

    int state = blockModel.get_block_state();
    uint32_t left_pwm = blockModel.get_left_speed();
    uint32_t right_pwm = blockModel.get_right_speed();
    int time = blockModel.get_time() * 100;  //FIXME get_time()が10で1秒なので、単位msに変えるので*100してる  なんかWrapper作ったほうが良さげ?

    switch(state)
    {
      case 1:  //forward
        forward(left_pwm, right_pwm);
        break;

      case 2:  //back
        back(left_pwm, right_pwm);
        break;

      case 3: //left
        left(left_pwm);
        break;

      case 4: //right
        right(right_pwm);
        break;
    }

    delay(time);
    coast(); //空転
    delay(400);
}

void Motor::forward(uint32_t left_pwm, uint32_t right_pwm)
{ 
    if (left_pwm > VALUE_MAX)
    {
        left_pwm = VALUE_MAX;
    }
    if (right_pwm > VALUE_MAX)
    {
        right_pwm = VALUE_MAX;
    }

    ledcWrite(CHANNEL_0, 0);
    ledcWrite(CHANNEL_1, left_pwm);
    ledcWrite(CHANNEL_2, 0);
    ledcWrite(CHANNEL_3, right_pwm);
}

void Motor::back(uint32_t left_pwm, uint32_t right_pwm)
{ 
    if (left_pwm > VALUE_MAX)
    {
        left_pwm = VALUE_MAX;
    }
    if (right_pwm > VALUE_MAX)
    {
        right_pwm = VALUE_MAX;
    }

    ledcWrite(CHANNEL_0, left_pwm);
    ledcWrite(CHANNEL_1, 0);
    ledcWrite(CHANNEL_2, right_pwm);
    ledcWrite(CHANNEL_3, 0);
}

void Motor::right(uint32_t pwm)
{ 
    if (pwm > VALUE_MAX)
    {
        pwm = VALUE_MAX;
    }
    ledcWrite(CHANNEL_0, 0);
    ledcWrite(CHANNEL_1, pwm);
    ledcWrite(CHANNEL_2, 0);
    ledcWrite(CHANNEL_3, 0);
}

void Motor::left(uint32_t pwm)
{ 
    if (pwm > VALUE_MAX)
    {
        pwm = VALUE_MAX;
    }
    ledcWrite(CHANNEL_0, 0);
    ledcWrite(CHANNEL_1, 0);
    ledcWrite(CHANNEL_2, 0);
    ledcWrite(CHANNEL_3, pwm);
}

void Motor::brake()
{
    ledcWrite(CHANNEL_0, VALUE_MAX);
    ledcWrite(CHANNEL_1, VALUE_MAX);
    ledcWrite(CHANNEL_2, VALUE_MAX);
    ledcWrite(CHANNEL_3, VALUE_MAX);
}

void Motor::coast()
{
    ledcWrite(CHANNEL_0, 0);
    ledcWrite(CHANNEL_1, 0);
    ledcWrite(CHANNEL_2, 0);
    ledcWrite(CHANNEL_3, 0);
}

void Motor::debug_show_command(BlockModel blockModel)
{
    int state = blockModel.get_block_state();
    float time = blockModel.get_time() * 0.1; // get_time()が10で1秒

    switch(state)
    {
        case 1:
            Serial.print("Exec front ");
            Serial.print(time);
            Serial.println("sec");
            break;
        case 2:
            Serial.print("Exec  back ");
            Serial.print(time);
            Serial.println("sec");
            break;
        case 3:
            Serial.print("Exec  left ");
            Serial.print(time);
            Serial.println("sec");
            break;
        case 4:
            Serial.print("Exec right ");
            Serial.print(time);
            Serial.println("sec");
            break;
        default:
            Serial.print("Exec error");
            break;
    }
}
