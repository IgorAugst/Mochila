#include <Arduino.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <Secrets.h>
#include <WifiClientSecure.h>
#include <MQTT.h>
#include <DHT.h>

#define DHTPIN D1
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

WiFiClientSecure net;
MQTTClient client = MQTTClient(512);
BearSSL::X509List cert(AWS_CERT_CA);
BearSSL::X509List client_cert(CLIENT_CRT);
BearSSL::PrivateKey key(CLIENT_PRIV);

const char updateWeatherTopic[] = "$aws/things/mochila/shadow/name/weather/update";

long lastWeather = 0;
long lastGps = 0;

void messageHandler(String &topic, String &payload)
{
  Serial.println("incoming: " + topic + " - " + payload);
}

void connectAWS()
{
    Serial.println("Connecting to AWS...");
    client.begin(AWS_IOT_ENDPOINT, 8883, net);
    client.setKeepAlive(60);
    client.onMessage(messageHandler);

    while (!client.connect(THINGNAME))
    {
        Serial.print(".");
        Serial.println(client.lastError());
        delay(100);
    }

    if (!client.connected())
    {
        Serial.println("AWS IoT Timeout!");
        return;
    }

}

void setup()
{
    Serial.begin(9600);
    Serial.println("Starting...");
    Serial.println("CPU clock: " + String(ESP.getCpuFreqMHz()) + "MHz");

    WiFiManager wifiManager;
    wifiManager.autoConnect("ESP-igor", "esp123456");

    Serial.println("Connected to wifi");

    configTime(0, 0, "pool.ntp.org");

    net.setTrustAnchors(&cert);
    net.setClientRSACert(&client_cert, &key);

    connectAWS();

    Serial.println("Connected to AWS");
    client.subscribe("mochila/teste");

    dht.begin();
}

void manageWeather()
{
    float h = dht.readHumidity();
    float t = dht.readTemperature();
        
    StaticJsonDocument<200> doc;
    char payload[200];

    doc["state"]["reported"]["temp"] = t;
    doc["state"]["reported"]["hum"] = h;
        
    serializeJson(doc, payload);

    Serial.println(payload);

    client.publish(updateWeatherTopic, payload);
}

void manageGps()
{
    //TODO
}

void loop()
{
    if(millis() - lastWeather > 5000){
        manageWeather();
        lastWeather = millis();
    }

    if(millis() - lastGps > 60000){
        //manageGps();
        lastGps = millis();
    }

    client.loop();
}