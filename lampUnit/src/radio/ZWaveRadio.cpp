#include "ZWaveRadio.h"

bool ZWaveRadio::begin() {
    Serial.println(F("[Z-Wave] not implemented: requires Z-Wave SoC and Silicon Labs SDK"));
    return false;
}

int ZWaveRadio::transmit(const String& /*payload*/) {
    return -1;
}

void ZWaveRadio::finishTransmit() {}

void ZWaveRadio::setOnPacketSent(void (* /*cb*/)()) {}

void ZWaveRadio::startReceive() {}

int ZWaveRadio::readReceived(String& /*payload*/) {
    return -1;
}

void ZWaveRadio::setOnPacketReceived(void (* /*cb*/)()) {}
