# Firmware Guide

## Framework

The current firmware is built on the **Arduino ESP32 core** (not ESP-IDF directly, not PlatformIO). It is a single Arduino sketch: `firmware/proton-ai-core-vision-plus/Proton_AI_CoreVision_Firmware_V20.ino`.

## Required Build Settings

These come directly from the firmware file's own header comment and are required for a correct build:

| Setting | Value |
|---|---|
| Board | ESP32S3 Dev Module |
| USB CDC On Boot | Enabled |
| PSRAM | OPI PSRAM / Enabled |
| Flash Size | 16MB |
| Partition Scheme | Huge APP |
| Serial Monitor | 115200 baud, Newline or Both NL & CR |

## Dependencies

Arduino libraries used: `WiFi`, `WebServer`, `Wire`, `SPI`, `SD`, `Preferences`, `esp_camera` (ESP32 camera driver, bundled with the ESP32 Arduino core), `driver/i2s` (ESP-IDF I2S driver, callable from Arduino), `Adafruit_NeoPixel`.

Only `Adafruit_NeoPixel` needs to be installed manually via Library Manager — the rest ship with the ESP32 board package.

## Architecture

The firmware is a single file, organized into clearly commented sections:

1. **Pin map and globals** — all GPIO assignments as `#define` constants (see [Pinout](pinout.md))
2. **I2C helpers and bus recovery** — manual bus-clock toggling to recover a stuck I2C bus, used before camera/I2C device tests
3. **LED control** — RGB (WS2812B) and 1W LED (direct GPIO + PWM modes)
4. **Camera resolution/FPS/memory helpers** — resolution preset parsing, memory diagnostics
5. **Camera init/capture/shutdown** — `esp_camera` driver setup, single-frame capture, clean deinit
6. **Camera reboot stability test** — a stress-test mode that reboots the board N times and re-initializes the camera each time, using `Preferences` (NVS) to persist test state across reboots
7. **HTTP server** — `/` (link page), `/jpg` (single JPEG), `/stream` (MJPEG multipart stream)
8. **Wi-Fi connection**
9. **Device tests** — I2C scan, IMU read, microSD mount/write test, I2S microphone read test
10. **Status and help** — `status`, `pins`, `help` serial commands
11. **Serial command parser** — dispatches all commands listed below

## Configuration Points

- **Wi-Fi credentials** — `WIFI_SSID` / `WIFI_PASS` near the top of the file. The published copy in this repository uses placeholders (`YOUR_WIFI_SSID` / `YOUR_WIFI_PASSWORD`) — replace them with your own network before using Wi-Fi-dependent commands.
- **Default capture/stream resolution and JPEG quality** — set via the `captureFrameSize`/`captureJpegQuality`/`streamFrameSize`/`streamJpegQuality` globals, or changed at runtime with `capture_res` / `stream_res` commands.
- **Stream FPS target and auto-timeout** — `streamTargetFps` (default 15) and `streamMaxMs` (default 15000ms), changeable at runtime with `stream_fps` and `stream_timeout`.

## Serial Command Reference

| Category | Commands |
|---|---|
| Basic | `help`, `status`, `pins`, `reboot` |
| I2C | `i2c`, `i2c_reset`, `pca_scan`, `imu`, `crypto` / `atecc` / `secure` |
| Wi-Fi | `wifi`, `ip` |
| Camera | `cam_init`, `cam_capture`, `stream_on`, `stream_off` / `cam_off` / `camera_off` |
| Capture resolution | `capture_res <low\|medium\|high\|veryhigh>` |
| Stream resolution | `stream_res <low\|medium\|high\|veryhigh>` |
| Stream tuning | `stream_fps <1-30>`, `stream_timeout <ms>` |
| Validation | `cam_status` / `res_status`, `cam_reboot_test <1-20>`, `cam_reboot_clear`, `led_cam_test` |
| LEDs | `rgb <r> <g> <b>`, `led on`, `led off`, `led blink`, `led pwm`, `led level <0-255>` |
| Other | `sd` / `sd_test`, `mic` |

HTTP endpoints (once Wi-Fi is connected, via `wifi`): `/` (link page), `/jpg` (single JPEG capture), `/stream` (MJPEG stream — must be enabled first with `stream_on`).

## Build and Flash

See [Flashing Guide](flashing.md).

## Modifying the Firmware

If you change GPIO mappings, camera init parameters, clock settings, or partition/flash settings, keep in mind this firmware has been physically tested as-is — verify your changes on real hardware before relying on them, and keep a copy of the known-working version. Pull requests that change hardware-dependent configuration should explain why the change is needed (see [CONTRIBUTING.md](../CONTRIBUTING.md)).
