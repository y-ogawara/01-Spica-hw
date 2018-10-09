#include "Udp.h"
#include "Motor.h"
#include "Sensor.h"
#include "BlockModel.h"

Motor motor;
Udp udp;
Sensor sensor;

char ssid[] = "";
char password[] = "";
char local_ip[] = "";
char remote_ip[] = "192.168.0.8";

void setup()
{
    Serial.begin(115200);

    Serial.println("setup start!");

    //Wifi設定
    udp.setup_udp(ssid, password, local_ip);

    motor = Motor();
    sensor = Sensor();

    delay(100);

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
    delay(100);

    String commands = udp.get_packet_buffer();
    delay(100);
    if (commands.length() > 0)
    {
        Serial.println("get packet Datas");
        BlockModel models[100];
        block_split(models, commands);
        int i; //modelsのインデックス

        // BlockModel if_models[100];            //ifブロックで囲まれた範囲内のBlockModelが入っている配列
        // BlockModel if_decomposed_models[100]; //if_modelsの中身をif_judge関数で分解した結果が入る配列
        // boolean is_in_if_block = false;       //if対象のブロックかどうか
        // int m = 0;                            //if_models配列の要素数

        BlockModel run_models[100]; //実行対象のBlockModel

        for (i = 0; i < 100; i++)
        {
            bool is_undefined = models[i].get_block_state() <= 0;
            //bool is_standard = models[i].get_block_state() == 1;
            bool is_loop_start = models[i].get_block_state() == 5;
            //bool is_if_start = models[i].get_block_state() == 7;
            //bool is_if_end = models[i].get_block_state() == 8;

            if (is_undefined)
            {
                break;
            }

            else if (is_loop_start)
            {
                //TODO for文分解後の文字列をrun_modelsに追加する
                int for_start_index = i;
                BlockModel for_decomposed_models[100];
                for_judge(for_decomposed_models, models, for_start_index);

                for (int j = 0; j < 100; j++)
                {
                    if (for_decomposed_models[j].get_block_state() <= 0)
                    {
                        break;
                    }

                    run_models[i] = for_decomposed_models[j];
                    i++;
                }
            }

            else
            {
                run_models[i] = models[i];
            }

            // else if (is_if_end)
            // {
            //     Serial.println("ifエンド");

            //     Serial.println(models[i].get_block_state());
            //     Serial.println(models[i].get_left_speed());
            //     Serial.println(models[i].get_right_speed());
            //     Serial.println(models[i].get_time());

            //     if_models[m].set_block_state(models[i].get_block_state());
            //     if_models[m].set_left_speed(models[i].get_left_speed());
            //     if_models[m].set_right_speed(models[i].get_right_speed());
            //     if_models[m].set_time(models[i].get_time());

            //     //TODO if_models をif解釈関数に投げて、run_motorに投げる
            //     if_judge(if_decomposed_models, if_models);
            //     Serial.println("if解釈後------------");
            //     for (int c = 0; c < 100; c++)
            //     {
            //         Serial.println(if_decomposed_models[c].get_block_state());
            //         Serial.println(if_decomposed_models[c].get_left_speed());
            //         Serial.println(if_decomposed_models[c].get_right_speed());
            //         Serial.println(if_decomposed_models[c].get_time());
            //     }
            //     Serial.println("--------------------");

            //     is_in_if_block = false;
            //     m = 0;
            // }

            // else if (is_in_if_block)
            // {
            //     Serial.print("if: state:   ");
            //     Serial.println(models[i].get_block_state());
            //     Serial.print("if: left_speed:   ");
            //     Serial.println(models[i].get_left_speed());
            //     Serial.print("if: right_speed:   ");
            //     Serial.println(models[i].get_right_speed());
            //     Serial.print("if: time:   ");
            //     Serial.println(models[i].get_time());

            //     if_models[m].set_block_state(models[i].get_block_state());
            //     if_models[m].set_left_speed(models[i].get_left_speed());
            //     if_models[m].set_right_speed(models[i].get_right_speed());
            //     if_models[m].set_time(models[i].get_time());
            //     m++;
            // }
        }

        for (int x = 0; x < 100; x++)
        {
            if (run_models[x].get_block_state() <= 0)
            {
                break;
            }

            Serial.print("run_models: state:   ");
            Serial.println(run_models[x].get_block_state());
            Serial.print("run_models: left_speed:   ");
            Serial.println(run_models[x].get_left_speed());
            Serial.print("run_models: right_speed:   ");
            Serial.println(run_models[x].get_right_speed());
            Serial.print("run_models: time:   ");
            Serial.println(run_models[x].get_time());
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
        int block_time = block_texts[i].substring(10, 13).toInt();

        block_models[i].set_block_state(block_state);
        block_models[i].set_left_speed(block_left_speed);
        block_models[i].set_right_speed(block_right_speed);
        block_models[i].set_time(block_time);
    }
}

void if_judge(BlockModel return_blocks[100], BlockModel block_models[100])
{

    BlockModel true_blocks[100];
    int true_count = 0;
    BlockModel false_blocks[100];
    int false_count = 0;
    for (int i = 1; i < 100; i++)
    {
        if (block_models[i].get_block_state() == -1515870811)
        {
            break;
        }
        if (100 < block_models[i].get_block_state() && block_models[i].get_block_state() < 200)
        {
            true_blocks[true_count] = block_models[i];
            true_blocks[true_count] = block_state_change(true_blocks[true_count]);

            true_count++;
        }
        else if (200 < block_models[i].get_block_state() && block_models[i].get_block_state() < 300)
        {
            false_blocks[false_count] = block_models[i];
            false_blocks[false_count] = block_state_change(false_blocks[false_count]);

            false_count++;
        }
    }

    if (block_models[0].get_if_comparison() == 1)
    {
        if (block_models[0].get_if_threshold() < sensor.getDistance())
        {
            for (int i = 0; i < 100; i++)
            {
                return_blocks[i] = true_blocks[i];
            }
            Serial.println("111111111111 ");
            return;
        }
        else
        {
            for (int i = 0; i < 100; i++)
            {
                return_blocks[i] = false_blocks[i];
            }
            Serial.println("22222222222 ");
            return;
        }
    }
    else if (block_models[0].get_if_comparison() == 2)
    {
        if (block_models[0].get_if_threshold() > sensor.getDistance())
        {
            for (int i = 0; i < 100; i++)
            {
                return_blocks[i] = true_blocks[i];
            }
            Serial.println("3333333333333");
            return;
        }
        else
        {
            for (int i = 0; i < 100; i++)
            {
                return_blocks[i] = false_blocks[i];
            }
            Serial.println("444444444444");
            return;
        }
    }
}

BlockModel block_state_change(BlockModel model)
{
    switch (model.get_block_state())
    {
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

/* ループ調整関数 */
void for_judge(BlockModel return_blocks[100], BlockModel models[100], int start_index)
{
    int loop_count = 0;
    BlockModel loop_blocks[100];
    int j = 0; //loop_blocksのインデックス

    for (int i = start_index; i < 100; i++)
    {
        if (models[i].get_block_state() <= 0)
        {
            break;
        }
        else if (models[i].get_block_state() == 5) //ループスタートブロックからループカウント数を取得
        {
            loop_count = models[i].get_loop_count();
        }
        else if (models[i].get_block_state() == 6)
        {
            int m = 0; //return_blocksのインデックス

            //ループ回数分 繰り返す
            for (int count = 0; count < loop_count; count++)
            {
                for (int n = 0; n < j; n++)
                {
                    if (loop_blocks[n].get_block_state() <= 0)
                    {
                        n = 0;
                        break;
                    }

                    return_blocks[m] = loop_blocks[n];
                    m++;
                }
            }
        }
        else //ループ対象ブロックを抽出
        {
            loop_blocks[j] = models[i];
            j++;
        }
    }
}
