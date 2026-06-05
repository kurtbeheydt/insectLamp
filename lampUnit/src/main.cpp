#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <RadioLib.h>
#include <WiFiClientSecure.h>
#include <WiFiManager.h>

#include "LoRaBoards.h"
#include "credentials.h"
#include "letsencrypt.h"

#define MODE_LORA 1
#define MODE_WIFI 2
#define MODE_BRIDGE 3  // mode for bridging LoRa and WiFi

uint8_t wirelessMode = MODE_LORA;

// wifi settings
WiFiClientSecure WiFiSecure;
const char* apName = "InsectLampAP";
#define WIFI_STATUS_AP 1
#define WIFI_STATUS_CONNECTED 2
#define WIFI_STATUS_DISCONNECTED 3

uint8_t wifiStatus = 0;

// lora settings
const float frequency = 868.0;
const float bandwidth = 125.0;
const uint8_t spreadingFactor = 10;
const uint8_t syncword = 0x34;
const uint8_t power = 20;

SX1262 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
static volatile bool receivedFlag = false;

// mqtt client
const char* mqttBroker = "mqtt.allesaanelkaar.be";
const uint16_t mqttPort = 8886;

PubSubClient mqttClient(mqttBroker, mqttPort, WiFiSecure);

// ledPins
const uint8_t powerLedCount = 4;

struct PowerLed {
    const uint8_t id;
    const uint8_t pin;
    uint16_t pwm;
    const uint8_t channel;
};

PowerLed powerLeds[powerLedCount] = {
    {1, 41, 0, 0},
    {2, 45, 0, 1},
    {3, 46, 0, 2},
    {4, 42, 0, 3}};

void statusLed(bool newValue = false) {
#ifdef BOARD_LED
    digitalWrite(BOARD_LED, newValue);
#endif
}

void onPacketReceived(void) {
    receivedFlag = true;
}

void setPowerLeds() {
    for (uint8_t i = 0; i < powerLedCount; i++) {
        ledcWrite(powerLeds[i].channel, powerLeds[i].pwm);
    }
}

void printSetup() {
    if (u8g2) {
        u8g2->clearBuffer();

        String header;
        u8g2->setFont(u8g2_font_NokiaLargeBold_tf);

        if (wirelessMode == MODE_LORA) {
            header = "LoRa Mode";
        } else if (wirelessMode == MODE_WIFI) {
            header = "WiFi Mode";
        }

        uint16_t str_w = u8g2->getStrWidth(header.c_str());
        u8g2->drawStr((u8g2->getWidth() - str_w) / 2, 16, header.c_str());
        u8g2->drawHLine(5, 21, u8g2->getWidth() - 5);

        u8g2->setFont(u8g2_font_NokiaSmallPlain_tf);

        if (wirelessMode == MODE_LORA) {
            String frequencyText = "Frequency: " + String(frequency) + " MHz";
            u8g2->drawStr(0, 34, frequencyText.c_str());

            String bandwidthText = "BandWidth: " + String(bandwidth) + " kHz";
            u8g2->drawStr(0, 46, bandwidthText.c_str());

            String spreadingFactorText = "SpreadingFactor: " + String(spreadingFactor);
            u8g2->drawStr(0, 58, spreadingFactorText.c_str());
        } else if (wirelessMode == MODE_WIFI) {
            if (wifiStatus == WIFI_STATUS_AP) {
                u8g2->drawStr(0, 34, "WiFi: AP Mode");

                String apText = "AP: " + String(apName);
                u8g2->drawStr(0, 46, apText.c_str());
            } else if (wifiStatus == WIFI_STATUS_CONNECTED) {
                String ssid = "SSID: " + WiFi.SSID();
                u8g2->drawStr(0, 34, ssid.c_str());

                IPAddress localIp = WiFi.localIP();
                String ipaddress = "Ip: " + String(localIp[0]) + "." + String(localIp[1]) + "." + String(localIp[2]) + "." + String(localIp[3]);
                u8g2->drawStr(0, 46, ipaddress.c_str());

                if (!mqttClient.connected()) {
                    u8g2->drawStr(0, 58, "MQTT: Disconnected");
                } else {
                    u8g2->drawStr(0, 58, "MQTT: Connected");
                }

            } else if (wifiStatus == WIFI_STATUS_DISCONNECTED) {
                u8g2->drawStr(0, 34, "WiFi: Disconnected");
            }
        }

        u8g2->sendBuffer();
    }
}

