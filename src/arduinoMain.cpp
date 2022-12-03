#include <Arduino.h>
#include <SoftwareSerial.h>

#define ESPRX 2

SoftwareSerial esp(ESPRX, -1);

void setup()
{
    Serial.begin(9600);
    esp.begin(9600);
}

String readSerial()
{
    String data = "";
    while (esp.available() > 0)
    {
        char c = esp.read();
        data += c;
    }
    return data;
}

void loop()
{

    String data = readSerial();

    if (data.length() > 0)
    {
        Serial.print(data);
    }

}