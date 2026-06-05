#pragma once

#include <Arduino.h>

// Abstraction over a wireless link between bridge and lamp units.
// Implementations: LoRaRadio (working), ZigbeeRadio (stub), ZWaveRadio (stub).
class Radio {
public:
    virtual ~Radio() = default;

    // Initialise the underlying hardware. Returns true on success.
    virtual bool begin() = 0;

    // Transmit side (used by lampBridge).
    virtual int transmit(const String& payload) = 0;
    virtual void finishTransmit() = 0;
    virtual void setOnPacketSent(void (*cb)()) = 0;

    // Receive side (used by lampUnit; safe no-ops on TX-only backends).
    virtual void startReceive() = 0;
    virtual int readReceived(String& payload) = 0;
    virtual void setOnPacketReceived(void (*cb)()) = 0;

    // Link quality on the last received packet, where supported.
    virtual float getRSSI() { return 0.0f; }
    virtual float getSNR() { return 0.0f; }

    // Short identifier for logging/UI ("LoRa", "Zigbee", "Z-Wave").
    virtual const char* name() const = 0;
};
