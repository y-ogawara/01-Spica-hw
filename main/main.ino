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
    // float distance = sensor.getDistance();
    // Serial.print("距離:    ");
    // Serial.println(distance);
    udp.recieve_packet();

    String commands = udp.get_packet_buffer();
    if (commands.length() > 0)
    {
        Serial.println("get packet Datas");
        BlockModel models[100];
        block_split(models, commands);

        BlockModel loop_models[100];      //ループ対象のBlockModelが入っている配列
        boolean is_in_loop_block = false; //ループ対象のブロックかどうか
        int loop_count = 0;               //ループ回数
        int j = 0;                        //loop_models配列の要素数

        BlockModel if_models[100];      //ifブロックで囲まれた範囲内のBlockModelが入っている配列
        boolean is_in_if_block = false; //if対象のブロックかどうか
        int m = 0;                      //if_models配列の要素数

        for (int i = 0; i < 100; i++)
        {
            if (models[i].get_block_state() == 0)
            {
                break;
            }
            bool is_loop_start = models[i].get_block_state() == 5;
            if (is_loop_start) //ループスタートのブロックのとき、ループ回数を取得し、ループエンドブロックが出るまで以降のBlockModelをループ対象のブロックとする
            {
                Serial.println("ループスタート");
                loop_count = models[i].get_time(); //ループ回数
                Serial.println(loop_count);
                is_in_loop_block = true;
            }
            else if (models[i].get_block_state() == 7)
            {
                Serial.println("ifスタート");
                is_in_if_block = true;

                Serial.println(models[i].get_block_state());
                Serial.println(models[i].get_left_speed());
                Serial.println(models[i].get_right_speed());
                Serial.println(models[i].get_time());

                if_models[m].set_block_state(models[i].get_block_state());
                if_models[m].set_left_speed(models[i].get_left_speed());
                if_models[m].set_right_speed(models[i].get_right_speed());
                if_models[m].set_time(models[i].get_time());
                m++;
            }

            else if (models[i].get_block_state() == 8)
            {
                Serial.println("ifエンド");

                Serial.println(models[i].get_block_state());
                Serial.println(models[i].get_left_speed());
                Serial.println(models[i].get_right_speed());
                Serial.println(models[i].get_time());

                if_models[m].set_block_state(models[i].get_block_state());
                if_models[m].set_left_speed(models[i].get_left_speed());
                if_models[m].set_right_speed(models[i].get_right_speed());
                if_models[m].set_time(models[i].get_time());

                //TODO if_models をif解釈関数に投げて、run_motorに投げる

                is_in_if_block = false;
                m = 0;
            }

            else if (models[i].get_block_state() == 6) //ループエンドのブロックのとき、ループ対象のブロックをループ回数分処理
            {
                Serial.println("ループエンド");

                for (int i = 0; i < loop_count; i++)
                {
                    for (int m = 0; m < j; m++) //ループ対象のブロックを処理
                    {
                        Serial.print("loop: state:   ");
                        Serial.println(loop_models[m].get_block_state());
                        Serial.print("loop: left_speed:   ");
                        Serial.println(loop_models[m].get_left_speed());
                        Serial.print("loop: right_speed:   ");
                        Serial.println(loop_models[m].get_right_speed());
                        Serial.print("loop: time:   ");
                        Serial.println(loop_models[m].get_time());

                        motor.run_motor(loop_models[m].get_block_state(), loop_models[m].get_left_speed(), loop_models[m].get_right_speed(), loop_models[m].get_time());
                    }
                }

                is_in_loop_block = false;
                loop_count = 0;
                j = 0;
            }
            else if (is_in_if_block)
            {
                Serial.print("if: state:   ");
                Serial.println(models[i].get_block_state());
                Serial.print("if: left_speed:   ");
                Serial.println(models[i].get_left_speed());
                Serial.print("if: right_speed:   ");
                Serial.println(models[i].get_right_speed());
                Serial.print("if: time:   ");
                Serial.println(models[i].get_time());

                if_models[m].set_block_state(models[i].get_block_state());
                if_models[m].set_left_speed(models[i].get_left_speed());
                if_models[m].set_right_speed(models[i].get_right_speed());
                if_models[m].set_time(models[i].get_time());
                m++;
            }

            else if (is_in_loop_block) //ループ対象のブロックをloop_models配列に入れる
            {
                Serial.print("loop: state:   ");
                Serial.println(models[i].get_block_state());
                Serial.print("loop: left_speed:   ");
                Serial.println(models[i].get_left_speed());
                Serial.print("loop: right_speed:   ");
                Serial.println(models[i].get_right_speed());
                Serial.print("loop: time:   ");
                Serial.println(models[i].get_time());

                loop_models[j].set_block_state(models[i].get_block_state());
                loop_models[j].set_left_speed(models[i].get_left_speed());
                loop_models[j].set_right_speed(models[i].get_right_speed());
                loop_models[j].set_time(models[i].get_time());
                j++;
            }
            else //ループ対象でないブロックの処理
            {
                Serial.print("default: state:   ");
                Serial.println(models[i].get_block_state());
                Serial.print("default: left_speed:   ");
                Serial.println(models[i].get_left_speed());
                Serial.print("default: right_speed:   ");
                Serial.println(models[i].get_right_speed());
                Serial.print("default: time:   ");
                Serial.println(models[i].get_time());

                motor.run_motor(models[i].get_block_state(), models[i].get_left_speed(), models[i].get_right_speed(), models[i].get_time());
            }
        }

        //データ削除
        udp.clear_packet_buffer();

        //完了通知
        char sendText[] = "Done!\r";
        udp.send_data(remote_ip, sendText);
    }
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
