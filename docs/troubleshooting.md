# Troubleshooting

Failure modes below are either directly implemented in the firmware (with the exact message it prints) or general, well-known ESP32-S3/Arduino IDE behavior. Nothing here is a claimed hardware defect specific to Proton AI Core Vision+.

## Board Not Detected / Upload Fails

- Confirm you're using a USB-C cable capable of data (not power-only).
- Confirm **USB CDC On Boot: Enabled** is set in Tools (this board uses the ESP32-S3's native USB for its serial console; with this setting wrong, the port may not appear as expected).
- If upload still doesn't start, hold **BOOT**, tap **RESET**, then release **BOOT** to force the board into bootloader mode before uploading (standard ESP32-S3 boot-mode procedure).
- Try a different USB port or cable.

## Serial Output Missing or Garbled

- Confirm the Serial Monitor is set to **115200 baud**.
- Confirm you selected the correct port after connecting the board.
- If output is garbled immediately after flashing, press the **RESET** button once the upload finishes.

## Camera Init Fails

Firmware prints `Camera init FAILED: 0x<code>` when `esp_camera_init()` returns an error.

- Confirm the camera FPC cable is fully seated in the connector, with contacts facing the correct direction (see [Camera Guide](camera.md)).
- Run `cam_off` then `cam_init` to force a clean re-init and I2C bus recovery.
- Confirm PSRAM is enabled in your build settings (**PSRAM: OPI PSRAM / Enabled**) — without PSRAM, the firmware falls back to a smaller DRAM frame buffer and reduced settings.

## Capture Fails ("Capture FAILED.")

- Run `cam_init` first if you haven't already.
- Try a lower resolution: `capture_res low`.
- Check available memory with `status` — low free PSRAM/heap can cause capture failures at higher resolutions.

## Blank or No MJPEG Stream

- Confirm you ran `stream_on` over Serial before opening `/stream` in a browser — the endpoint returns `403` with "stream disabled; send stream_on over Serial" otherwise.
- Confirm Wi-Fi is connected (`wifi`, then `ip` to get the board's address).
- Remember that `/stream` blocks the Serial command interface while a client is connected — this is expected behavior, not a fault. Close the browser tab or wait for the auto-timeout (`stream_timeout`, default 15000ms) to regain Serial control.

## Wi-Fi Cannot Connect

Firmware prints `WiFi FAILED.` if it can't connect within 20 seconds.

- Double-check `WIFI_SSID` / `WIFI_PASS` in the firmware source (2.4GHz networks only — ESP32-S3 does not support 5GHz Wi-Fi).
- Re-flash after correcting credentials — the firmware does not currently support setting Wi-Fi credentials over Serial at runtime.

## I2C Device Not Found (IMU / Secure Element / I/O Expander)

The `status` and `i2c` commands report per-device ACK results (`OK`, `ADDR_NACK`, `TIMEOUT`, etc.).

- If the camera was recently active, run `cam_off` first — the camera's SCCB lines share the I2C bus (GPIO17/GPIO18), and `cam_off` runs a bus recovery sequence.
- Run `i2c_reset` to manually recover the bus, then retry.

## microSD Mount Fails

Firmware prints `SD mount FAILED at all speeds.` after trying 400kHz–20MHz.

- Confirm the card is formatted (FAT32 is the most broadly compatible format for this class of card).
- Reseat the card in the push-push slot.
- Try a different microSD card — not all cards are guaranteed compatible.

## Microphone Read Fails

Firmware prints `I2S driver install FAILED.`, `I2S pin setup FAILED.`, or `I2S mic read FAILED.` depending on where the failure occurs.

- Run `mic` again — the firmware uninstalls and reinstalls the I2S driver each time it's called.
- If this persists across multiple attempts, note the exact failure message when reporting an issue (see [CONTRIBUTING.md](../CONTRIBUTING.md)).

## Reboot Loop During `cam_reboot_test`

This is an intentional stress test (`cam_reboot_test <1-20>`) that reboots the board N times, re-initializing the camera after each reboot and persisting progress in NVS (`Preferences`). If it doesn't complete, run `cam_reboot_clear` over Serial to reset the test state.

## Still Stuck?

Open an issue using the [Hardware Question](../.github/ISSUE_TEMPLATE/hardware_question.md) template, including your `status` command output and the exact error text.
