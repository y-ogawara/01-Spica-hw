#include "Udp.h"
#include "Motor.h"
#include "Sensor.h"

Motor motor;
char ssid[] = "aaa";
char password[] = "";
static Udp udp = Udp(ssid,password,"");
Sensor sensor;

void setup()
{
    Serial.begin(115200);

    Serial.print("setup start!");

    motor = Motor();
    sensor = Sensor();
}

void loop()
{
    udp.recieve_packet();
}
