#include "Buzzer.h"
#include "Arduino.h"

Buzzer::Buzzer() {
  // pinMode(BUZZER_PIN, OUTPUT);
  ledcSetup(LEDC_CHANNEL, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcAttachPin(BUZZER_PIN, LEDC_CHANNEL);
}

void Buzzer::play(int oto) {
    int tmp_num = 0;
    switch(oto) {
        case 1:
            tmp_num = 262;
            break;
        case 2:
            tmp_num = 293;
            break;
        case 3:
            tmp_num = 330;
            break;
        case 4:
            tmp_num = 392;
            break;
        case 5:
            tmp_num = 440;
            break;
        case 6:
            tmp_num = 494;
            break;
    }
     ledcWriteTone(LEDC_CHANNEL, tmp_num); }

void Buzzer::stop() { ledcWriteTone(LEDC_CHANNEL, 0); }