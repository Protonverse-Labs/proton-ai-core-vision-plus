# Pinout

All GPIO assignments below are taken directly from the `#define` pin map in `firmware/proton-ai-core-vision-plus/Proton_AI_CoreVision_Firmware_V20.ino` (the highest-priority source per project policy) and cross-checked against the approved schematic (`hardware/Schematic.pdf`), where the same net names appear. This is the authoritative source — if you're wiring against this board, trust this table over any inference from photos or connector position.

## Camera (24-pin FPC)

| Signal | GPIO | Notes |
|---|---:|---|
| PWDN | 2 | Power-down control |
| RESET | 45 | Labeled `CAMERA_RET` on the schematic |
| XCLK | 40 | Camera master clock |
| SIOD (SDA) | 17 | Shared with the board's main I2C bus |
| SIOC (SCL) | 18 | Shared with the board's main I2C bus |
| VSYNC | 21 | |
| HREF | 38 | |
| PCLK | 11 | |

### Camera data bus — two naming conventions, same pins

The firmware's `#define` constants use ESP32-side "D0–D7" naming; the firmware's own debug output (`pins` command) and the camera sensor's datasheet use OV5640-side "Y2–Y9" naming. Both refer to the same physical signals — they are cross-verified against the OV5640 module datasheet (`Technical/5640.pdf` in the source materials) and are **not** a contradiction, just two different naming conventions for the same 8-bit bus:

| Firmware `CAM_PIN_Dx` | Sensor pin name (Yx) | GPIO |
|---|---|---:|
| D0 | Y2 | 13 |
| D1 | Y3 | 47 |
| D2 | Y4 | 14 |
| D3 | Y5 | 3 |
| D4 | Y6 | 12 |
| D5 | Y7 | 42 |
| D6 | Y8 | 41 |
| D7 | Y9 | 39 |

## I2C Bus (shared)

Used by the camera's SCCB control interface, the IMU, the secure element, and the I/O expander. Also broken out to a 4-pin header on the board.

| Signal | GPIO |
|---|---:|
| SDA | 17 |
| SCL | 18 |

| Device | I2C address |
|---|---|
| PCA9685 (I/O expander) | 0x40 |
| ATECC608B (secure element) | 0x60 |
| ICM-42670-P (IMU) | 0x68 or 0x69 (address-pin dependent) |

## RGB Status LED

| Signal | GPIO |
|---|---:|
| WS2812B data | 1 |

## 1W LED

| Signal | GPIO |
|---|---:|
| MOSFET gate (PWM-capable) | 48 |

## I2S Microphone (ICS-43434)

| Signal | GPIO |
|---|---:|
| BCLK | 16 |
| DATA | 8 |
| WS | 9 |

## microSD (SPI)

| Signal | GPIO |
|---|---:|
| CS | 5 |
| MOSI | 6 |
| CLK | 15 |
| MISO | 10 |

## Buttons

| Button | Function |
|---|---|
| RESET | Hardware reset (EN) |
| BOOT | Bootloader-entry strap (GPIO0) |

## Headers

| Header | Exposes | Notes |
|---|---|---|
| I2C (4-pin) | GPIO17 (SDA), GPIO18 (SCL) | Same bus as camera/IMU/secure element/expander |
| UART (2×2) | Hardware UART, separate from the USB-C serial console | Exact GPIO assignment: `TODO: Verification required` |
| GPIO/PWM expansion (6-pin) | PCA9685 channels LED0–LED5 | I2C-controlled, not direct ESP32-S3 GPIOs |

## A Note on Schematic Reference Designators

The approved schematic reuses the reference designator "J2" for two different connectors (the I2C header and the battery connector). This is a cosmetic labeling issue in the schematic and does not affect any of the GPIO assignments above, which come from the firmware's pin definitions. If you're reading the schematic directly, be aware of this when looking up "J2."
