# insectLamp

IoT-controlled insect lamp: a Node.js backend with a web UI publishes power-LED
brightness levels over MQTT; an ESP32 *bridge* receives them and forwards them
over LoRa to one or more remote ESP32 *lamp units* that drive the actual power
LEDs via PWM.

## Architecture

```
  ┌──────────────┐   MQTT     ┌──────────────────┐   LoRa    ┌──────────────┐
  │ Web UI       │ ────────►  │ lampBridge       │ ────────► │ lampUnit     │
  │ (browser)    │            │ (ESP32 + SX1262) │           │ (ESP32 +     │
  │              │            │   - WiFi/MQTT    │           │   SX1262)    │
  └──────┬───────┘            │   - LoRa TX      │           │   - LoRa RX  │
         │                    │   - drives 4 PWM │           │   - drives 4 │
         │ HTTP/JSON          │     channels     │           │     PWM      │
         ▼                    └──────────────────┘           │     channels │
  ┌──────────────┐                                           └──────────────┘
  │ backend      │
  │ (Node.js +   │
  │  Express +   │
  │  MQTT bus)   │
  └──────────────┘
```

Topics:
- `insectLamp/webUpdate` — UI → backend → bridge; sets PWM values.
- `insectLamp/unitUpdate` — backend → bridge; broadcast of current PWM state.
- `insectLamp/init` — bridge → backend on first connect; triggers state replay.

## Layout

| Path           | What                                                    |
|----------------|---------------------------------------------------------|
| `backend/`     | Node.js server + browser UI (Express, mqtt.js, jQuery). |
| `lampBridge/`  | PlatformIO firmware for the WiFi/MQTT ↔ LoRa bridge.    |
| `lampUnit/`    | PlatformIO firmware for a LoRa-only lamp unit.          |
| `design/`      | Hardware design (LED holder; `ledholder/` v1, `ledholder_v2/` v2). |
| `docs/`        | Reference photos and pinouts.                           |

## Hardware

- **Bridge & unit board:** LilyGO T3-S3 (ESP32-S3 + SX1262 LoRa). See
  `docs/T3-S3.jpg`.
- **LED driver:** TIP120 Darlington per channel. See `docs/TIP120-Pinout.jpg`.
- **Power LEDs:** 4 channels, each PWM-driven (8-bit, 10 kHz). Default GPIO map:
  - Bridge: `42, 46, 45, 41` (LEDC channels `0..3`).
  - Unit:   `41, 45, 46, 42` (LEDC channels `0..3`).

### LED holder

- `design/ledholder/` — v1: basic 60×30 mm two-part snap housing (STL only).
- `design/ledholder_v2/led_holder_v2.scad` — v2: parametric OpenSCAD redesign.
  All key dimensions (housing size, fin count, screw size, pole diameter,
  tilt detents, channel count, etc.) are variables at the top of the file.
  Features:
  - **Heat dissipation:** external vertical heatsink fins along the long axis,
    thickened LED-side floor as a heat spreader, side ventilation slots.
  - **Cable management:** strain-relief clamping slot at the entry, grommet
    groove for weatherproofing, zip-tie anchor, and four internal rib-pairs
    that keep each channel's wire pair separated and away from the LED zone.
  - **Mounting:** parametric pole/pipe clamp (separate part), flat-mount tabs
    with M4 screw holes, keyhole quick-attach slots, and an optional tilt
    bracket with angle detents.

  Export STLs (requires OpenSCAD on `PATH`):
  ```sh
  cd design/ledholder_v2
  openscad -o top.stl    -D 'part="top"'    led_holder_v2.scad
  openscad -o bottom.stl -D 'part="bottom"' led_holder_v2.scad
  openscad -o clamp.stl  -D 'part="clamp"'  led_holder_v2.scad
  openscad -o tilt.stl   -D 'part="tilt"'   led_holder_v2.scad
  ```

## Backend setup

```sh
cd backend
npm install
cp config.json.example config.json
# edit config.json: at minimum set mqtt.host / port and a user.name / pass
node server.js
```

The server reads `backend/config.json` at startup. Open
`http://localhost:<port>` for the slider UI.

## Firmware setup

Both firmwares use PlatformIO:

```sh
# bridge
cd lampBridge
cp src/credentials.h.example src/credentials.h   # set mqttClientId, etc.
pio run -t upload

# unit
cd ../lampUnit
cp src/credentials.h.example src/credentials.h
pio run -t upload
```

On first boot the bridge starts a WiFi AP named `InsectLampAP`; connect to it
to provision WiFi credentials via WiFiManager.

## Security

- The MQTT broker should require TLS + auth for any non-LAN deployment. The
  bridge ships a `letsencrypt.h` CA bundle for `mqtts://` use; toggle the
  `mqtts` block in `lampBridge/src/main.cpp` once your broker is configured.
- **The LoRa link is currently unencrypted.** Anyone in range can replay or
  inject `setPower` payloads. See the `TODO: encrypt LoRa payload` markers in
  `lampBridge/src/main.cpp` and `lampUnit/src/main.cpp`. A symmetric scheme
  (AES-128 with a shared key in `credentials.h`) is the smallest fix.
- `backend/config.json` and `*/src/credentials.h` are gitignored. Never commit
  them.
