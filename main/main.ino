#include <WiFi.h>
#include <WiFiUdp.h>

#define UDP_TX_PACKET_MAX_SIZE 500

// 使うピンの定義
const int IN1 = 25;
const int IN2 = 26;
const int IN3 = 16;
const int IN4 = 17;

// チャンネルの定義
const int CHANNEL_0 = 0;
const int CHANNEL_1 = 1;
const int CHANNEL_2 = 2;
const int CHANNEL_3 = 3;

const int LEDC_TIMER_BIT = 8;   // PWMの範囲(8bitなら0〜255、10bitなら0〜1023)
const int LEDC_BASE_FREQ = 490; // 周波数(Hz)
const int VALUE_MAX = 255;      // PWMの最大値

// wifiの設定
const char SSID[] = "ERS-AP";
const char PASSWORD[] = "1234567890";
const int localPort = 10000; // ポート番号

IPAddress local_IP(192, 168, 0, 76);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiUDP udp;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
String cmd_plus_split[UDP_TX_PACKET_MAX_SIZE] = {"\0"};
String cmd_and_split[UDP_TX_PACKET_MAX_SIZE] = {"\0"};

int split(String data, char delimiter, String *dst){
    int index = 0;
    int arraySize = (sizeof(data)/sizeof((data)[0]));  
    int datalength = data.length();
    for (int i = 0; i < datalength; i++) {
        char tmp = data.charAt(i);
        if ( tmp == delimiter ) {
            index++;
            if ( index > (arraySize - 1)) return -1;
        }
        else dst[index] += tmp;
    }
    return (index + 1);
}
 

void setup()
{
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

    delay(100);

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
    udp.begin(localPort); // UDP通信の開始(引数はポート番号)
    Serial.print("Local port: ");
    Serial.println(localPort);
}

void loop(){
  int packetSize = udp.parsePacket();
  if(packetSize)
  {
    //UDP情報の表示
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = udp.remoteIP();
    for (int i =0; i < 4; i++)
    {
      Serial.print(remote[i], DEC);
      if (i < 3)
      {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(udp.remotePort());

    // 実際にパケットを読む
    udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    
    split(packetBuffer,'+',cmd_plus_split);

    int i = 0;
    while(true) {
      if (cmd_plus_split[i] == "\0") {
        break;
      }
      split(cmd_plus_split[i],'&',cmd_and_split);

      /* 配列の中身
       *  0 = 空
       *  1 = 動きの内容
       *  2 = 左パワー
       *  3 = 右パワー
       *  4 = 時間
       */
      for (int j = 0; j < 5; j++) {
        
        Serial.println(cmd_and_split[j]);
      }
      String command = cmd_and_split[1];
      int left_power = cmd_and_split[2].toInt();
      int right_power = cmd_and_split[3].toInt();
      int time = cmd_and_split[4].toInt();
      
      guruguru_motor(command,left_power,right_power,time);
        
      //配列データ削除
      memset( cmd_and_split,'\0',UDP_TX_PACKET_MAX_SIZE);  
      i++;
    }


    // データ削除
    int packetBuffer_length = strlen(packetBuffer);
    memset( packetBuffer,'\0',packetBuffer_length);
    memset( cmd_plus_split,'\0',UDP_TX_PACKET_MAX_SIZE);
    memset( cmd_and_split,'\0',UDP_TX_PACKET_MAX_SIZE);
  }
}

// 命名 中里見
void guruguru_motor(String command, uint32_t left_pwm, uint32_t right_pwm, int time) {
  
    if (command.equals("forward")) {
      forward(left_pwm,right_pwm);
    }
    else if (command.equals("left")) {
      left(left_pwm);
    }
    else if (command.equals("right")) {
      right(right_pwm);
    }
    else if (command.equals("back")) {
      back(left_pwm,right_pwm);
    }
  delay(time);
  coast();
  
}



void forward(uint32_t left_pwm,uint32_t right_pwm)
{
    if (left_pwm > VALUE_MAX)
    {
        left_pwm = VALUE_MAX;
    }
    if (right_pwm > VALUE_MAX)
    {
        right_pwm = VALUE_MAX;
    }
    ledcWrite(CHANNEL_0, 0);
    ledcWrite(CHANNEL_1, left_pwm);
    ledcWrite(CHANNEL_2, 0);
    ledcWrite(CHANNEL_3, right_pwm);
}

void back(uint32_t left_pwm,uint32_t right_pwm)
{
    if (left_pwm > VALUE_MAX)
    {
        left_pwm = VALUE_MAX;
    }
    if (right_pwm > VALUE_MAX)
    {
        right_pwm = VALUE_MAX;
    }
    ledcWrite(CHANNEL_0, left_pwm);
    ledcWrite(CHANNEL_1, 0);
    ledcWrite(CHANNEL_2, right_pwm);
    ledcWrite(CHANNEL_3, 0);
}

// 右回転
void right(uint32_t pwm)
{
    if (pwm > VALUE_MAX)
    {
        pwm = VALUE_MAX;
    }
    ledcWrite(CHANNEL_0, 0);
    ledcWrite(CHANNEL_1, pwm);
    ledcWrite(CHANNEL_2, VALUE_MAX);
    ledcWrite(CHANNEL_3, VALUE_MAX);
}

// 左回転
void left(uint32_t pwm)
{
    if (pwm > VALUE_MAX)
    {
        pwm = VALUE_MAX;
    }
    ledcWrite(CHANNEL_0, VALUE_MAX);
    ledcWrite(CHANNEL_1, VALUE_MAX);
    ledcWrite(CHANNEL_2, 0);
    ledcWrite(CHANNEL_3, pwm);
}

// ブレーキ
void brake()
{
    ledcWrite(CHANNEL_0, VALUE_MAX);
    ledcWrite(CHANNEL_1, VALUE_MAX);
    ledcWrite(CHANNEL_2, VALUE_MAX);
    ledcWrite(CHANNEL_3, VALUE_MAX);
}

// 空転
void coast()
{
    ledcWrite(CHANNEL_0, 0);
    ledcWrite(CHANNEL_1, 0);
    ledcWrite(CHANNEL_2, 0);
    ledcWrite(CHANNEL_3, 0);
}

