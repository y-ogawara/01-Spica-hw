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

void reboot_task(void *pvParameters)
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
    xTaskCreate(reboot_task, "reboot_task", 1024, NULL, 1, NULL);

    //Wifi設定
    udp.setup_udp(ssid, password, local_ip);

    motor = Motor();
    sensor = Sensor();
}

void loop()
{
    udp.recieve_packet();
    String commands = udp.get_packet_buffer(); // コマンド文字列受け取り

    if (commands.length() > 0)
    {
        Serial.println("get packet Datas");
        int model_size = 50;  //BlockModelの配列のサイズ

        BlockModel block_models[model_size] = {};  //block_splitした後のBlockModelが入る
        block_split(block_models, commands);  //受信したコマンドを命令ごとに整形

        BlockModel for_decomposed_models[model_size] = {};  //for文を全て外した後のBlockModelが入る
        for_check(for_decomposed_models, block_models);  //for文解釈

        run_models(for_decomposed_models);

        //データ削除
        udp.clear_packet_buffer();
        memset(block_models, '\0', model_size);
        memset(for_decomposed_models, '\0', model_size);

        //完了通知
        Serial.println("Done!");
        char sendText[] = "Done!\r";
        udp.send_data(remote_ip, sendText);
    }
}

//udp通信で送られてきた文字列をsplitして、参照渡しでblock_modelsに返す
void block_split(BlockModel block_models[50], String text)
{
    int model_size = 50;
    String block_texts[model_size] = {};
    String tmp_text = text;
    for (int i = 0; i < text.length() / 13; i++)
    {
        block_texts[i] = tmp_text.substring(0, 13);
        tmp_text = tmp_text.substring(13, tmp_text.length());
    }

    for (int i = 0; i < model_size; i++)
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
void for_check(BlockModel return_blocks[50], BlockModel block_models[50])
{
    int model_size = 50;
    BlockModel range_for_blocks[model_size] = {};
    bool is_loop_now = false;
    int loop_count = 0;
    int j = 0; //range_for_blocks のインデックス
    int k = 0; //return_blocks のインデックス

    for (int i = 0; i < model_size; i++)
    {
        bool is_undefined_state = is_incorrect_state(block_models[i].get_block_state());
        bool is_loop_start = (block_models[i].get_block_state() == 5 || block_models[i].get_block_state() == 105 || block_models[i].get_block_state() == 205);
        bool is_loop_end = (block_models[i].get_block_state() == 6 || block_models[i].get_block_state() == 106 || block_models[i].get_block_state() == 206);

        if (is_undefined_state)
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
            BlockModel for_decomposed_models[model_size] = {};

            for_judge(for_decomposed_models, range_for_blocks, loop_count);

            for (int l = 0; l < model_size; l++)
            {
                bool is_undefined_state = is_incorrect_state(for_decomposed_models[l].get_block_state());

                if (is_undefined_state)
                {
                    break;
                }

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
void for_judge(BlockModel return_blocks[50], BlockModel block_models[50], int loop_count)
{
    int model_size = 50;
    int i = 0; //return_blocks のインデックス

    for (int count = 0; count < loop_count; count++)
    {
        for (int j = 0; j < model_size; j++)
        {
            bool is_undefined_state = is_incorrect_state(block_models[j].get_block_state());

            if (is_undefined_state)
            {
                break;
            }

            return_blocks[i] = block_models[j];
            i++;
        }
    }
}

// block_modelsの中身のコマンドを順次モーター駆動処理しながら、途中にifブロックがあれば随時解釈処理を実行してモーター駆動処理を行う
void run_models(BlockModel block_models[50])
{
    int model_size = 50;
    BlockModel range_if_blocks[model_size] = {};
    int j = 0; // range_if_blocks のインデックス
    bool is_if_now = false;

    for (int i = 0; i < model_size; i++)
    {
        bool is_undefined_state = is_incorrect_state(block_models[i].get_block_state());
        bool is_if_start = block_models[i].get_block_state() == 7;
        bool is_if_end = block_models[i].get_block_state() == 8;

        if (is_undefined_state)
        {
            break;
        }
        else if (is_if_start)
        {
            is_if_now = true;
            range_if_blocks[j] = block_models[i];
            j++;
        }
        else if (is_if_end) //全体のblock_modelsから、ifスタートブロックからifエンドブロックのブロックを抜き出して分解する
        {
            is_if_now = false;
            range_if_blocks[j] = block_models[i];
            BlockModel if_decomposed_models[model_size] = {};

            if_judge(if_decomposed_models, range_if_blocks);

            for (int k = 0; k < model_size; k++)
            {
                bool is_undefined_state = is_incorrect_state(if_decomposed_models[k].get_block_state());

                if (is_undefined_state)
                {
                    break;
                }

                motor.run_motor(if_decomposed_models[k]);
            }

            //if関係の変数値初期化
            j = 0;
            memset(range_if_blocks, '\0', model_size);
            memset(if_decomposed_models, '\0', model_size);
        }
        else if (is_if_now)
        {
            range_if_blocks[j] = block_models[i];
            j++;
        }
        else
        {
            motor.run_motor(block_models[i]);
        }
    }
}

//ifスタートブロックからifエンドブロックまでのblock_modelsを投げて、センサー値の結果に基づいて適切なblock_modelsを返す
void if_judge(BlockModel return_blocks[50], BlockModel block_models[50])
{
    int model_size = 50;
    BlockModel true_blocks[model_size] = {};
    int true_count = 0;
    BlockModel false_blocks[model_size] = {};
    int false_count = 0;
    for (int i = 1; i < model_size; i++)
    {
        bool is_undefined_state = is_incorrect_state(block_models[i].get_block_state());
        bool is_if_end = block_models[i].get_block_state() == 8;
        bool is_true_models = (100 < block_models[i].get_block_state() && block_models[i].get_block_state() < 200);
        bool is_false_models = (200 < block_models[i].get_block_state() && block_models[i].get_block_state() < 300);

        if (is_undefined_state || is_if_end)
        {
            break;
        }
        if (is_true_models)
        {
            true_blocks[true_count] = block_models[i];
            true_blocks[true_count] = block_state_change(true_blocks[true_count]);

            true_count++;
        }
        else if (is_false_models)
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
            for (int j = 0; j <= true_count; j++)
            {
                bool is_undefined_state = is_incorrect_state(true_blocks[j].get_block_state());
                if (is_undefined_state)
                {
                    break;
                }

                return_blocks[j] = true_blocks[j];
            }
            return;
        }
        else
        {
            for (int j = 0; j <= false_count; j++)
            {
                bool is_undefined_state = is_incorrect_state(false_blocks[j].get_block_state());
                if (is_undefined_state)
                {
                    break;
                }

                return_blocks[j] = false_blocks[j];
            }
            return;
        }
    }
    else if (block_models[0].get_if_comparison() == 2)
    {
        if (block_models[0].get_if_threshold() > sensor.getDistance())
        {
            for (int j = 0; j <= true_count; j++)
            {
                bool is_undefined_state = is_incorrect_state(true_blocks[j].get_block_state());
                if (is_undefined_state)
                {
                    break;
                }

                return_blocks[j] = true_blocks[j];
            }
            return;
        }
        else
        {
            for (int j = 0; j <= false_count; j++)
            {
                bool is_undefined_state = is_incorrect_state(false_blocks[j].get_block_state());
                if (is_undefined_state)
                {
                    break;
                }

                return_blocks[j] = false_blocks[j];
            }
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

bool is_incorrect_state(int state_num)
{
    bool is_undefined = false;

    switch (state_num)
    {
    case 1:
    case 101:
    case 201:
        //Serial.println("前進処理のブロック");
        break;

    case 2:
    case 102:
    case 202:
        //Serial.println("後退処理のブロック");
        break;

    case 3:
    case 103:
    case 203:
        //Serial.println("左回転処理のブロック");
        break;

    case 4:
    case 104:
    case 204:
        //Serial.println("右回転処理のブロック");
        break;

    case 5:
    case 105:
    case 205:
        //Serial.println("forスタートのブロック");
        break;

    case 6:
    case 106:
    case 206:
        //Serial.println("forエンドのブロック");
        break;

    case 7:
        //Serial.println("ifスタートのブロック");
        break;

    case 8:
        //Serial.println("ifエンドのブロック");
        break;

    case 9:
    case 109:
    case 209:
        //Serial.println("breakのブロック");
        break;

    default:
        //Serial.println("コマンド終了");
        is_undefined = true;
        break;
    }

    return is_undefined;
}
