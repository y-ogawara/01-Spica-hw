/* 使うピンの定義 */
const int IN1 = 25;
const int IN2 = 26;

/* チャンネルの定義 */
const int CHANNEL_0 = 0;
const int CHANNEL_1 = 1;

const int LEDC_TIMER_BIT = 8;   // PWMの範囲(8bitなら0〜255、10bitなら0〜1023)
const int LEDC_BASE_FREQ = 490; // 周波数(Hz)
const int VALUE_MAX = 255;      // PWMの最大値

void setup() {
  pinMode(IN1, OUTPUT); // IN1
  pinMode(IN2, OUTPUT); // IN2

  // ピンのセットアップ
  ledcSetup(CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcSetup(CHANNEL_1, LEDC_BASE_FREQ, LEDC_TIMER_BIT);

  // ピンのチャンネルをセット
  ledcAttachPin(IN1, CHANNEL_0);
  ledcAttachPin(IN2, CHANNEL_1);

  Serial.begin(38400);
}

void loop() {
  forward(100); // 正転
  delay(3000);
  coast();      // 空転
  delay(3000);
  reverse(100); // 逆転
  delay(3000);
  coast();      // 空転
  delay(3000);
  Serial.print("Hello");
}

// 正転
void forward(uint32_t pwm) {
  if (pwm > VALUE_MAX) {
    pwm = VALUE_MAX;
  }
  ledcWrite(CHANNEL_0, pwm);
  ledcWrite(CHANNEL_1, 0);
}

// 逆転
void reverse(uint32_t pwm) {
  if (pwm > VALUE_MAX) {
    pwm = VALUE_MAX;
  }
  ledcWrite(CHANNEL_0, 0);
  ledcWrite(CHANNEL_1, pwm);
}

// ブレーキ
void brake() {
  ledcWrite(CHANNEL_0, VALUE_MAX);
  ledcWrite(CHANNEL_1, VALUE_MAX);
}

// 空転
void coast() {
  ledcWrite(CHANNEL_0, 0);
  ledcWrite(CHANNEL_1, 0);
}

