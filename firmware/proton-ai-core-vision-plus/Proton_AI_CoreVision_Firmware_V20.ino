// SPDX-License-Identifier: MIT

/*
  ============================================================================
  Proton AI Core-Vision - Standalone V20 Firmware
  ============================================================================
  Board:
    ESP32-S3-WROOM-1-N16R8
    OV5640 camera
    WS2812B RGB LED
    1W LED on GPIO48
    ICM-42670-P IMU
    ATECC608B crypto chip
    PCA9685 expander
    ICS-43434 I2S microphone
    SPI microSD

  Arduino IDE:
    Board: ESP32S3 Dev Module
    USB CDC On Boot: Enabled
    PSRAM: OPI PSRAM / Enabled
    Flash: 16MB
    Partition: Huge APP
    Serial Monitor: 115200 baud, Newline or Both NL & CR

  WiFi:
    SSID: YOUR_SSID
    PASS: YOUR_PASSWORD

  ============================================================================
  FULL COMMAND LIST
  ============================================================================

  Basic:
    help
    status
    pins
    reboot

  I2C:
    i2c
    i2c_reset
    pca_scan
    imu
    crypto
    atecc
    secure

  WiFi:
    wifi
    ip

  Camera:
    cam_init
    cam_capture
    stream_on
    stream_off
    cam_off
    camera_off

  Capture resolution:
    capture_res low
    capture_res medium
    capture_res high
    capture_res veryhigh
    capture_low
    capture_medium
    capture_high
    capture_veryhigh
    jpg_low
    jpg_medium
    jpg_high
    jpg_veryhigh

  Stream resolution:
    stream_res low
    stream_res medium
    stream_res high
    stream_res veryhigh
    stream_low
    stream_medium
    stream_high
    stream_veryhigh

  Stream FPS / timeout:
    stream_fps 1..30
    stream_timeout <ms>

  Validation / stress:
    cam_status
    res_status
    cam_reboot_test <1-20>
    cam_reboot_clear
    led_cam_test

  LEDs:
    rgb <r> <g> <b>
    led on
    led off
    led blink
    led pwm
    led level <0-255>

  Other:
    sd
    sd_test
    mic

  Notes:
    - /stream blocks Serial command handling while the browser is connected.
      Use stream_timeout 5000 so the stream closes automatically.
    - Camera SCCB shares GPIO17/GPIO18 with I2C. cam_off and I2C commands
      recover the bus before testing I2C devices.
  ============================================================================
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Preferences.h>
#include "esp_camera.h"
#include "driver/i2s.h"
#include <Adafruit_NeoPixel.h>

const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";



// ============================================================================
// Board pin map
// ============================================================================
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

#define I2C_SDA_PIN     17
#define I2C_SCL_PIN     18

#define RGB_LED_PIN     1
#define RGB_LED_COUNT   1
#define LED_1W_PIN      48

#define I2S_BCLK_PIN    16
#define I2S_DATA_PIN    8
#define I2S_WS_PIN      9

#define SD_CS_PIN       5
#define SD_MOSI_PIN     6
#define SD_CLK_PIN      15
#define SD_MISO_PIN     10

#define ADDR_PCA9685    0x40
#define ADDR_ATECC608B  0x60
#define ADDR_ICM42670_A 0x68
#define ADDR_ICM42670_B 0x69


WebServer server(80);
Adafruit_NeoPixel rgbLed(RGB_LED_COUNT, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);
Preferences prefs;

bool cameraReady = false;
bool wifiReady = false;
bool streamEnabled = false;
bool streamClientActive = false;
bool i2sMicInstalled = false;

framesize_t captureFrameSize = FRAMESIZE_VGA;
int captureJpegQuality = 12;
framesize_t streamFrameSize = FRAMESIZE_QVGA;
int streamJpegQuality = 14;
uint8_t streamTargetFps = 15;
uint32_t streamMaxMs = 15000;

uint32_t streamFrameCounter = 0;
uint32_t streamStartHeap = 0;
uint32_t streamStartPsram = 0;

// Forward declarations

bool i2cPresent(uint8_t addr);
uint8_t i2cAckCode(uint8_t addr);
const char* i2cAckText(uint8_t err);
uint8_t i2cRead8(uint8_t addr, uint8_t reg);
void ateccWake();
uint8_t ateccAckWithWake(uint8_t retries = 5);
const char* frameSizeName(framesize_t fs);
bool parseResolutionMode(String mode, framesize_t &fs, int &quality);
void applyCameraSettings(framesize_t fs, int quality);
void printHelp();
void printPins();
void printStatus();
void connectWiFi();
void startServer();
bool initCamera();
void captureOnce();
void stopCameraAndRecoverI2C();
void setCaptureResolution(String mode);
void setStreamResolution(String mode);
void setStreamTargetFps(uint8_t fps);
void printCameraResolutionStatus();
void scanI2C();
void i2cBusRecover();
void rgbSet(uint8_t r, uint8_t g, uint8_t b);
void ledOnDirect();
void ledOffDirect();
void ledBlinkTest();
void ledPwmMode();
void ledPwmWrite(uint8_t duty);
void ledCameraTest();
void testSD();
void testIMU();
void testMic();
void startCameraRebootTest(uint8_t cycles);
void clearCameraRebootTest();
void continueCameraRebootTestIfNeeded();
void handleCommand(String line);
void handleJpg();
void handleStream();
void printMemoryLine(const char* label);


void setup() {
  Serial.begin(115200);
  delay(1500);

  Serial.println();
  Serial.println(F("Proton AI Core-Vision ESP32-S3 standalone firmware v20"));

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, 100000);
  Wire.setTimeOut(50);

  rgbLed.begin();
  rgbLed.setBrightness(40);
  rgbSet(0, 0, 20);

  pinMode(LED_1W_PIN, OUTPUT);
  ledOffDirect();

  pinMode(CAM_PIN_PWDN, OUTPUT);
  digitalWrite(CAM_PIN_PWDN, LOW);
  pinMode(CAM_PIN_RESET, OUTPUT);
  digitalWrite(CAM_PIN_RESET, HIGH);

  continueCameraRebootTestIfNeeded();

  printHelp();
  printPins();
  printStatus();
}

void loop() {
  server.handleClient();

  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    handleCommand(line);
  }
}

// ============================================================================
// I2C helpers and recovery
// ============================================================================
bool i2cPresent(uint8_t addr) {
  Wire.beginTransmission(addr);
  return Wire.endTransmission() == 0;
}

uint8_t i2cAckCode(uint8_t addr) {
  Wire.beginTransmission(addr);
  return Wire.endTransmission();
}

const char* i2cAckText(uint8_t err) {
  switch (err) {
    case 0: return "OK";
    case 1: return "DATA_TOO_LONG";
    case 2: return "ADDR_NACK";
    case 3: return "DATA_NACK";
    case 4: return "OTHER_ERROR";
    case 5: return "TIMEOUT";
    default: return "UNKNOWN";
  }
}

uint8_t i2cRead8(uint8_t addr, uint8_t reg) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return 0xFF;
  Wire.requestFrom((int)addr, 1);
  if (!Wire.available()) return 0xFF;
  return Wire.read();
}

void i2cBusRecover() {
  Wire.end();
  delay(5);

  pinMode(I2C_SDA_PIN, INPUT_PULLUP);
  pinMode(I2C_SCL_PIN, OUTPUT_OPEN_DRAIN);
  digitalWrite(I2C_SCL_PIN, HIGH);
  delayMicroseconds(10);

  for (uint8_t i = 0; i < 9; i++) {
    digitalWrite(I2C_SCL_PIN, LOW);
    delayMicroseconds(5);
    digitalWrite(I2C_SCL_PIN, HIGH);
    delayMicroseconds(5);
  }

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, 100000);
  Wire.setTimeOut(50);
  delay(10);
}

void ateccWake() {
  Wire.setClock(100000);
  Wire.beginTransmission(0x00);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(3);
}

uint8_t ateccAckWithWake(uint8_t retries) {
  uint8_t err = 255;
  for (uint8_t i = 0; i < retries; i++) {
    ateccWake();
    Wire.beginTransmission(ADDR_ATECC608B);
    err = Wire.endTransmission();
    if (err == 0) return 0;
    delay(10);
  }
  return err;
}

// ============================================================================
// LEDs
// ============================================================================
void rgbSet(uint8_t r, uint8_t g, uint8_t b) {
  rgbLed.setPixelColor(0, rgbLed.Color(r, g, b));
  rgbLed.show();
}

void ledOnDirect() {
  ledcDetach(LED_1W_PIN);
  pinMode(LED_1W_PIN, OUTPUT);
  digitalWrite(LED_1W_PIN, HIGH);
}

void ledOffDirect() {
  ledcDetach(LED_1W_PIN);
  pinMode(LED_1W_PIN, OUTPUT);
  digitalWrite(LED_1W_PIN, LOW);
}

void ledPwmMode() {
  ledcAttach(LED_1W_PIN, 1000, 8);
}

void ledPwmWrite(uint8_t duty) {
  ledcWrite(LED_1W_PIN, duty);
}

void ledBlinkTest() {
  ledOffDirect();
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_1W_PIN, HIGH);
    delay(300);
    digitalWrite(LED_1W_PIN, LOW);
    delay(300);
  }
}

void ledCameraTest() {
  Serial.println(F("LED + camera combined test starting..."));
  if (!cameraReady && !initCamera()) return;

  rgbSet(0, 0, 80);
  ledOnDirect();
  delay(150);

  for (uint8_t i = 0; i < 5; i++) {
    rgbSet(80, 0, 0);
    captureOnce();
    rgbSet(0, 80, 0);
    delay(250);
  }

  ledOffDirect();
  rgbSet(0, 0, 20);
  Serial.println(F("LED + camera combined test finished."));
}

// ============================================================================
// Camera resolution, FPS, and memory helpers
// ============================================================================
const char* frameSizeName(framesize_t fs) {
  switch (fs) {
    case FRAMESIZE_QQVGA: return "LOW/QQVGA 160x120";
    case FRAMESIZE_QVGA:  return "LOW/QVGA 320x240";
    case FRAMESIZE_VGA:   return "MEDIUM/VGA 640x480";
    case FRAMESIZE_SVGA:  return "HIGH/SVGA 800x600";
    case FRAMESIZE_XGA:   return "VERY_HIGH/XGA 1024x768";
    case FRAMESIZE_SXGA:  return "VERY_HIGH/SXGA 1280x1024";
    case FRAMESIZE_UXGA:  return "VERY_HIGH/UXGA 1600x1200";
    default: return "CUSTOM";
  }
}

bool parseResolutionMode(String mode, framesize_t &fs, int &quality) {
  mode.trim();
  mode.toLowerCase();

  if (mode == "low") {
    fs = FRAMESIZE_QVGA;
    quality = 14;
    return true;
  }
  if (mode == "medium" || mode == "med") {
    fs = FRAMESIZE_VGA;
    quality = 12;
    return true;
  }
  if (mode == "high") {
    fs = FRAMESIZE_SVGA;
    quality = 10;
    return true;
  }
  if (mode == "veryhigh" || mode == "very_high" || mode == "very high" || mode == "vh") {
    fs = FRAMESIZE_XGA;
    quality = 8;
    return true;
  }

  return false;
}

void applyCameraSettings(framesize_t fs, int quality) {
  sensor_t *s = esp_camera_sensor_get();
  if (s) {
    s->set_framesize(s, fs);
    s->set_quality(s, quality);
    delay(100);
    Serial.printf("Camera active settings: %s, JPEG quality=%d\n", frameSizeName(fs), quality);
  }
}

void setCaptureResolution(String mode) {
  framesize_t fs;
  int q;
  if (!parseResolutionMode(mode, fs, q)) {
    Serial.println(F("Use: capture_res low | medium | high | veryhigh"));
    return;
  }

  captureFrameSize = fs;
  captureJpegQuality = q;
  Serial.printf("Capture resolution selected: %s, JPEG quality=%d\n", frameSizeName(captureFrameSize), captureJpegQuality);
}

void setStreamResolution(String mode) {
  framesize_t fs;
  int q;
  if (!parseResolutionMode(mode, fs, q)) {
    Serial.println(F("Use: stream_res low | medium | high | veryhigh"));
    return;
  }

  streamFrameSize = fs;
  streamJpegQuality = q;
  Serial.printf("Stream resolution selected: %s, JPEG quality=%d\n", frameSizeName(streamFrameSize), streamJpegQuality);
}

void setStreamTargetFps(uint8_t fps) {
  streamTargetFps = constrain(fps, 1, 30);
  Serial.printf("Stream target FPS set to %u\n", streamTargetFps);
}

void printCameraResolutionStatus() {
  Serial.printf("Capture: %s, JPEG quality=%d\n", frameSizeName(captureFrameSize), captureJpegQuality);
  Serial.printf("Stream : %s, JPEG quality=%d, target FPS=%u\n", frameSizeName(streamFrameSize), streamJpegQuality, streamTargetFps);
  Serial.printf("Stream auto-timeout: %lu ms\n", (unsigned long)streamMaxMs);
}

void printMemoryLine(const char* label) {
  Serial.printf("%s heap=%u psram=%u minHeap=%u\n",
                label,
                ESP.getFreeHeap(),
                ESP.getFreePsram(),
                ESP.getMinFreeHeap());
}

// ============================================================================
// Camera init/capture/shutdown
// ============================================================================
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
  config.jpeg_quality = captureJpegQuality;
  config.fb_count     = 2;

  if (psramFound()) {
    config.frame_size = captureFrameSize;
  } else {
    config.frame_size = FRAMESIZE_QVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.fb_count = 1;
  }

  printMemoryLine("Before camera init:");

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init FAILED: 0x%X\n", err);
    cameraReady = false;
    return false;
  }

  sensor_t *s = esp_camera_sensor_get();
  if (s) {
    Serial.printf("Camera detected. PID=0x%04X VER=0x%02X MIDH=0x%02X MIDL=0x%02X\n",
                  s->id.PID, s->id.VER, s->id.MIDH, s->id.MIDL);
    s->set_framesize(s, captureFrameSize);
    s->set_quality(s, captureJpegQuality);
  }

  cameraReady = true;
  Serial.println(F("Camera init OK."));
  printMemoryLine("After camera init:");
  return true;
}

void captureOnce() {
  if (!cameraReady && !initCamera()) return;
  applyCameraSettings(captureFrameSize, captureJpegQuality);

  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println(F("Capture FAILED."));
    return;
  }

  Serial.printf("JPEG OK: %u bytes, %ux%u, format=%u\n", fb->len, fb->width, fb->height, fb->format);
  esp_camera_fb_return(fb);
}

void stopCameraAndRecoverI2C() {
  Serial.println(F("CAMERA OFF requested."));
  if (streamClientActive) Serial.println(F("Stream client is active; close browser tab or wait for stream auto-timeout."));

  streamEnabled = false;

  WiFiClient client = server.client();
  if (client) client.stop();

  delay(200);

  if (cameraReady) {
    Serial.println(F("Deinitializing camera driver..."));
    esp_err_t err = esp_camera_deinit();
    Serial.printf("esp_camera_deinit result: 0x%X\n", err);
    cameraReady = false;
  } else {
    Serial.println(F("Camera was not initialized."));
  }

  pinMode(CAM_PIN_PWDN, OUTPUT);
  digitalWrite(CAM_PIN_PWDN, HIGH);
  pinMode(CAM_PIN_RESET, OUTPUT);
  digitalWrite(CAM_PIN_RESET, LOW);
  delay(100);

  i2cBusRecover();
  printMemoryLine("After camera off:");
  Serial.println(F("Camera OFF done. Now test: i2c, crypto, imu"));
}

// ============================================================================
// Reboot camera init stability test
// ============================================================================
void startCameraRebootTest(uint8_t cycles) {
  cycles = constrain(cycles, 1, 20);

  prefs.begin("pcv", false);
  prefs.putUChar("rb_total", cycles);
  prefs.putUChar("rb_left", cycles);
  prefs.end();

  Serial.printf("Starting camera reboot stability test: %u cycles\n", cycles);
  Serial.println(F("Device will reboot now and auto-test camera init after each reboot."));
  delay(500);
  ESP.restart();
}

void clearCameraRebootTest() {
  prefs.begin("pcv", false);
  prefs.remove("rb_total");
  prefs.remove("rb_left");
  prefs.end();
  Serial.println(F("Camera reboot test state cleared."));
}

void continueCameraRebootTestIfNeeded() {
  prefs.begin("pcv", false);
  uint8_t total = prefs.getUChar("rb_total", 0);
  uint8_t left  = prefs.getUChar("rb_left", 0);

  if (total == 0 || left == 0) {
    prefs.end();
    return;
  }

  Serial.printf("Camera reboot test active: cycle %u of %u\n", (uint8_t)(total - left + 1), total);

  bool ok = initCamera();
  if (ok) captureOnce();

  left--;
  prefs.putUChar("rb_left", left);
  prefs.end();

  if (left > 0) {
    Serial.printf("Camera init test OK. Rebooting for next cycle. Remaining=%u\n", left);
    delay(1000);
    ESP.restart();
  } else {
    clearCameraRebootTest();
    Serial.println(F("Camera reboot stability test COMPLETE."));
  }
}

// ============================================================================
// HTTP server
// ============================================================================
void handleJpg() {
  if (!cameraReady && !initCamera()) {
    server.send(500, "text/plain", "camera init failed");
    return;
  }

  applyCameraSettings(captureFrameSize, captureJpegQuality);

  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    server.send(500, "text/plain", "capture failed");
    return;
  }

  server.sendHeader("Content-Disposition", "inline; filename=capture.jpg");
  server.setContentLength(fb->len);
  server.send(200, "image/jpeg", "");
  server.client().write(fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

void handleStream() {
  if (!streamEnabled) {
    server.send(403, "text/plain", "stream disabled; send stream_on over Serial");
    return;
  }

  if (!cameraReady && !initCamera()) {
    server.send(500, "text/plain", "camera init failed");
    return;
  }

  applyCameraSettings(streamFrameSize, streamJpegQuality);

  WiFiClient client = server.client();
  streamClientActive = true;
  streamFrameCounter = 0;
  streamStartHeap = ESP.getFreeHeap();
  streamStartPsram = ESP.getFreePsram();

  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n";
  response += "Connection: close\r\n\r\n";
  client.print(response);

  Serial.printf("MJPEG client connected. Resolution=%s. Target FPS=%u. Auto-close in %lu ms.\n",
                frameSizeName(streamFrameSize),
                streamTargetFps,
                (unsigned long)streamMaxMs);

  printMemoryLine("Stream start memory:");

  uint32_t startMs = millis();
  uint32_t lastStatsMs = startMs;
  uint32_t delayMs = 1000UL / max((uint8_t)1, streamTargetFps);

  while (client.connected() && streamEnabled && (millis() - startMs < streamMaxMs)) {
    uint32_t frameStart = millis();

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) break;

    client.printf("--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", fb->len);
    client.write(fb->buf, fb->len);
    client.print("\r\n");

    esp_camera_fb_return(fb);
    streamFrameCounter++;

    if (millis() - lastStatsMs >= 2000) {
      float elapsed = (millis() - startMs) / 1000.0f;
      float fps = elapsed > 0 ? (streamFrameCounter / elapsed) : 0;
      Serial.printf("Stream stats: frames=%lu fps=%.2f heap=%u psram=%u heapDelta=%ld psramDelta=%ld\n",
                    (unsigned long)streamFrameCounter,
                    fps,
                    ESP.getFreeHeap(),
                    ESP.getFreePsram(),
                    (long)ESP.getFreeHeap() - (long)streamStartHeap,
                    (long)ESP.getFreePsram() - (long)streamStartPsram);
      lastStatsMs = millis();
    }

    uint32_t used = millis() - frameStart;
    if (used < delayMs) delay(delayMs - used);
  }

  client.stop();
  streamClientActive = false;

  float elapsed = (millis() - startMs) / 1000.0f;
  float fps = elapsed > 0 ? (streamFrameCounter / elapsed) : 0;

  Serial.printf("MJPEG client disconnected/timeout. frames=%lu avgFPS=%.2f\n",
                (unsigned long)streamFrameCounter,
                fps);
  printMemoryLine("Stream end memory:");
  Serial.println(F("Serial commands are active now."));
}

void startServer() {
  server.on("/", []() {
    server.send(200, "text/html",
      "<html><body><h2>Proton AI Core-Vision</h2>"
      "<p><a href='/jpg'>Single JPEG</a></p>"
      "<p><a href='/stream'>MJPEG Stream</a></p>"
      "</body></html>");
  });

  server.on("/jpg", HTTP_GET, []() { handleJpg(); });
  server.on("/stream", HTTP_GET, []() { handleStream(); });
  server.begin();

  Serial.println(F("HTTP server started."));
}

void connectWiFi() {
  if (wifiReady && WiFi.status() == WL_CONNECTED) {
    Serial.println(F("WiFi already connected."));
    return;
  }

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
    wifiReady = true;
    Serial.print(F("WiFi OK. IP: "));
    Serial.println(WiFi.localIP());
    startServer();
    Serial.print(F("Open http://"));
    Serial.print(WiFi.localIP());
    Serial.println(F("/"));
  } else {
    wifiReady = false;
    Serial.println(F("WiFi FAILED."));
  }
}

// ============================================================================
// Device tests
// ============================================================================
void scanI2C() {
  i2cBusRecover();
  Serial.println(F("Scanning I2C..."));

  uint8_t count = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    if (i2cPresent(addr)) {
      Serial.printf("  Found 0x%02X", addr);
      if (addr == ADDR_PCA9685) Serial.print(F(" PCA9685"));
      if (addr == ADDR_ATECC608B) Serial.print(F(" ATECC608B"));
      if (addr == ADDR_ICM42670_A || addr == ADDR_ICM42670_B) Serial.print(F(" ICM-42670-P"));
      Serial.println();
      count++;
    }
  }

  Serial.printf("I2C scan done. Devices=%u\n", count);
}

void testIMU() {
  i2cBusRecover();

  uint8_t addr = i2cPresent(ADDR_ICM42670_A) ? ADDR_ICM42670_A :
                 (i2cPresent(ADDR_ICM42670_B) ? ADDR_ICM42670_B : 0);

  if (!addr) {
    Serial.println(F("ICM-42670-P not found at 0x68 or 0x69."));
    return;
  }

  uint8_t who = i2cRead8(addr, 0x75);
  Serial.printf("ICM-42670-P found at 0x%02X, WHO_AM_I register 0x75 = 0x%02X\n", addr, who);
}

void testSD() {
  Serial.println(F("Testing microSD card..."));
  Serial.println(F("Pin map: CS=GPIO5, MOSI=GPIO6, CLK=GPIO15, MISO=GPIO10"));
  Serial.println(F("Trying SPI speeds: 400kHz, 1MHz, 4MHz, 20MHz"));

  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);
  delay(20);

  SPI.end();
  delay(20);
  SPI.begin(SD_CLK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);

  const uint32_t speeds[] = {400000, 1000000, 4000000, 20000000};
  bool mounted = false;
  uint32_t goodSpeed = 0;

  for (uint8_t i = 0; i < sizeof(speeds) / sizeof(speeds[0]); i++) {
    Serial.printf("  Trying SD.begin at %lu Hz... ", (unsigned long)speeds[i]);
    if (SD.begin(SD_CS_PIN, SPI, speeds[i])) {
      mounted = true;
      goodSpeed = speeds[i];
      Serial.println(F("OK"));
      break;
    } else {
      Serial.println(F("FAILED"));
      SD.end();
      delay(100);
    }
  }

  if (!mounted) {
    Serial.println(F("SD mount FAILED at all speeds."));
    Serial.println(F("Check card format, socket soldering, VDD=3.3V, CS/CLK/MOSI/MISO continuity."));
    return;
  }

  uint8_t type = SD.cardType();
  uint64_t sizeMB = SD.cardSize() / (1024ULL * 1024ULL);

  Serial.printf("SD OK at %lu Hz. Type=%u Size=%llu MB\n", (unsigned long)goodSpeed, type, sizeMB);

  File f = SD.open("/bringup.txt", FILE_WRITE);
  if (f) {
    f.println("Proton AI Core-Vision SD test OK");
    f.close();
    Serial.println(F("Wrote /bringup.txt OK"));
  } else {
    Serial.println(F("Could not write /bringup.txt"));
  }
}

void testMic() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 256,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK_PIN,
    .ws_io_num = I2S_WS_PIN,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_DATA_PIN
  };

  if (i2sMicInstalled) {
    i2s_driver_uninstall(I2S_NUM_0);
    i2sMicInstalled = false;
  }

  if (i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL) != ESP_OK) {
    Serial.println(F("I2S driver install FAILED."));
    return;
  }
  i2sMicInstalled = true;

  if (i2s_set_pin(I2S_NUM_0, &pin_config) != ESP_OK) {
    Serial.println(F("I2S pin setup FAILED."));
    return;
  }

  int32_t samples[256];
  size_t bytesRead = 0;
  esp_err_t err = i2s_read(I2S_NUM_0, samples, sizeof(samples), &bytesRead, 1000 / portTICK_PERIOD_MS);

  if (err != ESP_OK || bytesRead == 0) {
    Serial.println(F("I2S mic read FAILED."));
    return;
  }

  uint32_t n = bytesRead / sizeof(int32_t);
  uint64_t acc = 0;

  for (uint32_t i = 0; i < n; i++) {
    int32_t v = samples[i] >> 14;
    acc += abs(v);
  }

  Serial.printf("I2S mic OK. Samples=%u average_abs_level=%llu\n", n, acc / n);
}

// ============================================================================
// Status and help
// ============================================================================
void printStatus() {
  i2cBusRecover();

  Serial.println(F("==== Status ===="));
  Serial.printf("PSRAM: %s, heap=%u, psram=%u, minHeap=%u\n",
                psramFound() ? "YES" : "NO",
                ESP.getFreeHeap(),
                ESP.getFreePsram(),
                ESP.getMinFreeHeap());

  Serial.printf("WiFi: %s", WiFi.status() == WL_CONNECTED ? "CONNECTED " : "OFF/FAILED");
  if (WiFi.status() == WL_CONNECTED) Serial.print(WiFi.localIP());
  Serial.println();

  Serial.printf("Camera: %s\n", cameraReady ? "READY" : "NOT INIT");
  Serial.printf("Stream: %s\n", streamEnabled ? "ENABLED" : "DISABLED");
  printCameraResolutionStatus();

  uint8_t pcaErr    = i2cAckCode(ADDR_PCA9685);
  uint8_t ateccErr  = ateccAckWithWake(5);
  uint8_t imu69Err  = i2cAckCode(ADDR_ICM42670_B);

  Serial.println(F("I2C status using raw ACK check:"));
  Serial.printf("  PCA9685    0x40: %s, code=%u\n", i2cAckText(pcaErr), pcaErr);
  Serial.printf("  ATECC608B  0x60: %s, code=%u\n", i2cAckText(ateccErr), ateccErr);
  Serial.printf("  ICM42670   0x69: %s, code=%u\n", i2cAckText(imu69Err), imu69Err);
}

void printPins() {
  Serial.println(F("Camera: PWDN=2 RESET=45 XCLK=40 SIOD/SDA=17 SIOC/SCL=18"));
  Serial.println(F("Camera data: Y2/D2=13 Y3/D3=47 Y4/D4=14 Y5/D5=3 Y6/D6=12 Y7/D7=42 Y8/D8=41 Y9/D9=39"));
  Serial.println(F("Camera sync: VSYNC=21 HREF=38 PCLK=11"));
  Serial.println(F("RGB WS2812: GPIO1"));
  Serial.println(F("1W LED MOSFET gate: GPIO48"));
  Serial.println(F("I2S mic: BCLK=16 DATA=8 WS=9"));
  Serial.println(F("SD SPI: CS=5 MOSI=6 CLK=15 MISO=10"));
}

void printHelp() {
  Serial.println();
  Serial.println(F("==== Proton AI Core-Vision Commands ===="));
  Serial.println(F("Basic: help, status, pins, reboot"));
  Serial.println(F("I2C: i2c, i2c_reset, pca_scan, imu, crypto, atecc, secure"));
  Serial.println(F("WiFi: wifi, ip"));
  Serial.println(F("Camera: cam_init, cam_capture, stream_on, stream_off, cam_off, camera_off"));
  Serial.println(F("Capture resolution: capture_res low | medium | high | veryhigh"));
  Serial.println(F("Stream resolution: stream_res low | medium | high | veryhigh"));
  Serial.println(F("Stream FPS: stream_fps <1-30>"));
  Serial.println(F("Stream timeout: stream_timeout <ms>"));
  Serial.println(F("Resolution/status: cam_status or res_status"));
  Serial.println(F("Stress: cam_reboot_test <1-20>, cam_reboot_clear, led_cam_test"));
  Serial.println(F("LEDs: rgb <r> <g> <b>, led on, led off, led blink, led pwm, led level <0-255>"));
  Serial.println(F("Other: sd, mic"));
  Serial.println(F("Note: while /stream is open, Serial waits until timeout or browser close."));
  Serial.println();
}

// ============================================================================
// Serial command parser
// ============================================================================
void handleCommand(String line) {
  line.trim();
  if (!line.length()) return;

  Serial.print(F("> "));
  Serial.println(line);

  if (line == "stream_off" || line == "cam_off" || line == "camera_off") {
    stopCameraAndRecoverI2C();
    return;
  }

  if (line == "cam_status" || line == "res_status") {
    printCameraResolutionStatus();
    printMemoryLine("Memory:");
    return;
  }

  if (line.startsWith("capture_res ")) {
    setCaptureResolution(line.substring(12));
    return;
  }
  if (line.startsWith("stream_res ")) {
    setStreamResolution(line.substring(11));
    return;
  }
  if (line.startsWith("stream_fps ")) {
    setStreamTargetFps((uint8_t)line.substring(11).toInt());
    return;
  }
  if (line.startsWith("stream_timeout ")) {
    streamMaxMs = max((uint32_t)1000, (uint32_t)line.substring(15).toInt());
    Serial.printf("Stream auto-timeout set to %lu ms\n", (unsigned long)streamMaxMs);
    return;
  }
  if (line.startsWith("cam_reboot_test ")) {
    startCameraRebootTest((uint8_t)line.substring(16).toInt());
    return;
  }

  if (line == "cam_reboot_clear") { clearCameraRebootTest(); return; }
  if (line == "led_cam_test") { ledCameraTest(); return; }

  if (line == "capture_low" || line == "jpg_low") { setCaptureResolution("low"); return; }
  if (line == "capture_medium" || line == "jpg_medium") { setCaptureResolution("medium"); return; }
  if (line == "capture_high" || line == "jpg_high") { setCaptureResolution("high"); return; }
  if (line == "capture_veryhigh" || line == "jpg_veryhigh") { setCaptureResolution("veryhigh"); return; }

  if (line == "stream_low") { setStreamResolution("low"); return; }
  if (line == "stream_medium") { setStreamResolution("medium"); return; }
  if (line == "stream_high") { setStreamResolution("high"); return; }
  if (line == "stream_veryhigh") { setStreamResolution("veryhigh"); return; }

  if (line == "help") printHelp();
  else if (line == "status") printStatus();
  else if (line == "pins") printPins();
  else if (line == "i2c") scanI2C();
  else if (line == "i2c_reset") {
    i2cBusRecover();
    Serial.println(F("I2C bus recovered/restarted on GPIO17/GPIO18 at 100kHz."));
  }
  else if (line == "reboot") {
    Serial.println(F("Restarting..."));
    delay(200);
    ESP.restart();
  }
  else if (line == "wifi") connectWiFi();
  else if (line == "ip") {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print(F("Open: http://")); Serial.print(WiFi.localIP()); Serial.println(F("/"));
      Serial.print(F("JPEG: http://")); Serial.print(WiFi.localIP()); Serial.println(F("/jpg"));
      Serial.print(F("Stream: http://")); Serial.print(WiFi.localIP()); Serial.println(F("/stream"));
    } else Serial.println(F("WiFi not connected."));
  }
  else if (line == "cam_init") initCamera();
  else if (line == "cam_capture") captureOnce();
  else if (line == "stream_on") {
    if (!cameraReady) initCamera();
    applyCameraSettings(streamFrameSize, streamJpegQuality);
    streamEnabled = true;
    Serial.printf("MJPEG stream enabled: %s @ target %u FPS\n", frameSizeName(streamFrameSize), streamTargetFps);
    Serial.println(F("Open /stream in browser. Close browser or wait timeout before using cam_off."));
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print(F("Open: http://")); Serial.print(WiFi.localIP()); Serial.println(F("/stream"));
    }
  }
  else if (line == "led on") {
    ledOnDirect();
    Serial.println(F("1W LED direct GPIO ON. GPIO48 should be HIGH."));
  }
  else if (line == "led off") {
    ledOffDirect();
    Serial.println(F("1W LED direct GPIO OFF. GPIO48 should be LOW."));
  }
  else if (line == "led blink") {
    Serial.println(F("Blinking 1W LED 5 times using direct GPIO..."));
    ledBlinkTest();
    Serial.println(F("Blink done."));
  }
  else if (line == "led pwm") {
    ledPwmMode();
    ledPwmWrite(0);
    Serial.println(F("1W LED PWM attached on GPIO48 at 1kHz, 8-bit. Use: led level <0-255>"));
  }
  else if (line.startsWith("led level ")) {
    int v = constrain(line.substring(10).toInt(), 0, 255);
    ledPwmMode();
    ledPwmWrite(v);
    Serial.printf("1W LED PWM=%d on GPIO48\n", v);
  }
  else if (line.startsWith("rgb ")) {
    int r = 0, g = 0, b = 0;
    sscanf(line.c_str(), "rgb %d %d %d", &r, &g, &b);
    rgbSet(constrain(r, 0, 255), constrain(g, 0, 255), constrain(b, 0, 255));
    Serial.printf("RGB set to %d,%d,%d\n", constrain(r,0,255), constrain(g,0,255), constrain(b,0,255));
  }
  else if (line == "pca_scan") {
    i2cBusRecover();
    uint8_t err = i2cAckCode(ADDR_PCA9685);
    Serial.printf("PCA9685 at 0x40: %s, code=%u\n", i2cAckText(err), err);
    Serial.printf("PCA9685 result: %s\n", err == 0 ? "PRESENT" : "NOT PRESENT");
  }
  else if (line == "sd" || line == "sd_test") testSD();
  else if (line == "imu") testIMU();
  else if (line == "crypto" || line == "atecc" || line == "secure") {
    i2cBusRecover();
    Serial.println(F("Waking ATECC608B and retrying ACK..."));
    uint8_t err = ateccAckWithWake(5);
    Serial.printf("ATECC608B crypto chip at 0x60: %s, code=%u\n", i2cAckText(err), err);
    if (err == 0) Serial.println(F("ATECC608B result: PRESENT"));
    else Serial.println(F("ATECC608B result: NOT PRESENT after wake/retry"));
  }
  else if (line == "mic") testMic();
  else {
    Serial.print(F("Unknown command: "));
    Serial.println(line);
    Serial.println(F("Type: help"));
  }
}
