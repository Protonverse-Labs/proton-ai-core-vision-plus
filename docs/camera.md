# Camera Guide

## Sensor

Proton AI Core Vision+ uses an **OV5640** image sensor, 5MP (2592×1944 array), on a 24-pin FPC camera module (part `SF4V564088BYC-ESP` per the module datasheet). Lens: 3.0mm EFL, F2.2, 76.6° field of view, 1/4" optical format.

## Connector and Orientation

The camera module connects to the board via a 24-pin FPC cable into connector J1. Install it with the contacts facing the board (contacts and PCB connector fingers must align — check against the reference photos in [assets/board/](../assets/board/) if unsure). The camera is pre-attached on the reference hardware shown in this repository's photos.

## Firmware Initialization

The camera is driven with the ESP32 `esp_camera` driver. Initialization (`initCamera()` in the firmware) configures:
- 8-bit parallel data bus + SCCB (I2C) control (see [Pinout](pinout.md))
- JPEG output format
- Frame buffer in PSRAM (falls back to DRAM with a single QVGA frame buffer if PSRAM is not detected)
- `CAMERA_GRAB_LATEST` grab mode, 2 frame buffers when PSRAM is available

## Tested Functionality

Per the project's verified-features record and firmware testing:
- Camera detection and initialization: **tested, working**
- Single-frame JPEG capture: **tested, working**
- MJPEG streaming over Wi-Fi (browser): **tested, working**
- Firmware can be flashed successfully with camera code included: **tested, working**

## Resolution Presets

The firmware exposes four selectable presets, applied to both single-shot capture and streaming independently:

| Preset | Frame size | Resolution | Default JPEG quality |
|---|---|---|---:|
| `low` | QVGA | 320×240 | 14 |
| `medium` | VGA | 640×480 | 12 |
| `high` | SVGA | 800×600 | 10 |
| `veryhigh` | XGA | 1024×768 | 8 |

Set via Serial commands: `capture_res <preset>` (single-shot capture) and `stream_res <preset>` (streaming). The OV5640 sensor itself supports resolutions up to its native 5MP, but the firmware currently only exposes the four presets above.

**Not yet measured or published:** achieved frame rate (FPS) at each resolution/quality setting, and image-quality benchmarks. Do not assume a specific FPS until it has been measured on hardware — see [Project Status](project-status.md).

## Using the Camera

**Single JPEG capture (Serial):**
```
cam_init
cam_capture
```
Expect a line like `JPEG OK: <bytes> bytes, <width>x<height>, format=<n>` in the Serial Monitor.

**MJPEG stream (browser, requires Wi-Fi):**
```
wifi
stream_on
ip
```
Then open the printed `/stream` URL in a browser. The stream auto-closes after `stream_timeout` milliseconds (default 15000ms) — the firmware's own comments note that `/stream` blocks Serial command handling while a browser is connected, so a short timeout is recommended for interactive use.

**Single JPEG over HTTP:** open `http://<board-ip>/jpg` in a browser once Wi-Fi is connected.

## Known Limitations

- The camera's SCCB control lines share GPIO17/GPIO18 with the board's main I2C bus. The firmware's `cam_off` command deinitializes the camera and runs an I2C bus recovery sequence before other I2C devices (IMU, secure element, expander) can be reliably tested.
- While `/stream` is open in a browser, the Serial command interface is blocked until the client disconnects or the stream times out.
- No on-device object/image AI inference is implemented in the current firmware — see [Project Status](project-status.md).

## Troubleshooting

See [Troubleshooting](troubleshooting.md#camera) for camera-specific failure modes (init failure, blank stream, capture failure).
