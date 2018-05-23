#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiMulti.h>
#include <WiFiUdp.h>
#include <WiFiScan.h>
#include <ETH.h>
#include <WiFiClient.h>
#include <WiFiSTA.h>
#include <WiFiServer.h>
#include <WiFiType.h>
#include <WiFiGeneric.h>

// 使うピンの定義
const int IN1 = 25;
const int IN2 = 26;
const int IN3 = 32;
const int IN4 = 33;

// チャンネルの定義
const int CHANNEL_0 = 0;
const int CHANNEL_1 = 1;
const int CHANNEL_2 = 2;
const int CHANNEL_3 = 3;

const int LEDC_TIMER_BIT = 8;   // PWMの範囲(8bitなら0〜255、10bitなら0〜1023)
const int LEDC_BASE_FREQ = 490; // 周波数(Hz)
const int VALUE_MAX = 255;      // PWMの最大値

// wifiの設定
const char SSID[] = "ayushio";
const char PASSWORD[] = "password";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.print("setup start!");
  pinMode(IN1, OUTPUT); // IN1
  pinMode(IN2, OUTPUT); // IN2
  pinMode(IN3, OUTPUT); // IN3
  pinMode(IN4, OUTPUT); // IN4

  // ピンのセットアップ
  ledcSetup(CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcSetup(CHANNEL_1, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcSetup(CHANNEL_2, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcSetup(CHANNEL_3, LEDC_BASE_FREQ, LEDC_TIMER_BIT);

  // ピンのチャンネルをセット
  ledcAttachPin(IN1, CHANNEL_0);
  ledcAttachPin(IN2, CHANNEL_1);
  ledcAttachPin(IN3, CHANNEL_2);
  ledcAttachPin(IN4, CHANNEL_3);



  WiFi.begin(SSID, PASSWORD);
  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println(" connected");
  Serial.print("HTTP Server: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  server.begin();
}

void loop() {
//  right(100);
//  delay(1000);
//  coast();
//  delay(1000);
//  
//  left(100);
//  delay(1000);
//  coast();
//  delay(1000);
//  
//  forward(100);
//  delay(1000);
//  coast();
//  delay(1000);
//  
//  reverse(100);
//  delay(1000);
//  coast();
//  delay(1000);

  WiFiClient client = server.available();
 
  if (client) {
    Serial.println("new client");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
 
            client.println("<!DOCTYPE html>");
            client.println("<html>");
            client.println("<body>");
            client.println("<form method='get'>");
            client.println("<font size='4'>test<br>");
            client.println("ESP32-Wi-Fi</font><br>");
            client.println("<br>");
            client.println("<input type='submit' name=0 value='ON'>");
            client.println("<input type='submit' name=1 value='OFF'>");
            client.println("</form>");
            client.println("</body>");
            client.println("</html>");
 
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
 
        if (currentLine.endsWith("GET /?0=ON")) {
          digitalWrite(13, HIGH);
        }
        if (currentLine.endsWith("GET /?1=OFF")) {
          digitalWrite(13, LOW);
        }
      }
    }
     
    client.stop();
    Serial.println("client disonnected");
  }
}

// 正転
void forward(uint32_t pwm) {
  if (pwm > VALUE_MAX) {
    pwm = VALUE_MAX;
  }
  ledcWrite(CHANNEL_0, 0);
  ledcWrite(CHANNEL_1, pwm);
  ledcWrite(CHANNEL_2, 0);
  ledcWrite(CHANNEL_3, pwm);
}

// 逆転
void reverse(uint32_t pwm) {
  if (pwm > VALUE_MAX) {
    pwm = VALUE_MAX;
  }
  ledcWrite(CHANNEL_0, pwm);
  ledcWrite(CHANNEL_1, 0);
  ledcWrite(CHANNEL_2, pwm);
  ledcWrite(CHANNEL_3, 0);
}

// 右回転
void right(uint32_t pwm) {
  if (pwm > VALUE_MAX) {
    pwm = VALUE_MAX;
  }
  ledcWrite(CHANNEL_0, 0);
  ledcWrite(CHANNEL_1, pwm);
  ledcWrite(CHANNEL_2, VALUE_MAX);
  ledcWrite(CHANNEL_3, VALUE_MAX);
}

// 左回転
void left(uint32_t pwm) {
  if (pwm > VALUE_MAX) {
    pwm = VALUE_MAX;
  }
  ledcWrite(CHANNEL_0, VALUE_MAX);
  ledcWrite(CHANNEL_1, VALUE_MAX);
  ledcWrite(CHANNEL_2, 0);
  ledcWrite(CHANNEL_3, pwm);
}

// ブレーキ
void brake() {
  ledcWrite(CHANNEL_0, VALUE_MAX);
  ledcWrite(CHANNEL_1, VALUE_MAX);
  ledcWrite(CHANNEL_2, VALUE_MAX);
  ledcWrite(CHANNEL_3, VALUE_MAX);
}

// 空転
void coast() {
  ledcWrite(CHANNEL_0, 0);
  ledcWrite(CHANNEL_1, 0);
  ledcWrite(CHANNEL_2, 0);
  ledcWrite(CHANNEL_3, 0);
}

