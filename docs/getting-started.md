# Getting Started

This walkthrough assumes no prior familiarity with this project. If you already know Arduino/ESP32 development, the [README Quick Start](../README.md#quick-start) is a faster path.

## What You Need

- A Proton AI Core Vision+ board
- A USB-C cable (data-capable, not power-only)
- A computer with [Arduino IDE](https://www.arduino.cc/en/software) 2.x installed
- (Optional) A 2.4GHz Wi-Fi network, if you want camera streaming
- (Optional) A microSD card, if you want to test storage

## Supported Development Environment

This project currently ships firmware for the **Arduino ESP32 core**, built and flashed through the Arduino IDE. ESP-IDF and PlatformIO project files are not provided in this repository.

## 1. Install the ESP32 Board Package

In Arduino IDE: **Tools → Board → Boards Manager**, search `esp32`, install the package by **Espressif Systems**.

## 2. Install the Required Library

**Tools → Manage Libraries**, search `Adafruit NeoPixel`, install it. This is the only third-party library the firmware needs — everything else ships with the ESP32 board package.

## 3. Connect the Board

Plug in Proton AI Core Vision+ with a USB-C cable. No additional USB driver installation has been reported as necessary — the board uses the ESP32-S3's native USB as a CDC serial port. If the board doesn't appear as a serial port, see [Troubleshooting](troubleshooting.md#board-not-detected--upload-fails).

## 4. Clone or Download This Repository

```bash
git clone https://github.com/Protonverse-Labs/proton-ai-core-vision-plus.git
cd proton-ai-core-vision-plus
```

## 5. Open the Firmware

Open `firmware/proton-ai-core-vision-plus/Proton_AI_CoreVision_Firmware_V20.ino` in Arduino IDE.

## 6. Configure Build Settings

**Tools** menu:

| Setting | Value |
|---|---|
| Board | ESP32S3 Dev Module |
| USB CDC On Boot | Enabled |
| PSRAM | OPI PSRAM / Enabled |
| Flash Size | 16MB |
| Partition Scheme | Huge APP |
| Port | Select the board's port |

## 7. (Optional) Set Wi-Fi Credentials

If you want camera streaming or the HTTP endpoints, edit these two lines near the top of the `.ino` file:

```cpp
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
```

Skip this step if you only want to use the Serial-based commands (camera capture over Serial, LED control, I2C/SD/mic tests) — Wi-Fi is not required for those.

## 8. Build and Flash

Click **Upload** in Arduino IDE. First compiles can take a few minutes.

## 9. First Boot — Expected Output

Open **Tools → Serial Monitor**, set to `115200` baud. On boot, you should see something like:

```
Proton AI Core-Vision ESP32-S3 standalone firmware v20

==== Proton AI Core-Vision Commands ====
Basic: help, status, pins, reboot
I2C: i2c, i2c_reset, pca_scan, imu, crypto, atecc, secure
WiFi: wifi, ip
Camera: cam_init, cam_capture, stream_on, stream_off, cam_off, camera_off
...

Camera: PWDN=2 RESET=45 XCLK=40 SIOD/SDA=17 SIOC/SCL=18
...

==== Status ====
PSRAM: YES, heap=..., psram=..., minHeap=...
WiFi: OFF/FAILED
Camera: NOT INIT
Stream: DISABLED
...
I2C status using raw ACK check:
  PCA9685    0x40: OK, code=0
  ATECC608B  0x60: OK, code=0
  ICM42670   0x69: OK, code=0
```

(The firmware's own boot banner currently reads "Proton AI Core-Vision" — the product name used throughout this repository is Proton AI Core Vision+. See [FAQ](faq.md) if this is confusing.)

If the three I2C devices above don't show `OK`, that's worth investigating before continuing — see [Troubleshooting](troubleshooting.md#i2c-device-not-found-imu--secure-element--io-expander).

## 10. First Camera Test

In the Serial Monitor input field, type each command and press Enter:

```
cam_init
cam_capture
```

Expected output:

```
Camera detected. PID=0x... VER=... MIDH=... MIDL=...
Camera init OK.
JPEG OK: <bytes> bytes, 640x480, format=1
```

## 11. (Optional) First Wi-Fi Camera Stream

```
wifi
stream_on
ip
```

Open the printed `http://<ip>/stream` URL in a browser to see the live MJPEG stream. See [Camera Guide](camera.md) for details and the stream auto-timeout behavior.

## Common Problems

See [Troubleshooting](troubleshooting.md) for the full list. Most first-time issues are one of:
- Wrong Tools → Board settings (especially PSRAM/Flash Size/Partition Scheme)
- Camera FPC cable not fully seated
- Wi-Fi credentials not set, or a 5GHz-only network (ESP32-S3 requires 2.4GHz)

## Next Steps

- [Camera Guide](camera.md) — full camera command reference
- [Firmware Guide](firmware.md) — architecture and full serial command reference
- [Example Projects](../examples/) — smaller, focused sketches to start from
