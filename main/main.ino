#include "Udp.h"
#include "Motor.h"
#include "Sensor.h"
#include "BlockModel.h"

Motor motor;
char ssid[] = "aaa";
char password[] = "";
Sensor sensor;

void setup()
{
    Serial.begin(115200);

    Serial.println("setup start!");

    //参照渡しで実装済み
    BlockModel models[100];
    block_split(models,"000110010002100030500500110001100100021");
    motor = Motor();
    sensor = Sensor();
}

void loop()
{
    //udp.recieve_packet();
}

// 参照渡しで実装してあります
void block_split(BlockModel block_models[100],String text) {
    String block_texts[100];
    String tmp_text = text;
    for(int i = 0; i < text.length()/13; i++) {
        block_texts[i] = tmp_text.substring(0,13);
        tmp_text = tmp_text.substring(13,tmp_text.length());
    }

    for(int i = 0; i < 100; i++) {
        if (block_texts[i].length() == 0) {
            Serial.println("aaa-brak");
            break;
        }
        int block_state = block_texts[i].substring(0,4).toInt();
        int block_left_speed = block_texts[i].substring(4,7).toInt();
        int block_right_speed = block_texts[i].substring(7,10).toInt();
        int block_time = block_texts[i].substring(10,3).toInt();
        block_models[i].set_block_state(block_state);
        block_models[i].set_left_speed(block_left_speed);
        block_models[i].set_right_speed(block_right_speed);
        block_models[i].set_time(block_time);
    }
}
