#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal_I2C.h>

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define 10 LED pins for 10 compartments
const int ledPins[10] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
const int buzzerPin = 12;

struct MedicineSchedule {
  int compartment;           // LED index
  String medicineName;       // Name for LCD
  int hour;                  // Time (24h)
  int minute;
  bool alarmTriggered;
};

// 10 medicine schedules (add/edit times & names)
MedicineSchedule schedules[10] = {
  {0, "Med A", 9, 0, false},
  {1, "Med B", 10, 0, false},
  {2, "Med C", 11, 0, false},
  {3, "Med D", 12, 0, false},
  {4, "Med E", 13, 0, false},
  {5, "Med F", 14, 0, false},
  {6, "Med G", 15, 0, false},
  {7, "Med H", 16, 0, false},
  {8, "Med I", 17, 0, false},
  {9, "Med J", 18, 0, false}
};

void setup() {
  Wire.begin();
  rtc.begin();
  lcd.init();
  lcd.backlight();

  for (int i = 0; i < 10; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
}

void loop() {
  DateTime now = rtc.now();

  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  if (now.hour() < 10) lcd.print("0");
  lcd.print(now.hour());
  lcd.print(":");
  if (now.minute() < 10) lcd.print("0");
  lcd.print(now.minute());

  for (int i = 0; i < 10; i++) {
    if (now.hour() == schedules[i].hour && now.minute() == schedules[i].minute) {
      if (!schedules[i].alarmTriggered) {
        digitalWrite(ledPins[schedules[i].compartment], HIGH);
        digitalWrite(buzzerPin, HIGH);
        delay(1000);
        digitalWrite(buzzerPin, LOW);

        lcd.setCursor(0, 1);
        lcd.print("Take: ");
        lcd.print(schedules[i].medicineName);
        schedules[i].alarmTriggered = true;
      }
    } else {
      digitalWrite(ledPins[i], LOW);
      if (now.minute() != schedules[i].minute) {
        schedules[i].alarmTriggered = false;
      }
    }
  }

  delay(1000);
}
