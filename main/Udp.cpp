#include "Udp.h"
#include "Arduino.h"
#include <WiFi.h>
#include <WiFiUdp.h>

Udp::Udp() {}

void Udp::setup_udp(char ssid[], char password[], String ip)
{
  IPAddress local_IP(192, 168, 0, 19);
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

String Udp::get_packet_buffer()
{
  return String(this->packet_buffer);
}

void Udp::clear_packet_buffer()
{
  int packetBuffer_length = strlen(this->packet_buffer);
  memset(this->packet_buffer, '\0', packetBuffer_length);
}

void Udp::send_data(char remote_ip[], char text[])
{
  Serial.println("send...");

  wifi_udp.beginPacket(remote_ip, local_port);
  wifi_udp.printf(text);
  wifi_udp.endPacket();
  delay(1000);
}
