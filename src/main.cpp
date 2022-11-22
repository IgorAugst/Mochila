#include <Arduino.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <Secrets.h>
#include <WifiClientSecure.h>
#include <MQTT.h>

WiFiClientSecure net;
MQTTClient client = MQTTClient(512);
BearSSL::X509List cert(AWS_CERT_CA);
BearSSL::X509List client_cert(CLIENT_CRT);
BearSSL::PrivateKey key(CLIENT_PRIV);

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

}

void loop()
{
    client.loop();
}