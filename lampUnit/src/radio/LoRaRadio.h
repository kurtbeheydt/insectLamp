#pragma once

#include "Radio.h"

class LoRaRadio : public Radio {
public:
    LoRaRadio(float freqMHz, float bwKHz, uint8_t spreadingFactor,
              uint8_t syncWord, uint8_t powerDbm);

    bool begin() override;

    int transmit(const String& payload) override;
    void finishTransmit() override;
    void setOnPacketSent(void (*cb)()) override;

    void startReceive() override;
    int readReceived(String& payload) override;
    void setOnPacketReceived(void (*cb)()) override;

    float getRSSI() override;
    float getSNR() override;

    const char* name() const override { return "LoRa"; }

private:
    float frequency_;
    float bandwidth_;
    uint8_t spreadingFactor_;
    uint8_t syncWord_;
    uint8_t power_;
};
