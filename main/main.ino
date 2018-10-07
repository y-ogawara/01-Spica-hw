#include "Udp.h"
#include "Motor.h"
#include "Sensor.h"
#include "BlockModel.h"

Motor motor;
Udp udp;
Sensor sensor;

void setup()
{
    Serial.begin(115200);

    Serial.println("setup start!");

    //Wifi設定
    char ssid[] = "robop-WiFi-n";
    char password[] = "robop0304";
    char ip[] = "";
    udp.setup_udp(ssid, password, ip);

    motor = Motor();
    sensor = Sensor();
}

void loop()
{  
    float distance = sensor.getDistance();
    Serial.print("距離:    ");
    Serial.println(distance);
    udp.recieve_packet();

    String commands = udp.get_packet_buffer();
    if(commands.length() > 0)
    {
      Serial.println("get packet Datas");
      BlockModel models[100];
      block_split(models, commands); 

      //データ削除
      udp.clear_packet_buffer();
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
            Serial.println("aaa-brak");
            break;
        }
        int block_state = block_texts[i].substring(0, 4).toInt();
        int block_left_speed = block_texts[i].substring(4, 7).toInt();
        int block_right_speed = block_texts[i].substring(7, 10).toInt();
        int block_time = block_texts[i].substring(10, 13).toInt();

        Serial.println(block_state);
        Serial.println(block_left_speed);
        Serial.println(block_right_speed);
        Serial.println(block_time);
        
        block_models[i].set_block_state(block_state);
        block_models[i].set_left_speed(block_left_speed);
        block_models[i].set_right_speed(block_right_speed);
        block_models[i].set_time(block_time);
    }
}
