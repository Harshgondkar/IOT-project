#define BLYNK_TEMPLATE_ID "TMPL3X5oc0tv4"
#define BLYNK_TEMPLATE_NAME "smart energy meter"
#define BLYNK_AUTH_TOKEN "UflHsmqoAsn0jjCjaNbScRBqSczOv47I"

#define BLYNK_PRINT Serial  // Enable Serial printing for Blynk debug

#include <EmonLib.h>   // Energy monitoring library
#include <EEPROM.h>    // EEPROM storage for persistent data
#include <ESP8266WiFi.h>  // WiFi connectivity for ESP8266
#include <BlynkSimpleEsp8266.h> // Blynk for ESP8266

// **Calibration Constants**
const float vCalibration = 42.5;
const float currCalibration = 1.80;

// **WiFi Credentials**
const char ssid[] = "shiv";
const char pass[] = "11111111";

// **Energy Monitor**
EnergyMonitor emon;

// **Blynk Timer**
BlynkTimer timer;

// **Energy Variables**
float kWh = 0.0;
float cost = 0.0;
const float ratePerkWh = 6.5;
unsigned long lastMillis = millis();

// **EEPROM Addresses**
const int addrKWh = 12;
const int addrCost = 16;

// **Reset Button Pin**
const int resetButtonPin = D2;

// **Function Prototypes**
void sendEnergyDataToBlynk();
void readEnergyDataFromEEPROM();
void saveEnergyDataToEEPROM();
void resetEEPROM();

void setup() {
  Serial.begin(115200);

  // **WiFi Connection**
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // **Blynk Initialization**
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // **EEPROM Initialization**
  EEPROM.begin(512);

  // **Initialize Reset Button**
  pinMode(resetButtonPin, INPUT_PULLUP);

  // **Read Stored Data**
  readEnergyDataFromEEPROM();

  // **Setup Voltage & Current Measurement**
  emon.voltage(A0, vCalibration, 1.7);
  emon.current(D1, currCalibration);

  // **Timers for Blynk Updates**
  timer.setInterval(2000L, sendEnergyDataToBlynk);
}

void loop() {
  Blynk.run();
  timer.run();

  // **Check Reset Button**
  if (digitalRead(resetButtonPin) == LOW) {
    delay(200); // Debounce delay
    resetEEPROM();
  }
}

void sendEnergyDataToBlynk() {
  emon.calcVI(20, 2000);
  float Vrms = emon.Vrms;
  float Irms = emon.Irms;
  float apparentPower = emon.apparentPower;

  // **Calculate Energy Consumed (kWh)**
  unsigned long currentMillis = millis();
  kWh += apparentPower * (currentMillis - lastMillis) / 3600000000.0;
  lastMillis = currentMillis;

  // **Calculate Cost**
  cost = kWh * ratePerkWh;

  // **Save to EEPROM**
  saveEnergyDataToEEPROM();

  // **Send Data to Blynk**
  Blynk.virtualWrite(V0, Vrms);
  Blynk.virtualWrite(V1, Irms);
  Blynk.virtualWrite(V2, apparentPower);
  Blynk.virtualWrite(V3, kWh);
  Blynk.virtualWrite(V4, cost);

  // **Print Data to Serial Monitor**
  Serial.printf("V: %.2fV, I: %.2fA, P: %.2fW, Energy: %.2fkWh, Cost: %.2f\n", Vrms, Irms, apparentPower, kWh, cost);
}

void readEnergyDataFromEEPROM() {
  EEPROM.get(addrKWh, kWh);
  EEPROM.get(addrCost, cost);

  // **Check if Data is Valid**
  if (isnan(kWh)) {
    kWh = 0.0;
    saveEnergyDataToEEPROM();
  }
  if (isnan(cost)) {
    cost = 0.0;
    saveEnergyDataToEEPROM();
  }
}

void saveEnergyDataToEEPROM() {
  EEPROM.put(addrKWh, kWh);
  EEPROM.put(addrCost, cost);
  EEPROM.commit();
}

void resetEEPROM() {
  kWh = 0.0;
  cost = 0.0;
  saveEnergyDataToEEPROM();
  Serial.println("EEPROM Reset!");
}