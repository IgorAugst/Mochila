#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#define ESPRX 2

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