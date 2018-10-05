#include "Udp.h"
#include "Motor.h"
#include "Sensor.h"

Motor motor;
Udp udp;
Sensor sensor;

void setup()
{
    Serial.begin(115200);

    Serial.print("setup start!");

    motor = Motor();
    udp = Udp();
    sensor = Sensor();
}

void loop()
{
    udp.recieve_packet();
}
