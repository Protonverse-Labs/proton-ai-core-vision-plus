/*
  Proton AI Core Vision+ — Example: Wi-Fi Camera Stream
  ============================================================================
  Minimal example: connect to Wi-Fi and serve an MJPEG camera stream over
  HTTP. Extracted from the full board firmware
  (firmware/proton-ai-core-vision-plus/), which also includes LED control,
  I2C device tests, and configurable resolution presets.

  Arduino IDE settings (same as the full firmware):
    Board: ESP32S3 Dev Module
    USB CDC On Boot: Enabled
    PSRAM: OPI PSRAM / Enabled
    Flash: 16MB
    Partition: Huge APP
    Serial Monitor: 115200 baud

  WiFi:
    Replace WIFI_SSID / WIFI_PASS below with your own 2.4GHz network.

  Usage:
    1. Flash this sketch and open the Serial Monitor.
    2. Wait for "WiFi OK" and the printed IP address.
    3. Open http://<printed-ip>/stream in a browser.
  ============================================================================
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "esp_camera.h"

const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";

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

WebServer server(80);
bool cameraReady = false;
uint8_t streamTargetFps = 15;
uint32_t streamMaxMs = 15000;

bool initCamera() {
  if (cameraReady) return true;

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
  config.jpeg_quality = 14;
  config.fb_count     = 2;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_QVGA;
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

void handleStream() {
  if (!cameraReady && !initCamera()) {
    server.send(500, "text/plain", "camera init failed");
    return;
  }

  WiFiClient client = server.client();

  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n";
  response += "Connection: close\r\n\r\n";
  client.print(response);

  Serial.println(F("MJPEG client connected."));

  uint32_t startMs = millis();
  uint32_t delayMs = 1000UL / max((uint8_t)1, streamTargetFps);

  while (client.connected() && (millis() - startMs < streamMaxMs)) {
    uint32_t frameStart = millis();

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) break;

    client.printf("--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", fb->len);
    client.write(fb->buf, fb->len);
    client.print("\r\n");

    esp_camera_fb_return(fb);

    uint32_t used = millis() - frameStart;
    if (used < delayMs) delay(delayMs - used);
  }

  client.stop();
  Serial.println(F("MJPEG client disconnected/timeout."));
}

void connectWiFi() {
  Serial.printf("Connecting to WiFi SSID: %s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 20000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print(F("WiFi OK. IP: "));
    Serial.println(WiFi.localIP());
    server.on("/stream", HTTP_GET, []() { handleStream(); });
    server.begin();
    Serial.print(F("Open: http://"));
    Serial.print(WiFi.localIP());
    Serial.println(F("/stream"));
  } else {
    Serial.println(F("WiFi FAILED. Check WIFI_SSID/WIFI_PASS and that the network is 2.4GHz."));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println();
  Serial.println(F("Proton AI Core Vision+ — Wi-Fi Camera Stream example"));
  connectWiFi();
}

void loop() {
  server.handleClient();
}
