#ifndef _Buzzer_h
#define _Buzzer_h
#include "Arduino.h"

#define LEDC_CHANNEL 5      // チャンネル
#define LEDC_TIMER_BIT 13
#define LEDC_BASE_FREQ 5000
class Buzzer {
 private:
    int BUZZER_PIN = 32;

public:
    Buzzer();
    void play(int oto);
    void stop();
};

#endif