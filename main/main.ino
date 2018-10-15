#include "Udp.h"
#include "Motor.h"
#include "Sensor.h"
#include "BlockModel.h"

Motor motor;
Udp udp;
Sensor sensor;

char ssid[] = "robop-WiFi-n";
char password[] = "robop0304";
char local_ip[] = "192.168.1.170";
char remote_ip[] = "192.168.1.10";

void rebootTask(void *pvParameters)
{
    while (1)
    {
        udp.recieve_packet();
        String packet = udp.get_packet_buffer();
        if (packet.startsWith("reboot-now"))
        {
            ESP.restart();
        }
        udp.clear_packet_buffer();
        delay(1000);
    }
}

void setup()
{
    Serial.begin(115200);

    Serial.println("setup start!");
    //xTaskCreate(rebootTask, "rebootTask", 1024, NULL, 1, NULL);

    //Wifi設定
    //udp.setup_udp(ssid, password, local_ip);

    motor = Motor();
    sensor = Sensor();
}

void loop()
{
    //udp.recieve_packet();
    String commands = "000310010000100050000000020001100100001000210010000100060000000000004100100001"; // udp.get_packet_buffer()

    if (commands.length() > 0)
    {
        Serial.println("get packet Datas");
        BlockModel block_models[100];
        block_split(block_models, commands);

        BlockModel for_decomposed_models[100];
        forCheck(for_decomposed_models, block_models);

        //debug
        for (int i = 0; i < sizeof(for_decomposed_models); i++)
        {
            if (for_decomposed_models[i].get_block_state() <= 0)
            {
                break;
            }

            Serial.print("for_decomposed_models: state:   ");
            Serial.println(for_decomposed_models[i].get_block_state());
            Serial.print("for_decomposed_models: left_speed:   ");
            Serial.println(for_decomposed_models[i].get_left_speed());
            Serial.print("for_decomposed_models: right_speed:   ");
            Serial.println(for_decomposed_models[i].get_right_speed());
            Serial.print("for_decomposed_models: time:   ");
            Serial.println(for_decomposed_models[i].get_time());
        }

        //データ削除
        udp.clear_packet_buffer();

        //完了通知
        Serial.println("Done!");
    }
}

//udp通信で送られてきた文字列をsplitして、参照渡しでblock_modelsに返す
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

//全体のblock_modelsから、forスタートブロックとforエンドブロックの間のループ対象ブロック抜き出して分解する
void forCheck(BlockModel return_blocks[100], BlockModel block_models[100])
{
    int model_size = 100;
    BlockModel range_for_blocks[model_size];
    bool is_loop_now = false;
    int loop_count = 0;
    int j = 0; //range_for_blocks のインデックス
    int k = 0; //return_blocks のインデックス

    for (int i = 0; i < model_size; i++)
    {
        bool is_undefiend_state = block_models[i].get_block_state() <= 0;
        bool is_loop_start = block_models[i].get_block_state() == 5;
        bool is_loop_end = block_models[i].get_block_state() == 6;

        if (is_undefiend_state)
        {
            break;
        }
        else if (is_loop_start)
        {
            is_loop_now = true;
            loop_count = block_models[i].get_loop_count();
        }
        else if (is_loop_end)
        {
            is_loop_now = false;
            BlockModel for_decomposed_models[model_size];

            //TODO forJudgeにわたす
            forJudge(for_decomposed_models, range_for_blocks, loop_count);

            for (int l = 0; l < model_size; l++)
            {
                is_undefiend_state = for_decomposed_models[l].get_block_state() <= 0;

                if (is_undefiend_state)
                {
                    break;
                }

                // Serial.print("for block: state:   ");
                // Serial.println(for_decomposed_models[l].get_block_state());
                // Serial.print("for block: left_speed:   ");
                // Serial.println(for_decomposed_models[l].get_left_speed());
                // Serial.print("for block: right_speed:   ");
                // Serial.println(for_decomposed_models[l].get_right_speed());
                // Serial.print("for block: time:   ");
                // Serial.println(for_decomposed_models[l].get_time());

                return_blocks[k] = for_decomposed_models[l];
                k++;
            }

            //ループ関係の変数値初期化
            loop_count = 0;
            j = 0;
            memset(range_for_blocks, '\0', model_size);
        }
        else if (is_loop_now)
        {
            Serial.println("find loop_now");
            range_for_blocks[j] = block_models[i];
            j++;
        }
        else
        {
            return_blocks[k] = block_models[i];
            k++;
        }
    }
}

//forスタートとforエンドブロックの間のループ対象block_modelsを投げて、ループ回数分つなげたblock_modelsを返す
void forJudge(BlockModel return_blocks[100], BlockModel block_models[100], int loop_count)
{
    int model_size = 100;
    int i = 0; //return_blocks のインデックス

    for (int count = 0; count < loop_count; count++)
    {
        for (int j = 0; j < model_size; j++)
        {
            bool is_undefiend_state = block_models[j].get_block_state() <= 0;

            if (is_undefiend_state)
            {
                break;
            }

            return_blocks[i] = block_models[j];
            i++;
        }
    }
}

//ifスタートブロックからifエンドブロックまでのblock_modelsを投げて、センサー値の結果に基づいて適切なblock_modelsを返す
void ifJudge(BlockModel return_blocks[100], BlockModel block_models[100])
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
            true_blocks[true_count] = blockStateChange(true_blocks[true_count]);

            true_count++;
        }
        else if (200 < block_models[i].get_block_state() && block_models[i].get_block_state() < 300)
        {
            false_blocks[false_count] = block_models[i];
            false_blocks[false_count] = blockStateChange(false_blocks[false_count]);

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

BlockModel blockStateChange(BlockModel model)
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