void processPayload(const String& payloadStr) {
    StaticJsonDocument<1024> payloadObject;
    DeserializationError error = deserializeJson(payloadObject, payloadStr);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }

    if (payloadObject.containsKey("action")) {
        String action = payloadObject["action"].as<String>();
        Serial.print("Action: ");
        Serial.println(action);

        if (action == "setPower") {
            JsonObject ledValues = payloadObject["ledValues"];

            for (JsonPair kv : ledValues) {
                int id = atoi(kv.key().c_str());
                int pwm = kv.value().as<int>();

                if (id >= 1 && id <= powerLedCount) {
                    powerLeds[id - 1].pwm = pwm;
                } else {
                    Serial.print(F("Ignoring out-of-range LED id: "));
                    Serial.println(id);
                }
            }

            setPowerLeds();
        }
    } else {
        Serial.println(F("No action specified in payload"));
    }
}

void onMqttMessage(char* topic, byte* payload, unsigned int length) {
    statusLed(true);

    Serial.println("====================================================");
    Serial.print("MQTT Message arrived [");
    Serial.print(topic);
    Serial.println("] ");

    char msg[length + 1];
    memcpy(msg, payload, length);
    msg[length] = '\0';
    String payloadStr = String(msg);

    Serial.print("Payload as String: ");
    Serial.println(payloadStr);

    processPayload(payloadStr);

    Serial.println("====================================================");
}

void mqttReconnect() {
    // Loop until we're reconnected
    while (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");

        if (mqttClient.connect(mqttClientId, mqttUsername, mqttPassword)) {
            String initMessage = "{\"action\": \"init\",\"clientId\":\"" + String(mqttClientId) + "\"}";

            mqttClient.setCallback(onMqttMessage);
            Serial.println(F("MQTT connected!"));

            mqttClient.subscribe("insectLamp/unitUpdate");
            mqttClient.publish("insectLamp/init", initMessage.c_str());
        } else {
            Serial.println(F("MQTT connection failed!"));
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(3000);
        }

        printSetup();
    }
}

void setup() {
    setupBoards(false, true);
    delay(500);

    for (uint8_t i = 0; i < powerLedCount; i++) {
        ledcSetup(powerLeds[i].channel, 10000, 8);
        ledcAttachPin(powerLeds[i].pin, powerLeds[i].channel);
        ledcWrite(powerLeds[i].channel, powerLeds[i].pwm);
    }

    analogWriteResolution(8);
    analogWriteFrequency(5000);

    if (wirelessMode == MODE_LORA) {
        Serial.print(F("Lora Initializing ... "));
        int state = radio.begin(frequency, bandwidth, spreadingFactor, 5, syncword, power);

        printResult(state == RADIOLIB_ERR_NONE);

        if (state == RADIOLIB_ERR_NONE) {
            Serial.println(F("success!"));
        } else {
            Serial.print(F("failed, code "));
            Serial.println(state);
            while (true);
        }

        radio.setDio1Action(onPacketReceived);  // when new packet is received

        radio.startReceive();
    } else if (wirelessMode == MODE_WIFI) {
        Serial.println(F("Starting WiFi Manager..."));
        wifiStatus = WIFI_STATUS_AP;
        printSetup();

        WiFiManager wifiManager;
        bool res = wifiManager.autoConnect(apName);

        if (!res) {
            Serial.println(F("Failed to connect or hit timeout"));
            wifiStatus = WIFI_STATUS_DISCONNECTED;
        } else {
            wifiStatus = WIFI_STATUS_CONNECTED;
            WiFiSecure.setCACert(letsencrypt_root_ca);
            Serial.println(F("connected... waiting for MQTT connection..."));
        }
    }

    printSetup();

    if (wirelessMode == MODE_WIFI) {
        Serial.println(F("Setting up MQTT client..."));
        mqttReconnect();
    }

    Serial.println("loaded");
}

void loop() {
    if (wirelessMode == MODE_WIFI) {
        if (!mqttClient.connected()) {
            mqttReconnect();
        }
        mqttClient.loop();
    }

    if (wirelessMode == MODE_LORA) {
        if (receivedFlag) {
            statusLed(true);

            receivedFlag = false;
            String payloadStr;
            int state = radio.readData(payloadStr);

            Serial.println("====================================================");

            if (state == RADIOLIB_ERR_NONE) {
                Serial.println(F("[SX1262] Received packet!"));

                // print data of the packet
                Serial.print(F("[SX1262] Data:\t\t"));
                Serial.println(payloadStr);

                // print RSSI (Received Signal Strength Indicator)
                Serial.print(F("[SX1262] RSSI:\t\t"));
                Serial.print(radio.getRSSI());
                Serial.println(F(" dBm"));

                // print SNR (Signal-to-Noise Ratio)
                Serial.print(F("[SX1262] SNR:\t\t"));
                Serial.print(radio.getSNR());
                Serial.println(F(" dB"));

                processPayload(payloadStr);

            } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
                // packet was received, but is malformed
                Serial.println(F("CRC error!"));

            } else {
                // some other error occurred
                Serial.print(F("failed, code "));
                Serial.println(state);
            }

            Serial.println("====================================================");
        }
    }

    statusLed(false);
}