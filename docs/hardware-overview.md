# Hardware Overview

This page documents the verified components and subsystems on Proton AI Core Vision+. Every item below is sourced from the working firmware (`firmware/proton-ai-core-vision-plus/`), the approved schematic (`hardware/Schematic.pdf`), or a component datasheet. Anything that could not be verified is explicitly marked.

This is **not** a manufacturing document — it does not include PCB layout information, exact power rail routing, or a purchasable parts list. See [hardware/README.md](../hardware/README.md) for what is and isn't published.

## Processor

- **Module:** ESP32-S3-WROOM-1-N16R8
- **Flash:** 16MB
- **PSRAM:** 8MB, OPI mode
- **Cores:** Dual-core (Xtensa LX7, per the ESP32-S3 module itself)

The flash/PSRAM sizes come directly from the module's own part number (`N16R8` = 16MB flash / 8MB PSRAM, a standard Espressif designation) and are confirmed by the firmware's documented Arduino IDE build settings (`PSRAM: OPI PSRAM / Enabled`, `Flash: 16MB`).

## Camera Interface

- **Sensor:** OV5640, 5MP (2592×1944 array), 1/4" optical format
- **Connector:** 24-pin FPC (board-side: FFC2B28-24-G)
- **Interface:** 8-bit parallel data bus + SCCB (I2C) for sensor control, shared with the board's main I2C bus
- **Lens:** 3.0mm EFL, F2.2, 76.6° field of view (per the camera module datasheet)

See [Camera Guide](camera.md) and [Pinout](pinout.md) for the full signal list.

## Wireless

- **Wi-Fi:** ESP32-S3 built-in radio, used by the firmware's `WiFi.h` / `WebServer.h` implementation for camera streaming and the onboard HTTP server

No Bluetooth functionality is implemented in the current firmware. Wireless range, throughput, and power-consumption figures have not been measured — `TODO: Verification required`.

## USB

- **Connector:** USB-C
- **Mode:** Native ESP32-S3 USB, used as a USB-CDC serial port (`USB CDC On Boot: Enabled` in the firmware's documented build settings)
- **ESD protection:** USBLC6-2SC6
- **Overcurrent protection:** 2A fuse on the USB-C power path (per schematic)

## Power

- **Battery input:** 3.7V Li-Po, 2-pin JST-style connector
- **Charging:** MCP73831-based Li-Po charge management (schematic labels this "Programmable current charging, 500mA")
- **Reverse polarity protection:** P-channel MOSFET on the battery input
- **Regulators:** a 3.3V main rail plus separate 2.8V and 1.5V rails dedicated to the camera module

Exact input voltage limits, maximum charge/discharge current, and battery runtime have not been measured — `TODO: Verification required`.

## Onboard Sensors and Peripherals

| Component | Part | Bus | Verified via |
|---|---|---|---|
| IMU | ICM-42670-P | I2C | Schematic, firmware `imu` command, photo |
| Secure element | ATECC608B-MAHDA-S | I2C | Schematic, firmware `crypto`/`atecc`/`secure` commands, photo |
| I/O expander | PCA9685BS,118 (16-channel PWM/LED driver) | I2C | Schematic, firmware `pca_scan` command, photo |
| Microphone | ICS-43434 (I2S MEMS) | I2S | Schematic, firmware `mic` command |
| Storage | microSD, push-push slot | SPI | Schematic, firmware `sd`/`sd_test` command, photo |

## LEDs

- **Status LED:** 1× addressable RGB (WS2812B), driven on GPIO1
- **Auxiliary LED:** 1× 1W high-power LED, MOSFET-switched on GPIO48 (supports on/off, blink, and PWM dimming in firmware)

## Buttons

- **RESET** — hardware reset (EN)
- **BOOT** — bootloader-entry strap (GPIO0), standard ESP32-S3 boot-mode button

## Expansion

- **I2C header** — 4-pin, breaks out the shared SDA/SCL bus (same bus used by the IMU, secure element, I/O expander, and camera SCCB)
- **UART header** — 2×2 header exposing a hardware UART separate from the USB-C serial console. Exact GPIO assignment: `TODO: Verification required`
- **GPIO/PWM expansion header** — 6-pin header, silkscreened "GPIO EXPANDER — CAN BE PWM OR GPIO"; these are PCA9685 channels (LED0–LED5) controlled over I2C, not directly memory-mapped ESP32-S3 pins

## Mechanical

Board dimensions and mounting hole spacing have not been published — `TODO: Verification required`.

## Hardware Revision

The board silkscreen reads "Proton Ai Core Vision+ — Designed by Protonverse Labs — Protonverse 0.10". Whether "0.10" denotes a specific hardware revision number has not been confirmed — `TODO: Verification required`.

## Not Included in This Repository

Consistent with protecting manufacturing IP, this repository does **not** include: PCB layout/design source files, Gerber files, drill files, pick-and-place/CPL files, panelization files, the full production Bill of Materials (exact reference designators, supplier part numbers, and pricing), or any other manufacturing package. See [hardware/README.md](../hardware/README.md).
