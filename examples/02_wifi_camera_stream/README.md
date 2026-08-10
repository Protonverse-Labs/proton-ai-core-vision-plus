# Example: Wi-Fi Camera Stream

Connects Proton AI Core Vision+ to Wi-Fi and serves a live MJPEG camera stream over HTTP, with no Serial commands, LED control, or other subsystems involved.

## Purpose

A focused starting point if you want the board to act as a standalone network camera, or as a base to build your own HTTP endpoints on top of.

## Requirements

- Proton AI Core Vision+ board with camera attached
- Arduino IDE 2.x with the ESP32 board package installed
- USB-C cable
- A 2.4GHz Wi-Fi network (ESP32-S3 does not support 5GHz)

## Setup

1. Open `02_wifi_camera_stream.ino` in Arduino IDE.
2. Edit these two lines near the top with your own network credentials:
   ```cpp
   const char* WIFI_SSID = "YOUR_WIFI_SSID";
   const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
   ```
3. **Tools** menu: Board = `ESP32S3 Dev Module`, USB CDC On Boot = `Enabled`, PSRAM = `OPI PSRAM / Enabled`, Flash Size = `16MB`, Partition Scheme = `Huge APP`.

## Build and Flash

Click **Upload** in Arduino IDE.

## Expected Result

Open Serial Monitor at `115200` baud. On boot:

```
Connecting to WiFi SSID: YOUR_WIFI_SSID
.....
WiFi OK. IP: 192.168.1.42
Open: http://192.168.1.42/stream
```

Open the printed URL in a browser — you should see a live, continuously updating camera image (MJPEG stream, QVGA 320×240 by default, ~15 FPS target). The stream automatically closes after 15 seconds of connection to free up the camera; refresh the page to reconnect.

## Troubleshooting

- `WiFi FAILED` — double-check your credentials, and confirm your network is 2.4GHz. See [docs/troubleshooting.md](../../docs/troubleshooting.md#wi-fi-cannot-connect).
- Blank browser page — confirm the camera initialized (`Camera init OK.` should appear in Serial output before you open the stream URL).
- For a configurable version with adjustable resolution/FPS and a Serial command interface, see the full firmware in [firmware/proton-ai-core-vision-plus/](../../firmware/proton-ai-core-vision-plus/).
