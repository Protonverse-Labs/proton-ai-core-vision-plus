# Proton AI Core Vision+ Firmware

`Proton_AI_CoreVision_Firmware_V20.ino` is the working Arduino firmware for Proton AI Core Vision+ — the same firmware used to physically test this hardware, not a simplified stand-in.

This is a **sanitized copy** of the original working firmware: the only change from the internal working version is that the hardcoded Wi-Fi SSID and password have been replaced with placeholders (`YOUR_WIFI_SSID` / `YOUR_WIFI_PASSWORD`). No functional code — GPIO mappings, camera configuration, clock settings, or partition/flash settings — has been changed.

## Before You Flash

Set your own Wi-Fi credentials near the top of the file if you plan to use Wi-Fi-dependent features (streaming, HTTP endpoints):

```cpp
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
```

Wi-Fi is not required for Serial-based commands (camera capture over Serial, LED control, I2C/SD/mic tests).

## Build Settings, Flashing, and Command Reference

See:
- [docs/flashing.md](../../docs/flashing.md) — step-by-step build/flash instructions
- [docs/firmware.md](../../docs/firmware.md) — architecture and full serial command reference
- [docs/getting-started.md](../../docs/getting-started.md) — full walkthrough with expected output

## Looking for a Smaller Starting Point?

See [examples/](../../examples/) for focused, single-purpose sketches extracted from this firmware (camera capture only, Wi-Fi streaming only, LED control only).
