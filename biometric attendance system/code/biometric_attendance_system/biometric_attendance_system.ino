#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Keypad.h>

// Initialize LCD (16x2) at I2C address 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initialize Fingerprint Sensor
SoftwareSerial mySerial(2, 3);  // RX, TX for Fingerprint Sensor
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Define buzzer and LED pins
#define BUZZER 4
#define GREEN_LED 5
#define RED_LED 6

// Define keypad
const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// User ID to Name Mapping
const char* userNames[] = {
    "Harsh",    // ID 1
    "Bob",      // ID 2
    "Charlie",  // ID 3
    "David"     // ID 4
};

void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Fingerprint System");
    delay(2000);
    lcd.clear();

    finger.begin(57600);
    if (finger.verifyPassword()) {
        lcd.setCursor(0, 0);
        lcd.print("Sensor Found!");
    } else {
        lcd.setCursor(0, 0);
        lcd.print("Sensor Error!");
        while (1);
    }
    delay(2000);
    lcd.clear();

    pinMode(BUZZER, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
}

void loop() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("1: Enroll Finger");
    lcd.setCursor(0, 1);
    lcd.print("2: Verify Finger");

    char choice = getKeyInput();
    lcd.clear();

    if (choice == '1') {
        lcd.setCursor(0, 0);
        lcd.print("Enter ID (1-9):");
        char idChar = getKeyInput();
        int id = idChar - '0';

        if (id > 0 && id <= 9) {
            enrollFingerprint(id);
        } else {
            lcd.setCursor(0, 0);
            lcd.print("Invalid ID!");
            delay(2000);
        }
    } else if (choice == '2') {
        verifyFingerprint();
    } else {
        lcd.setCursor(0, 0);
        lcd.print("Invalid Choice!");
        delay(2000);
    }
}

char getKeyInput() {
    char key;
    do {
        key = keypad.getKey();
    } while (!key);
    return key;
}

void enrollFingerprint(int id) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enrolling ID: ");
    lcd.setCursor(12, 0);
    lcd.print(id);

    int p = -1;
    lcd.setCursor(0, 1);
    lcd.print("Place finger...");

    while (p != FINGERPRINT_OK) {
        p = finger.getImage();
    }

    p = finger.image2Tz(1);
    if (p != FINGERPRINT_OK) {
        lcd.clear();
        lcd.print("Error! Try again");
        delay(2000);
        return;
    }

    lcd.clear();
    lcd.print("Remove Finger...");
    delay(2000);

    lcd.clear();
    lcd.print("Place Again...");
    p = -1;
    while (p != FINGERPRINT_OK) {
        p = finger.getImage();
    }

    p = finger.image2Tz(2);
    if (p != FINGERPRINT_OK) {
        lcd.clear();
        lcd.print("2nd scan failed!");
        delay(2000);
        return;
    }

    p = finger.createModel();
    if (p != FINGERPRINT_OK) {
        lcd.clear();
        lcd.print("Model failed!");
        delay(2000);
        return;
    }

    p = finger.storeModel(id);
    if (p == FINGERPRINT_OK) {
        lcd.clear();
        lcd.print("Enrolled!");
        delay(2000);
    } else {
        lcd.clear();
        lcd.print("Store failed!");
        delay(2000);
    }
}

void verifyFingerprint() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Place finger...");

    int id = getFingerprintID();

    lcd.clear();
    if (id > 0 && id < sizeof(userNames) / sizeof(userNames[0])) {
        lcd.setCursor(0, 0);
        lcd.print(userNames[id]);
        lcd.print(" is present");
        digitalWrite(GREEN_LED, HIGH);
        beep(2);
        delay(2000);
        digitalWrite(GREEN_LED, LOW);
    } else {
        lcd.setCursor(0, 0);
        lcd.print("Access Denied!");
        digitalWrite(RED_LED, HIGH);
        beep(1);
        delay(2000);
        digitalWrite(RED_LED, LOW);
    }
}

int getFingerprintID() {
    uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK) return -1;

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK) return -1;

    p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK) return -1;

    return finger.fingerID;
}

void beep(int times) {
    for (int i = 0; i < times; i++) {
        digitalWrite(BUZZER, HIGH);
        delay(200);
        digitalWrite(BUZZER, LOW);
        delay(200);
    }
}
