#pragma once

#include "Radio.h"

// TODO(zigbee): hardware not yet supported.
// The current bridge board (ESP32-S3 + SX1262) has no IEEE 802.15.4 PHY,
// so a Zigbee implementation needs either:
//   - an external Zigbee coordinator module over UART/SPI
//     (e.g. Silicon Labs EFR32MG21 with EmberZNet, or a TI CC2652 with Z-Stack), or
//   - swapping the SoC for one with a built-in 802.15.4 radio
//     (ESP32-H2 or ESP32-C6, which Espressif's esp-zigbee-sdk targets).
// Once the hardware is in place, replace these stubs with a driver that
// joins/forms a Zigbee network and exchanges payloads as cluster commands.
class ZigbeeRadio : public Radio {
public:
    bool begin() override;

    int transmit(const String& payload) override;
    void finishTransmit() override;
    void setOnPacketSent(void (*cb)()) override;

    void startReceive() override;
    int readReceived(String& payload) override;
    void setOnPacketReceived(void (*cb)()) override;

    const char* name() const override { return "Zigbee"; }
};
