#include "Arduino.h"
#include "Sensor.h"

int trig = 18; // 出力ピン
int echo = 19; // 入力ピン
Sensor::Sensor()
{
    //TODO ピンのセットアップ
}

float Sensor::getDistance()
{
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
    int loopNum = 4;
    float distance = 0.0;
    int zeroNum = 0;

    for (int i = 0; i < loopNum; i++)
    {
        // 超音波の出力終了
        digitalWrite(trig, LOW);
        delayMicroseconds(1);
        // 超音波を出力
        digitalWrite(trig, HIGH);
        delayMicroseconds(11);
        // 超音波を出力終了
        digitalWrite(trig, LOW);
        // 出力した超音波が返って来る時間を計測
        int t = pulseIn(echo, HIGH);
        // 計測した時間と音速から反射物までの距離を計算

        if (t * 0.017 > 300)
        {
            zeroNum++;
        }
        else
        {
            distance += t * 0.017;
        }
        delay(100);
    }
    float answerDistance = distance / loopNum - zeroNum;
    return answerDistance;
}
