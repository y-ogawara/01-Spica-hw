#include "Arduino.h"
#include "Udp.h"
#include <WiFi.h>
#include <WiFiUdp.h>

Udp::Udp()
{
  IPAddress local_IP(192, 168, 0, 76);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);
  
  WiFi.config(local_IP, gateway, subnet);
  delay(100);

  char ssid[] = "ERS-AP";
  char password[] = "1234567890";
  int local_port = 10000; // ポート番号

  WiFi.begin(ssid, password);
  delay(100);

  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println(" connected");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  wifi_udp.begin(local_port); // UDP通信の開始(引数はポート番号)
  Serial.print("Local port: ");
  Serial.println(local_port);
}

void Udp::recieve_packet()
{
  int packet_size = wifi_udp.parsePacket();
  if (packet_size)
  {
    //UDP情報の表示
    Serial.print(packet_size);
    IPAddress remote = wifi_udp.remoteIP();
  }
}
