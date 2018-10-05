#include "Arduino.h"
#include "Udp.h"
#include <WiFi.h>
#include <WiFiUdp.h>

Udp::Udp()
{
  WiFi.config(local_IP, gateway, subnet);
  delay(100);

  WiFi.begin(SSID, PASSWORD);
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
  wifi_udp.begin(LOCAL_PORT); // UDP通信の開始(引数はポート番号)
  Serial.print("Local port: ");
  Serial.println(LOCAL_PORT);
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
