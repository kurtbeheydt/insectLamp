#pragma once

#include "Radio.h"

// TODO(zwave): hardware not yet supported.
// Z-Wave is a proprietary 868/908 MHz mesh protocol and cannot be implemented
// on top of a generic SX1262. A working backend needs:
//   - a dedicated Z-Wave SoC (e.g. Silicon Labs ZGM130S or ZGM230S, or an
//     older Sigma Designs ZM5101 module) connected over UART/SPI, AND
//   - access to the Z-Wave 700/800 SDK from Silicon Labs (gated behind their
//     developer program) for firmware on that SoC.
// Once the hardware is in place, replace these stubs with a Z-Wave Serial API
// driver that joins a Z-Wave network and decodes incoming Binary/Multilevel
// Switch CC frames.
class ZWaveRadio : public Radio {
public:
    bool begin() override;

    int transmit(const String& payload) override;
    void finishTransmit() override;
    void setOnPacketSent(void (*cb)()) override;

    void startReceive() override;
    int readReceived(String& payload) override;
    void setOnPacketReceived(void (*cb)()) override;

    const char* name() const override { return "Z-Wave"; }
};
