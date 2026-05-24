#include "ZigbeeRadio.h"

bool ZigbeeRadio::begin() {
    Serial.println(F("[Zigbee] not implemented: requires 802.15.4-capable hardware"));
    return false;
}

int ZigbeeRadio::transmit(const String& /*payload*/) {
    return -1;
}

void ZigbeeRadio::finishTransmit() {}

void ZigbeeRadio::setOnPacketSent(void (* /*cb*/)()) {}

void ZigbeeRadio::startReceive() {}

int ZigbeeRadio::readReceived(String& /*payload*/) {
    return -1;
}

void ZigbeeRadio::setOnPacketReceived(void (* /*cb*/)()) {}
