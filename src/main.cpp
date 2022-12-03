#include <Arduino.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <Secrets.h>
#include <WifiClientSecure.h>
#include <MQTT.h>
#include <DHT.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

#define DHTPIN D1
#define DHTTYPE DHT11

#define GPSRX D2

DHT dht(DHTPIN, DHTTYPE);

TinyGPSPlus gps;
SoftwareSerial ss(GPSRX,-1);
bool gpsEnabled = true;

WiFiClientSecure net;
MQTTClient client = MQTTClient(512);
BearSSL::X509List cert(AWS_CERT_CA);
BearSSL::X509List client_cert(CLIENT_CRT);
BearSSL::PrivateKey key(CLIENT_PRIV);

const char updateWeatherTopic[] = "$aws/things/mochila/shadow/name/weather/update";
const char updateLocationTopic[] = "$aws/things/mochila/shadow/name/location/update";

long lastWeather = 0;
long lastGps = 0;

void messageHandler(String &topic, String &payload)
{
  Serial.println("incoming: " + topic + " - " + payload);
}

void connectAWS()
{
    long lastReconnectAttempt = 0;

    Serial.println("Connecting to AWS...");
    client.begin(AWS_IOT_ENDPOINT, 8883, net);
    client.setKeepAlive(60);
    client.onMessage(messageHandler);

    while (!client.connect(THINGNAME) && (millis() - lastReconnectAttempt < 5000))
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
    wifiManager.setConfigPortalTimeout(180);
    wifiManager.autoConnect("ESP-igor", "esp123456");

    Serial.println("Connected to wifi");

    configTime(0, 0, "pool.ntp.org");

    net.setTrustAnchors(&cert);
    net.setClientRSACert(&client_cert, &key);

    connectAWS();

    Serial.println("Connected to AWS");
    client.subscribe("mochila/teste");

    dht.begin();
    ss.begin(9600);

}

void publishMessage(String topic, String payload)
{
    client.publish(topic, payload);
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

    publishMessage(updateWeatherTopic, payload);
}

void manageGps()
{
    while(ss.available() > 0){
        if(gps.encode(ss.read())){
            
            if(gps.location.isValid()){
                gpsEnabled = false;
                //ss.end();

                StaticJsonDocument<200> doc;
                char payload[200];

                doc["state"]["reported"]["lat"] = gps.location.lat();
                doc["state"]["reported"]["lon"] = gps.location.lng();
                
                serializeJson(doc, payload);

                Serial.println(payload);
                publishMessage(updateLocationTopic, payload);
            }
        }
    }
}

void loop()
{
    if(millis() - lastWeather > 5000){
        manageWeather();
        lastWeather = millis();
    }

    if(gpsEnabled){
        manageGps();
    }

    if(millis() - lastGps > 5000 && !gpsEnabled){
        gpsEnabled = true;
        ss.begin(9600);
        Serial.print("GPS enabled");
        lastGps = millis();
    }

    client.loop();
}