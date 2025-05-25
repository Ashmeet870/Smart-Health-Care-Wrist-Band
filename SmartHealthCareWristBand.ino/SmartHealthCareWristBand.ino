#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <U8g2lib.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include <MAX30100_PulseOximeter.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

const char* ssid = "Your SSID";
const char* password = "Your Password"";
const char* serverName = "https://breakin-badmicroservices.vercel.app/api/submit";

PulseOximeter pox;
Adafruit_BMP085 bmp;

#define ECG_PIN 34
#define ECG_LO_PLUS 32
#define ECG_LO_MINUS 33
#define TOUCH_PIN T0
#define BATTERY_PIN 35
#define BUZZER_PIN 23

#define ECG_BUFFER_SIZE 128
uint8_t ecgBuffer[ECG_BUFFER_SIZE];
int ecgIndex = 0;
int currentScreen = 0;

float latestSpo2 = 0;
float latestBpm = 0;
float latestTemp = 0;
float latestPressure = 0;
int latestBpEstimate = 0;

float spo2Buffer[4];
float bpmBuffer[4];
float tempBuffer[4];
float ecgPeakBuffer[4];
int bufferIndex = 0;

unsigned long lastSensorRead = 0;
unsigned long buzzerMuteTime = 0;
bool criticalCondition = false;
bool buzzerMuted = false;
unsigned long touchPressStart = 0;
bool touchHeld = false;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  u8g2.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");

  if (!pox.begin()) {
    Serial.println("MAX30100 failed to start");
    while (true);
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
  pox.setOnBeatDetectedCallback(onBeatDetected);

  if (!bmp.begin()) {
    Serial.println("BMP180 not found");
    while (true);
  }

  pinMode(ECG_LO_PLUS, INPUT);
  pinMode(ECG_LO_MINUS, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  xTaskCreatePinnedToCore(apiSenderTask, "APISender", 4096, NULL, 1, NULL, 0);

  showStartupScreen();
}

void loop() {
  pox.update();
  unsigned long now = millis();

  if (now - lastSensorRead > 2000) {
    lastSensorRead = now;
    readAndDisplaySensors();
  }

  if (buzzerMuted && millis() > buzzerMuteTime) {
    buzzerMuted = false;
    Serial.println("Buzzer unmuted after timeout.");
  }

  handleTouchManual();
}

void handleTouchManual() {
  static bool touchActive = false;

  if (touchRead(TOUCH_PIN) < 60) {
    if (!touchActive) {
      touchPressStart = millis();
      touchActive = true;
    }
    unsigned long duration = millis() - touchPressStart;

    if (duration > 7000 && criticalCondition) {
      clearCriticalAlert();
      touchActive = false;
    }
  } else {
    if (touchActive) {
      unsigned long duration = millis() - touchPressStart;
      if (duration > 200 && duration < 1500) {
        changeScreen();
      }
      touchActive = false;
    }
  }
}