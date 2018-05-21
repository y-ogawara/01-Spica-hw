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
const char SSID[] = "WiFi の SSID";
const char PASSWORD[] = "WiFi のパスワード";

//WiFiServer server(80);

void setup() {
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



//  WiFi.begin(SSID, PASSWORD);
//  Serial.print("WiFi connecting");
//  while (WiFi.status() != WL_CONNECTED) {
//    Serial.print(".");
//    delay(100);
//  }
//  Serial.println(" connected");
//  Serial.print("HTTP Server: http://");
//  Serial.print(WiFi.localIP());
//  Serial.println("/");
}

void loop() {
  right(100);
  delay(1000);
  coast();
  delay(1000);
  
  left(100);
  delay(1000);
  coast();
  delay(1000);
  
  forward(100);
  delay(1000);
  coast();
  delay(1000);
  
  reverse(100);
  delay(1000);
  coast();
  delay(1000);
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

