# Example: Status LED Control

Drives the onboard addressable RGB status LED (WS2812B) and the 1W LED over Serial commands, with no camera or Wi-Fi involved.

## Purpose

A focused starting point for status/indicator logic — useful for testing the LEDs in isolation, or as a base for your own status-signaling code.

## Requirements

- Proton AI Core Vision+ board
- Arduino IDE 2.x with the ESP32 board package installed
- **Adafruit NeoPixel** library (Library Manager)
- USB-C cable

## Setup

1. Open `03_status_led.ino` in Arduino IDE.
2. **Tools** menu: Board = `ESP32S3 Dev Module`, USB CDC On Boot = `Enabled`. PSRAM/Flash/Partition settings don't affect this example, but the values used for the full firmware (`16MB` / `Huge APP`) are safe defaults.

## Build and Flash

Click **Upload** in Arduino IDE.

## Expected Result

Open Serial Monitor at `115200` baud. Try:

```
rgb 0 80 0
```
The RGB LED turns green.

```
led pwm
led level 128
```
The 1W LED turns on at roughly half brightness.

```
led blink
```
The 1W LED blinks 5 times.

## Troubleshooting

- No LED response — confirm the `Adafruit_NeoPixel` library is installed (Library Manager) and that the sketch compiled without errors.
- Unexpected colors — the RGB LED uses GRB color order (handled internally by the `NEO_GRB` flag); if colors look swapped, double-check your `rgb <r> <g> <b>` argument order.
