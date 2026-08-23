// SPDX-License-Identifier: CERN-OHL-P-2.0

/*
  Proton AI Core Vision+ — Example: Status LED Control
  ============================================================================
  Minimal example: drive the onboard addressable RGB status LED (WS2812B)
  and the 1W LED over Serial commands. Extracted from the full board
  firmware (firmware/proton-ai-core-vision-plus/), which also includes
  camera, Wi-Fi, and I2C device control.

  Arduino IDE settings (same as the full firmware):
    Board: ESP32S3 Dev Module
    USB CDC On Boot: Enabled
    Serial Monitor: 115200 baud

  Requires the Adafruit_NeoPixel library (Library Manager).

  Serial commands:
    rgb <r> <g> <b>   - set RGB LED color (0-255 each)
    led on            - 1W LED on (direct GPIO)
    led off           - 1W LED off (direct GPIO)
    led blink         - blink 1W LED 5 times
    led pwm           - switch 1W LED to PWM mode
    led level <0-255> - set 1W LED PWM brightness
  ============================================================================
*/

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define RGB_LED_PIN     1
#define RGB_LED_COUNT   1
#define LED_1W_PIN      48

Adafruit_NeoPixel rgbLed(RGB_LED_COUNT, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);

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

void setup() {
  Serial.begin(115200);
  delay(1500);

  rgbLed.begin();
  rgbLed.setBrightness(40);
  rgbSet(0, 0, 20);

  pinMode(LED_1W_PIN, OUTPUT);
  ledOffDirect();

  Serial.println();
  Serial.println(F("Proton AI Core Vision+ — Status LED example"));
  Serial.println(F("Commands: rgb <r> <g> <b>, led on, led off, led blink, led pwm, led level <0-255>"));
}

void loop() {
  if (!Serial.available()) return;

  String line = Serial.readStringUntil('\n');
  line.trim();
  if (!line.length()) return;

  if (line == "led on") {
    ledOnDirect();
    Serial.println(F("1W LED ON."));
  } else if (line == "led off") {
    ledOffDirect();
    Serial.println(F("1W LED OFF."));
  } else if (line == "led blink") {
    ledBlinkTest();
    Serial.println(F("Blink done."));
  } else if (line == "led pwm") {
    ledPwmMode();
    ledPwmWrite(0);
    Serial.println(F("1W LED PWM mode. Use: led level <0-255>"));
  } else if (line.startsWith("led level ")) {
    int v = constrain(line.substring(10).toInt(), 0, 255);
    ledPwmMode();
    ledPwmWrite(v);
    Serial.printf("1W LED PWM=%d\n", v);
  } else if (line.startsWith("rgb ")) {
    int r = 0, g = 0, b = 0;
    sscanf(line.c_str(), "rgb %d %d %d", &r, &g, &b);
    rgbSet(constrain(r, 0, 255), constrain(g, 0, 255), constrain(b, 0, 255));
    Serial.printf("RGB set to %d,%d,%d\n", constrain(r, 0, 255), constrain(g, 0, 255), constrain(b, 0, 255));
  } else {
    Serial.print(F("Unknown command: "));
    Serial.println(line);
  }
}
