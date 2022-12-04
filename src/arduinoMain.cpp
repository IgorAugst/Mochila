#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <LiquidCrystal.h>

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define ESPRX 11

SoftwareSerial esp(ESPRX, -1);

void setup()
{
    Serial.begin(9600);
    esp.begin(115200);
}

String readSerial()
{
    String data = "";
    while (esp.available() > 0)
    {
        char c = esp.read();

        if(c == '\n'){
            break;
        }

        data += c;
    }
    return data;
}

void loop()
{

    String data = readSerial();

    if (data.length() > 0)
    {
        StaticJsonDocument<200> doc;
        deserializeJson(doc, data);

        String l1 = doc["l1"];
        String l2 = doc["l2"];

        Serial.println(l1);
        Serial.println(l2);


    }

}