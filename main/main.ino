#include "Udp.h"
#include "Motor.h"
#include "Sensor.h"
#include "BlockModel.h"

Motor motor;
Udp udp;
Sensor sensor;

char ssid[] = "robop-WiFi-n";
char password[] = "robop0304";
char local_ip[] = "";
char remote_ip[] = "192.168.1.10";

void setup()
{
    Serial.begin(115200);

    Serial.println("setup start!");

    //Wifi設定
    udp.setup_udp(ssid, password, local_ip);

    motor = Motor();
    sensor = Sensor();

    // motor.forward(80,80);
    // delay(1000);
    // motor.back(80,80);
    // delay(1000);

    // motor.left(80);
    // delay(1000);
    // motor.right(80);
    // delay(1000);
    // motor.coast();
    // delay(1000);
    // motor.brake();
    // delay(1000);
}

void loop()
{  
    
    BlockModel block_models[100];
    BlockModel model1 = BlockModel(7, 2, 100, 20);
    BlockModel model2 = BlockModel(101, 100, 100, 2);
    BlockModel model3 = BlockModel(102, 100, 100, 4);
    BlockModel model4 = BlockModel(203, 100, 100, 4);
    BlockModel model5 = BlockModel(204, 100, 100, 3);
    BlockModel model6 = BlockModel(8, 100, 100, 0);

    block_models[0] = model1;
    block_models[1] = model2;
    block_models[2] = model3;
    block_models[3] = model4;
    block_models[4] = model5;
    block_models[5] = model6;

    BlockModel return_blocks[100] = {};
    ifJudge(return_blocks,block_models);
    
    Serial.println("egegegegegegegegegege");
    for (int i = 0; i < 2; i++) {
        Serial.print("default: state:   ");
        Serial.println(return_blocks[i].get_block_state());
        Serial.print("default: left_speed:   ");
        Serial.println(return_blocks[i].get_left_speed());
        Serial.print("default: right_speed:   ");
        Serial.println(return_blocks[i].get_right_speed());
        Serial.print("default: time:   ");
        Serial.println(return_blocks[i].get_time());
    }
     delay(10000);
    
}

// 参照渡しで実装してあります
void block_split(BlockModel block_models[100], String text)
{
    String block_texts[100];
    String tmp_text = text;
    for (int i = 0; i < text.length() / 13; i++)
    {
        block_texts[i] = tmp_text.substring(0, 13);
        tmp_text = tmp_text.substring(13, tmp_text.length());
    }

    for (int i = 0; i < 100; i++)
    {
        if (block_texts[i].length() == 0)
        {
            block_models[i].set_block_state(0);
            block_models[i].set_left_speed(0);
            block_models[i].set_right_speed(0);
            block_models[i].set_time(0);
            break;
        }
        int block_state = block_texts[i].substring(0, 4).toInt();
        int block_left_speed = block_texts[i].substring(4, 7).toInt();
        int block_right_speed = block_texts[i].substring(7, 10).toInt();
        int block_time = block_texts[i].substring(10, 13).toInt() * 1000;

        block_models[i].set_block_state(block_state);
        block_models[i].set_left_speed(block_left_speed);
        block_models[i].set_right_speed(block_right_speed);
        block_models[i].set_time(block_time);
    }
}

void ifJudge(BlockModel return_blocks[100],BlockModel block_models[100]) {

    BlockModel true_blocks[100];
    int true_count = 0;
    BlockModel false_blocks[100];
    int false_count = 0;
    for (int i = 1; i < 100; i ++) {
        if (block_models[i].get_block_state() == -1515870811) {
            break;
        }
        if (100 < block_models[i].get_block_state() && block_models[i].get_block_state() < 200 ) {
            true_blocks[true_count] = block_models[i];
            true_blocks[true_count] = blockStateChange(true_blocks[true_count]);

            true_count++;
        } else if (200 < block_models[i].get_block_state() && block_models[i].get_block_state() < 300) {
            false_blocks[false_count] = block_models[i];
            false_blocks[false_count] = blockStateChange(false_blocks[false_count]);

            false_count++;
        }
    }

    if (block_models[0].get_if_comparison() == 1) {
        if (block_models[0].get_if_threshold() < sensor.getDistance()) {
            for (int i = 0; i < 100; i++) {
                return_blocks[i] = true_blocks[i];
            }
            Serial.println("111111111111 ");
            return;
        } else {
            for (int i = 0; i < 100; i++) {
                return_blocks[i] = false_blocks[i];
            }
            Serial.println("22222222222 ");
            return;
        }

    }else if (block_models[0].get_if_comparison() == 2) {
        if (block_models[0].get_if_threshold() > sensor.getDistance()) {
             for (int i = 0; i < 100; i++) {
                return_blocks[i] = true_blocks[i];
            }
            Serial.println("3333333333333");
            return;
        } else {
            for (int i = 0; i < 100; i++) {
                return_blocks[i] = false_blocks[i];
            }
            Serial.println("444444444444");
            return;
        }
    }
}

BlockModel blockStateChange(BlockModel model) {
    switch(model.get_block_state()){
         case 101:
         case 201:
            model.set_block_state(1);
            break;

        case 102:
        case 202:
            model.set_block_state(2);
            break;

        case 103:
        case 203:
            model.set_block_state(3);
            break;

        case 104:
        case 204:
            model.set_block_state(4);
            break;

        case 105:
        case 205:
            model.set_block_state(5);
            break;

        case 106:
        case 206:
            model.set_block_state(6);
            break;

        case 109:
        case 209:
            model.set_block_state(9);
            break;
        
     }
     return model;
}
