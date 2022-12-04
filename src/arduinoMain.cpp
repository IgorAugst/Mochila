#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <LiquidCrystal.h>

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define ESPRX 11
#define BRIGHTPIN 10

int brightness = 5;
bool isValid = false;

SoftwareSerial esp(ESPRX, -1);

void setup()
{
    lcd.begin(16, 2);
    Serial.begin(9600);
    esp.begin(9600);
    pinMode(BRIGHTPIN, OUTPUT);
    analogWrite(BRIGHTPIN, brightness);
}

String dataFull = "";

String readSerial()
{
    while (esp.available() > 0)
    {
        char c = esp.read();
        if(c == '\n'){
            isValid = true;
            return dataFull;
        }

        dataFull += c;
    }
    return "";
}

void loop()
{

    String data = readSerial();

    if (data.length() > 0 && isValid)
    {
        StaticJsonDocument<200> doc;
        deserializeJson(doc, data);

        String l1 = doc["l1"];
        String l2 = doc["l2"];

        lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print(l1);

        lcd.setCursor(0, 1);
        lcd.print(l2);

        Serial.print(l1);
        Serial.print(l2);

        isValid = false;
        dataFull = "";
    }

    int value = analogRead(A0);

    if(value > 80 && value < 200){
        if(brightness + 10 < 255){
            brightness += 10;
            analogWrite(BRIGHTPIN, brightness);
        }
    }

    if(value > 200 && value < 400){
        if(brightness - 10 > 0){
            brightness -= 10;
            analogWrite(BRIGHTPIN, brightness);
        }
    }

    delay(100);

}