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

    char ssid[] = "robop-WiFi-n";
    char password[] = "robop0304";
    udp.setup_udp(ssid, password, "");
//
//    IPAddress send_IP(192, 168, 1, 24);
//    String text = "test";
//    udp.send_data(send_IP, text);
}

void loop()
{
  udp.recieve_packet();
}
