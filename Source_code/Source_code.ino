#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WiFiManager.h>

// Define constants for display dimensions and I2C address
#define WIDTH 128
#define HEIGHT 64
#define RESET_PIN -1
#define OLED_ADDRESS 0x3C

// Define pin assignments for components
#define BUZZER_PIN 14
#define GREEN_LED 19
#define YELLOW_LED 18
#define RED_LED 5
#define MUTE_BUTTON 32
#define TEST_BUTTON 33
#define MQ_SENSOR_PIN 35 // Other pins: 37, 38, 39, 34, 36
#define PIR_SENSOR_PIN 27
#define MUTE_LED 15
#define TEST_LED 4

// Define Blynk template information
#define BLYNK_TEMPLATE_ID "TMPL6FNWeJsx3"
#define BLYNK_TEMPLATE_NAME "Gas Leakage Detector"
#define BLYNK_AUTH_TOKEN "cj3DAuZWEThcB4aGmqUUZbpqSrFxeLQv"

// Virtual pins for Blynk buttons
#define VPIN_BUTTON_1 V1
#define VPIN_BUTTON_2 V2

// Global variables
bool timeouted = false;
BlynkTimer timer;
Adafruit_SSD1306 display(WIDTH, HEIGHT, &Wire, RESET_PIN);

// Set trigger value for gas leakage in ppm
int leakage_ppm = 5000;
bool motion = false;
float LPGppm = 0;
bool pressed_mute = false;
int t = 0;

// Blynk authentication
char auth[] = BLYNK_AUTH_TOKEN;

// Function prototypes
void measure_ppm();
void printState(String state);
void print_line(String text, int column, int row, int textSize);
void checkBlynkStatus();
void connectWiFi();

void setup() {
  // Initialize sensor and output pins
  pinMode(MQ_SENSOR_PIN, INPUT);
  pinMode(PIR_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(MUTE_BUTTON, INPUT);
  pinMode(TEST_BUTTON, INPUT);
  pinMode(MUTE_LED, OUTPUT);
  pinMode(TEST_LED, OUTPUT);

  // Initialize serial communication
  Serial.begin(9600);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(200);
  display.clearDisplay();

  // Display welcome message
  print_line("WELCOME", 10, 25, 2);
  delay(1000);

  display.clearDisplay();
  print_line("Gas leakage detector", 0, 20, 1.5);
  delay(1000);
  display.clearDisplay();

  // Set up Blynk connection and timer
  timer.setInterval(2000L, checkBlynkStatus); // Check Blynk server connection every 2 seconds
  Blynk.config(auth);
  delay(1000);

  // Connect to Wi-Fi
  t = millis();
  while (millis() - t < 30000) {
    if (WiFi.status() != WL_CONNECTED && !timeouted) {
      connectWiFi();
      break;
    }
  }
}

void loop() {
  // Handle Wi-Fi connection during button press
  if ((digitalRead(TEST_BUTTON) == LOW) && WiFi.status() != WL_CONNECTED) {
    t = millis();
    while (millis() - t < 30000) {
      if (WiFi.status() != WL_CONNECTED) {
        connectWiFi();
        break;
      }
    }
  }

  // Measure gas concentration
  measure_ppm();
  Serial.println(LPGppm);
  Blynk.virtualWrite(VPIN_BUTTON_1, LPGppm);

  // Check for normal gas concentration
  if (LPGppm < 200) {
    motion = false;
    pressed_mute = false;
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(TEST_LED, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    printState("NORMAL");
    Blynk.virtualWrite(VPIN_BUTTON_2, "NORMAL");
  } else {
    // Check for muted state or cooking mode
    if ((digitalRead(MUTE_BUTTON) == LOW) || (pressed_mute == true)) {
      Serial.println("MUTED");
      motion = false;
      pressed_mute = true;
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(YELLOW_LED, HIGH);
      digitalWrite(TEST_LED, LOW);
      digitalWrite(BUZZER_PIN, LOW);
      printState("COOKING");
      Blynk.virtualWrite(VPIN_BUTTON_2, "COOKING");
    } else {
      t = millis();
      while (((millis() - t) < 2000) && (!(motion))) {
        // Check for motion detection
        if (digitalRead(PIR_SENSOR_PIN)) {
          motion = true;
          Serial.println("Motion detected ");
          digitalWrite(TEST_LED, HIGH);
          print_line("Motion detected during leakage", 0, 0, 2);
          delay(500);
          break;
        }
        // Check for mute button press
        if (digitalRead(MUTE_BUTTON) == LOW) {
          pressed_mute = true;
          digitalWrite(TEST_LED, LOW);
        }
      }
      if (!(motion)) {
        delay(500);
        Blynk.logEvent("leakage_alert", "Gas Leakage Detected!");
      }
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(YELLOW_LED, LOW);
      digitalWrite(RED_LED, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      print_line("Hold mute button to mute", 0, 17, 1);
      printState("LEAKAGE");
      Blynk.virtualWrite(VPIN_BUTTON_2, "LEAKAGE");
      digitalWrite(RED_LED, LOW);
      digitalWrite(BUZZER_PIN, LOW);
      delay(500);
    }
  }
  Blynk.run();
  timer.run();
}

// Function to measure gas concentration in ppm
void measure_ppm() {
  float LPGAvg = 0;
  for (int i = 0; i < 30; i++) {
    delay(100);
    LPGAvg += analogRead(MQ_SENSOR_PIN);
  }
  LPGppm = map(LPGAvg / 30, 0, 4095, 200, 10000);
  LPGppm = LPGAvg;
}

// Function to print system state information
void printState(String state) {
  Serial.print("LPG ppm: ");
  Serial.print(LPGppm);
  Serial.println(" ppm");
  display.clearDisplay();
  print_line(state, 26, 45, 2);
  print_line("LPG concentration:", 0, 0, 1);
  display.setCursor(26, 20);
  print_line(String(LPGppm) + " ppm", 26, 20, 1);
  delay(3000);
}

// Function to print text on the OLED display
void print_line(String text, int column, int row, int textSize) {
  display.setTextSize(textSize);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(column, row);
  display.println(text);
  display.display();
}

// Function to check Blynk server status
void checkBlynkStatus() {
  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    Serial.println("Blynk Not Connected");
    digitalWrite(MUTE_LED, LOW);
  }
  if (isconnected == true) {
    digitalWrite(MUTE_LED, HIGH);
    Serial.println("Blynk Connected");
  }
}

// Function to connect to Wi-Fi using WiFiManager
void connectWiFi() {
  display.clearDisplay();
  print_line("Connect to WiFi...", 1, 1, 0);
  WiFiManager wm;
  wm.resetSettings();
  wm.setConfigPortalTimeout(120); // Set connection timeout to 2 minutes
  bool res = wm.autoConnect("AutoConnectAP", "password"); // Password-protected AP

  if (res) {
    Serial.println("Connected to WiFi");
    display.clearDisplay();
    digitalWrite(MUTE_LED, HIGH);
    print_line("Connected to WiFi", 1, 1, 0);
    delay(2);
    return; // Exit the function on successful connection
  } else {
    Serial.println("Failed to connect to WiFi");
    display.clearDisplay();
    timeouted = true;
    print_line("WiFi connection failed", 1, 1, 0);
    delay(2);
    return; // Exit the function on failure
  }
}

