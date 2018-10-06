#include "Udp.h"
#include "Arduino.h"
#include <WiFi.h>
#include <WiFiUdp.h>

Udp::Udp(){}

void Udp::setup_udp(char ssid[], char password[], String ip)
{
  IPAddress local_IP(192, 168, 1, 19);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.config(local_IP, gateway, subnet);
  delay(100);

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
  //Serial.println(packet_size);
  
  if (packet_size)
  {
    //UDP情報の表示
    Serial.print("Received packet of size ");
    Serial.println(packet_size);

    // 実際にパケットを読む
    wifi_udp.read(packet_buffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.println("Contents:");
    Serial.println(packet_buffer);
  }
}

void Udp::send_data(IPAddress ip_send,String text)
{ 
  Serial.print("send...");
  IPAddress remote_ip(192, 168, 1, 10);
  char remote[] = "192.168.1.10";
  //char message[] = "test";
  
  wifi_udp.beginPacket(remote, local_port);
  wifi_udp.printf("from ESP8266\r\n");
  wifi_udp.endPacket();
  delay(1000);
}
