#ifndef _Udp_h
#define _Udp_h

#include "Arduino.h"
#include <WiFi.h>
#include <WiFiUdp.h>

#define UDP_TX_PACKET_MAX_SIZE 500

class Udp
{
  private:
    // wifiの設定
    const char SSID[] = "ERS-AP";
    const char PASSWORD[] = "1234567890";
    const int LOCAL_PORT = 10000; // ポート番号

    IPAddress local_IP(192, 168, 0, 76);
    IPAddress gateway(192, 168, 0, 1);
    IPAddress subnet(255, 255, 255, 0);

    WiFiUDP wifi_udp;
    char packet_buffer[UDP_TX_PACKET_MAX_SIZE];
    String cmd_plus_split[UDP_TX_PACKET_MAX_SIZE] = {"\0"};
    String cmd_and_split[UDP_TX_PACKET_MAX_SIZE] = {"\0"};

  public:
    Udp();
    void recieve_packet();
};

#endif
