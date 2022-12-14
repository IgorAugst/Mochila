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
#define ARDTX D5
#define TRIGGER D6

#define MOSFETPIN D7

DHT dht(DHTPIN, DHTTYPE);

TinyGPSPlus gps;
SoftwareSerial ss(GPSRX, -1);
SoftwareSerial ard(-1, ARDTX);

bool gpsEnabled = true;
bool isWifiConnected = false;
bool buzzerState = false;
bool isLcdLocked = false;

WiFiClientSecure net;
MQTTClient client = MQTTClient(512);
BearSSL::X509List cert(AWS_CERT_CA);
BearSSL::X509List client_cert(CLIENT_CRT);
BearSSL::PrivateKey key(CLIENT_PRIV);

const char updateWeatherTopic[] = "$aws/things/mochila/shadow/name/weather/update";
const char updateLocationTopic[] = "$aws/things/mochila/shadow/name/location/update";

long lastWeather = 0;
long lastGps = 0;

void lcdMessage(String l1, String l2, int brightness = -1, bool highPriority = false)
{
    if (!isLcdLocked || highPriority)
    {
        StaticJsonDocument<200> doc;
        doc["l1"] = l1;
        doc["l2"] = l2;
        doc["b"] = brightness;
        char payload[200];
        serializeJson(doc, payload);
        ard.println(payload);
    }
}

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
        lcdMessage("Erro ao conectar", "AWS", 255, true);
        delay(300);
    }

    if (!client.connected())
    {
        Serial.println("AWS IoT Timeout!");
        return;
    }
}

void setup()
{

    pinMode(TRIGGER, INPUT_PULLUP);
    pinMode(MOSFETPIN, OUTPUT);

    digitalWrite(MOSFETPIN, LOW);

    dht.begin();
    
    Serial.begin(9600);
    ard.begin(9600);
    ss.begin(9600);
    
    lcdMessage("Iniciando...", "Sistema", 255, true);

    delay(1000);

    Serial.println("Starting...");
    Serial.println("CPU clock: " + String(ESP.getCpuFreqMHz()) + "MHz");

    WiFiManager wifiManager;
    wifiManager.setConfigPortalTimeout(60);
    lcdMessage("Conectando...", "WiFi", 255, true);
    isWifiConnected = wifiManager.autoConnect("ESP-igor", "esp123456");

    if (isWifiConnected && false)
    {
        Serial.println("Connected to wifi");

        configTime(0, 0, "pool.ntp.org");

        net.setTrustAnchors(&cert);
        net.setClientRSACert(&client_cert, &key);


        lcdMessage("Conectando...", "AWS", 255, true);
        connectAWS();

        if (client.connected())
        {
            Serial.println("Connected to AWS");
            client.subscribe("mochila/teste");
        }
    }

}

void publishMessage(String topic, String payload)
{
    if (client.connected())
    {
        client.publish(topic, payload);
    }
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

    lcdMessage("Temp: " + String((int)t) + "C", "Hum: " + String((int)h) + "%");

    publishMessage(updateWeatherTopic, payload);
}

void manageGps()
{
    while (ss.available() > 0)
    {
        if (gps.encode(ss.read()))
        {

            if (gps.location.isValid())
            {
                gpsEnabled = false;
                // ss.end();
                Serial.println("GPS disabled");
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
    if (millis() - lastWeather > 5000)
    {
        manageWeather();
        lastWeather = millis();
    }

    if (gpsEnabled)
    {
        manageGps();
    }

    if (millis() - lastGps > 60000 && !gpsEnabled)
    {
        gpsEnabled = true;
        ss.begin(9600);
        Serial.println("GPS enabled");
        lastGps = millis();
    }

    client.loop();

    int buttonState = digitalRead(TRIGGER);

    if (buttonState == HIGH && buzzerState == false)
    {
        buzzerState = true;
        isLcdLocked = true;
        Serial.println("Buzzer on");
        lcdMessage("Alarme", "Ativado", 255, true);
        digitalWrite(MOSFETPIN, HIGH);
    }
    else if (buttonState == LOW && buzzerState == true)
    {
        buzzerState = false;
        isLcdLocked = false;
        Serial.println("Buzzer off");
        lcdMessage("Alarme", "Desativado", -1, true);
        digitalWrite(MOSFETPIN, LOW);
    }
}