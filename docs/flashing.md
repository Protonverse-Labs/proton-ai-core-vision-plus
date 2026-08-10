# Flashing Guide

## 1. Install the Arduino IDE

Download and install [Arduino IDE](https://www.arduino.cc/en/software) 2.x.

## 2. Install the ESP32 Board Package

Arduino IDE → **Preferences** → add the Espressif ESP32 boards index URL if you don't already have the ESP32 board package installed, then:

**Tools → Board → Boards Manager** → search `esp32` (by Espressif Systems) → **Install**.

## 3. Install Required Libraries

**Tools → Manage Libraries** → search `Adafruit NeoPixel` → **Install**.

The other libraries used by the firmware (`WiFi`, `WebServer`, `Wire`, `SPI`, `SD`, `Preferences`, `esp_camera`, `driver/i2s`) ship with the ESP32 board package — no separate install needed.

## 4. Connect the Board

Connect Proton AI Core Vision+ to your computer with a USB-C cable. The board should enumerate as a USB serial device; no separate USB driver has been reported as necessary for this board's native USB-CDC connection.

## 5. Select Board and Settings

**Tools** menu:

| Setting | Value |
|---|---|
| Board | ESP32S3 Dev Module |
| USB CDC On Boot | Enabled |
| PSRAM | OPI PSRAM / Enabled |
| Flash Size | 16MB |
| Partition Scheme | Huge APP |
| Port | Select the port corresponding to the board |

These exact settings come from the firmware file's own header comment and are required — using different Flash/PSRAM/Partition settings than what the firmware expects can cause boot failures or camera init failures.

## 6. Open the Firmware

Open `firmware/proton-ai-core-vision-plus/Proton_AI_CoreVision_Firmware_V20.ino` in Arduino IDE.

## 7. (Optional) Configure Wi-Fi

If you plan to use Wi-Fi-dependent commands (`wifi`, streaming, HTTP endpoints), edit these two lines near the top of the file before flashing:

```cpp
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
```

Replace both placeholders with your own network credentials. Do not commit real credentials back into a shared or public copy of this file.

## 8. Build and Upload

Click **Upload** (→) in Arduino IDE. The IDE will compile the sketch and flash it over USB-C. If upload fails to start, see [Troubleshooting](troubleshooting.md#board-not-detected--upload-fails).

## 9. Open the Serial Monitor

**Tools → Serial Monitor**, set to `115200` baud, line ending "Newline" or "Both NL & CR". On boot you should see:

```
Proton AI Core-Vision ESP32-S3 standalone firmware v20
==== Proton AI Core-Vision Commands ====
...
==== Status ====
...
```

(The firmware's own boot banner currently prints "Proton AI Core-Vision"; the product name used throughout this repository is Proton AI Core Vision+.)

## 10. Verify the Board Is Working

Type `status` and press Enter. You should see PSRAM detection, Wi-Fi state, camera state, and I2C device ACK results for the onboard IMU, secure element, and I/O expander.

Type `cam_init` then `cam_capture` to confirm the camera is working — see [Camera Guide](camera.md).
