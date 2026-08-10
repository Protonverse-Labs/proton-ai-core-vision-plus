/*
  Proton AI Core Vision+ — Example: Camera Capture
  ============================================================================
  Minimal example: initialize the OV5640 camera and capture a single JPEG
  frame over Serial. Extracted from the full board firmware
  (firmware/proton-ai-core-vision-plus/), which also includes Wi-Fi
  streaming, LED control, and I2C device tests.

  Arduino IDE settings (same as the full firmware):
    Board: ESP32S3 Dev Module
    USB CDC On Boot: Enabled
    PSRAM: OPI PSRAM / Enabled
    Flash: 16MB
    Partition: Huge APP
    Serial Monitor: 115200 baud

  Serial commands:
    cam_init     - initialize the camera
    cam_capture  - capture a single JPEG frame and print its size
  ============================================================================
*/

#include <Arduino.h>
#include "esp_camera.h"

// Camera pin map — see docs/pinout.md
#define CAM_PIN_PWDN    2
#define CAM_PIN_RESET   45
#define CAM_PIN_XCLK    40
#define CAM_PIN_SIOD    17
#define CAM_PIN_SIOC    18
#define CAM_PIN_D7      39
#define CAM_PIN_D6      41
#define CAM_PIN_D5      42
#define CAM_PIN_D4      12
#define CAM_PIN_D3      3
#define CAM_PIN_D2      14
#define CAM_PIN_D1      47
#define CAM_PIN_D0      13
#define CAM_PIN_VSYNC   21
#define CAM_PIN_HREF    38
#define CAM_PIN_PCLK    11

bool cameraReady = false;

bool initCamera() {
  if (cameraReady) {
    Serial.println(F("Camera already initialized."));
    return true;
  }

  pinMode(CAM_PIN_PWDN, OUTPUT);
  digitalWrite(CAM_PIN_PWDN, LOW);
  pinMode(CAM_PIN_RESET, OUTPUT);
  digitalWrite(CAM_PIN_RESET, HIGH);
  delay(20);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = CAM_PIN_D0;
  config.pin_d1       = CAM_PIN_D1;
  config.pin_d2       = CAM_PIN_D2;
  config.pin_d3       = CAM_PIN_D3;
  config.pin_d4       = CAM_PIN_D4;
  config.pin_d5       = CAM_PIN_D5;
  config.pin_d6       = CAM_PIN_D6;
  config.pin_d7       = CAM_PIN_D7;
  config.pin_xclk     = CAM_PIN_XCLK;
  config.pin_pclk     = CAM_PIN_PCLK;
  config.pin_vsync    = CAM_PIN_VSYNC;
  config.pin_href     = CAM_PIN_HREF;
  config.pin_sccb_sda = CAM_PIN_SIOD;
  config.pin_sccb_scl = CAM_PIN_SIOC;
  config.pin_pwdn     = CAM_PIN_PWDN;
  config.pin_reset    = CAM_PIN_RESET;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode    = CAMERA_GRAB_LATEST;
  config.fb_location  = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count     = 2;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA;
  } else {
    config.frame_size = FRAMESIZE_QVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init FAILED: 0x%X\n", err);
    cameraReady = false;
    return false;
  }

  cameraReady = true;
  Serial.println(F("Camera init OK."));
  return true;
}

void captureOnce() {
  if (!cameraReady && !initCamera()) return;

  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println(F("Capture FAILED."));
    return;
  }

  Serial.printf("JPEG OK: %u bytes, %ux%u, format=%u\n", fb->len, fb->width, fb->height, fb->format);
  esp_camera_fb_return(fb);
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println();
  Serial.println(F("Proton AI Core Vision+ — Camera Capture example"));
  Serial.println(F("Commands: cam_init, cam_capture"));
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line == "cam_init") initCamera();
    else if (line == "cam_capture") captureOnce();
    else if (line.length()) {
      Serial.print(F("Unknown command: "));
      Serial.println(line);
      Serial.println(F("Commands: cam_init, cam_capture"));
    }
  }
}
