#include "LoRaRadio.h"

#include <RadioLib.h>

#include "../LoRaBoards.h"

static SX1262 sx1262 = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

LoRaRadio::LoRaRadio(float freqMHz, float bwKHz, uint8_t spreadingFactor,
                     uint8_t syncWord, uint8_t powerDbm)
    : frequency_(freqMHz),
      bandwidth_(bwKHz),
      spreadingFactor_(spreadingFactor),
      syncWord_(syncWord),
      power_(powerDbm) {}

bool LoRaRadio::begin() {
    int state = sx1262.begin(frequency_, bandwidth_, spreadingFactor_, 5, syncWord_, power_);
    return state == RADIOLIB_ERR_NONE;
}

int LoRaRadio::transmit(const String& payload) {
    return sx1262.startTransmit(payload.c_str());
}

void LoRaRadio::finishTransmit() {
    sx1262.finishTransmit();
}

void LoRaRadio::setOnPacketSent(void (*cb)()) {
    sx1262.setPacketSentAction(cb);
}

void LoRaRadio::startReceive() {
    sx1262.startReceive();
}

int LoRaRadio::readReceived(String& payload) {
    return sx1262.readData(payload);
}

void LoRaRadio::setOnPacketReceived(void (*cb)()) {
    sx1262.setDio1Action(cb);
}

float LoRaRadio::getRSSI() {
    return sx1262.getRSSI();
}

float LoRaRadio::getSNR() {
    return sx1262.getSNR();
}
