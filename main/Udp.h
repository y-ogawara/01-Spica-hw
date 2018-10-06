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

    WiFiUDP wifi_udp;
    char packet_buffer[UDP_TX_PACKET_MAX_SIZE];
    String cmd_plus_split[UDP_TX_PACKET_MAX_SIZE] = {"\0"};
    String cmd_and_split[UDP_TX_PACKET_MAX_SIZE] = {"\0"};

  public:
    Udp(char ssid[], char password[], String ip);
    void recieve_packet();
    void send_data(char ip_send[],String text);
};

#endif
