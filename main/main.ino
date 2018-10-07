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
}

void loop()
{
    udp.recieve_packet();

    String commands = udp.get_packet_buffer();
    if (commands.length() > 0)
    {
        Serial.println("get packet Datas");
        BlockModel models[100];
        block_split(models, commands);

        for (int i = 0; i < commands.length() / 13; i++)
        {
            Serial.println(models[i].get_block_state());
            Serial.println(models[i].get_left_speed());
            Serial.println(models[i].get_right_speed());
            Serial.println(models[i].get_time());

            motor.run_motor(models[i].get_block_state(), models[i].get_left_speed(), models[i].get_right_speed(), models[i].get_time());
            
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
