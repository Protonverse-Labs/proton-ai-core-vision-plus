# Example: Camera Capture

Minimal camera example for Proton AI Core Vision+ — initialize the OV5640 camera and capture a single JPEG frame over Serial, with no Wi-Fi or other subsystems involved.

## Purpose

A focused starting point if you only need camera capture (e.g. to save frames to microSD, or to build your own capture logic) without the rest of the full board firmware.

## Requirements

- Proton AI Core Vision+ board with camera attached
- Arduino IDE 2.x with the ESP32 board package installed
- USB-C cable

No additional libraries are required beyond the ESP32 board package (this example doesn't use `Adafruit_NeoPixel`).

## Setup

1. Open `01_camera_capture.ino` in Arduino IDE.
2. **Tools** menu: Board = `ESP32S3 Dev Module`, USB CDC On Boot = `Enabled`, PSRAM = `OPI PSRAM / Enabled`, Flash Size = `16MB`, Partition Scheme = `Huge APP`.

## Build and Flash

Click **Upload** in Arduino IDE.

## Expected Result

Open Serial Monitor at `115200` baud. Type:

```
cam_init
```
```
Camera init OK.
```

```
cam_capture
```
```
JPEG OK: 34521 bytes, 640x480, format=1
```

(Exact byte count varies per frame; resolution defaults to VGA 640×480 when PSRAM is available, QVGA 320×240 otherwise.)

## Troubleshooting

- `Camera init FAILED: 0x...` — confirm the camera FPC cable is fully seated, and that PSRAM is enabled in Tools settings. See [docs/troubleshooting.md](../../docs/troubleshooting.md#camera-init-fails).
- No response to commands — confirm Serial Monitor is set to 115200 baud and line ending is set to send a newline.
