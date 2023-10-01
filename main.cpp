#include <Adafruit_GFX.h>
#include <Wire.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <SD.h> // Include the SD library
#include <Adafruit_ILI9341.h>

#define TFT_CLK 13
#define TFT_MISO 12
#define TFT_MOSI 11
#define TFT_CS 10
#define TFT_DC 9
#define TFT_RST 8

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST, TFT_CLK, TFT_RST, TFT_MISO);

#define LOAD_CELL_PIN 34 // Analog pin to connect to the load cell

File dataFile; // Create a File object to write data to the SD card

// Mapping constants
const float SIGNAL_MIN = 0.01; // Minimum signal from the load cell
const float SIGNAL_MAX = 0.25; // Maximum signal from the load cell
const float WEIGHT_MIN = 0.01; // Minimum weight in lbs
const float WEIGHT_MAX = 100.0; // Maximum weight in lbs

// Function to map a float value from one range to another
const int mapFloat(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3); // Rotate the display if necessary
  tft.fillScreen(ILI9341_BLACK);

  // Initialize the SD card
  if (!SD.begin(SS)) {
    Serial.println("SD card initialization failed!");
    while (1);
  }

  // Create a new CSV file
  dataFile = SD.open("load_cell_data.csv", FILE_WRITE);

  if (dataFile) {
    dataFile.println("Time (ms),Load (lbs)"); // Write headers to the CSV file
    dataFile.close();
  } else {
    Serial.println("Error opening data file");
  }
}

void loop() {
  int adcValue = analogRead(LOAD_CELL_PIN); // Read the analog value from the load cell
    // Map the ADC reading to the weight range
  float pounds = mapFloat(adcValue, 0, 4095, WEIGHT_MIN, WEIGHT_MAX);

  // Record the data to the SD card
  dataFile = SD.open("load_cell_data.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.print(millis()); // Record the time in milliseconds
    dataFile.print(",");
    dataFile.println(pounds); // Record the load in pounds
    dataFile.close();
  } else {
    Serial.println("Error opening data file");
  }

  tft.fillScreen(ILI9341_BLACK); // Clear the screen
  tft.setCursor(10, 10);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("Load Cell Reading:");

  tft.setTextSize(3);
  tft.setCursor(10, 40);
  tft.println(String(pounds, 2) + " lbs"); // Display the weight with two decimal places

  delay(1000); // Update the reading and record data every second, adjust as needed
}
