#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_VEML7700.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <avr/pgmspace.h>

#define BUZZER_PIN 3
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1


void displayWhiteScreen();
void displayHydrationReminder(float light);
void playMelody();
bool isDayTime();

Adafruit_BME280 bme;
Adafruit_VEML7700 veml = Adafruit_VEML7700();
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long previousMillis = 0;
const long interval = 10000; // 10 seconds for testing, change to 3600000 for 1 hour
const float lightThreshold = 100.0; // Threshold for dim light in lux

// Melody notes and durations
int melody[] = {330, 330, 349, 392, 392, 349, 330, 294, 262, 262, 294, 330, 330, 294, 294};
int noteDurations[] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 8, 4};

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(9600);
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor");
    while (1);
  }
  if (!veml.begin()) {
    Serial.println("Could not find a valid VEML7700 sensor");
    while (1);
  }
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  display.clearDisplay();
}

void loop() {
  float light = veml.readLux();

  if (isDayTime() && light < lightThreshold) {
    displayWhiteScreen();
  } else {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      displayHydrationReminder(light); // Pass the light reading as a parameter
    }
  }
}

void displayHydrationReminder(float light) {
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  int lineSpacing = 10; // 设定行间距
  int startY = 0;       // 开始的Y坐标

  if (temperature > 26 || humidity > 60) {
    display.setCursor(0, startY);
    display.println("Hot/Humid Day");
    display.setCursor(0, startY + lineSpacing);
    display.println("Drink more water!");

    display.setCursor(0, startY + 2 * lineSpacing);
    display.print("Temp: ");
    display.print(temperature);
    display.println(" C");

    display.setCursor(0, startY + 3 * lineSpacing);
    display.print("Humidity: ");
    display.print(humidity);
    display.println(" %");

    display.setCursor(0, startY + 4 * lineSpacing);
    display.print("Light: ");
    display.print(light);
    display.println(" lux");
    playMelody();
  } else {
    display.setCursor(0, startY);
    display.println("Normal Day");
    display.setCursor(0, startY + lineSpacing);
    display.println("Stay hydrated!");

    display.setCursor(0, startY + 2 * lineSpacing);
    display.print("Temp: ");
    display.print(temperature);
    display.println(" C");

    display.setCursor(0, startY + 3 * lineSpacing);
    display.print("Humidity: ");
    display.print(humidity);
    display.println(" %");

    display.setCursor(0, startY + 4 * lineSpacing);
    display.print("Light: ");
    display.print(light);
    display.println(" lux");
  }
  display.display();
}



void playMelody() {
  for (int thisNote = 0; thisNote < 15; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZZER_PIN, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(BUZZER_PIN);
  }
}

bool isDayTime() {
  return true; // Placeholder for daytime check, replace with actual logic as needed
}

void displayWhiteScreen() {
  display.clearDisplay();
  display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  display.display();
}