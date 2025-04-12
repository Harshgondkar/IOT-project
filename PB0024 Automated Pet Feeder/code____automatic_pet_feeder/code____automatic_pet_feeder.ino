#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// WiFi credentials
const char* ssid = "shiv";
const char* password = "11111111";

// Component pins
#define SERVO_PIN 9
#define IR_SENSOR_PIN 7

// Create objects
Servo feederServo;
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address may vary (0x3F is another common one)
SoftwareSerial espSerial(2, 3); // RX, TX

// Variables
bool feedingTime = false;
bool petDetected = false;
unsigned long lastFeedTime = 0;
int feedInterval = 8; // Hours between feeds
int feedAmount = 45; // Degrees of servo rotation (adjust for your mechanism)

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  espSerial.begin(9600);
  
  // Initialize components
  pinMode(IR_SENSOR_PIN, INPUT);
  feederServo.attach(SERVO_PIN);
  feederServo.write(0); // Initial position
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pet Feeder v1.0");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  
  delay(2000);
  
  // Connect to WiFi
  connectToWiFi();
  
  // Display ready message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  updateDisplay();
}

void loop() {
  // Check current time (simplified - in a real project you'd use an RTC)
  unsigned long currentTime = millis() / 1000 / 60 / 60; // Convert to hours
  
  // Check if it's time to feed
  if (currentTime - lastFeedTime >= feedInterval) {
    feedingTime = true;
  }
  
  // Check IR sensor for pet presence
  petDetected = !digitalRead(IR_SENSOR_PIN); // Active LOW
  
  // Feed if it's time or if pet is detected (optional)
  if (feedingTime || petDetected) {
    dispenseFood();
    lastFeedTime = currentTime;
    feedingTime = false;
    
    // Send notification via WiFi
    sendNotification();
  }
  
  // Check for WiFi commands
  if (espSerial.available()) {
    String command = espSerial.readStringUntil('\n');
    processCommand(command);
  }
  
  // Update display
  updateDisplay();
  
  delay(500); // Short delay to reduce CPU usage
}

void dispenseFood() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dispensing Food");
  
  // Rotate servo to dispense food
  feederServo.write(feedAmount);
  delay(1000);
  
  // Return to original position
  feederServo.write(0);
  
  lcd.setCursor(0, 1);
  lcd.print("Done!");
  delay(2000);
}

void updateDisplay() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 5000) return; // Update every 5 seconds
  lastUpdate = millis();
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Next feed in: ");
  lcd.print(feedInterval - (millis() / 1000 / 60 / 60 - lastFeedTime));
  lcd.print("h");
  
  lcd.setCursor(0, 1);
  if (petDetected) {
    lcd.print("Pet detected!");
  } else {
    lcd.print("Waiting...");
  }
}

void connectToWiFi() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print(ssid);
  
  espSerial.println("AT+CWMODE=1"); // Set WiFi mode to station
  delay(1000);
  
  String cmd = "AT+CWJAP=\"";
  cmd += ssid;
  cmd += "\",\"";
  cmd += password;
  cmd += "\"";
  espSerial.println(cmd);
  
  delay(5000); // Wait for connection
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Status:");
  lcd.setCursor(0, 1);
  
  if (espSerial.find("OK")) {
    lcd.print("Connected!");
  } else {
    lcd.print("Failed!");
  }
  
  delay(2000);
}

void sendNotification() {
  espSerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80");
  delay(1000);
  
  String cmd = "GET /update?api_key=YOUR_API_KEY&field1=";
  cmd += feedAmount;
  cmd += " HTTP/1.1\r\nHost: api.thingspeak.com\r\n\r\n";
  
  espSerial.print("AT+CIPSEND=");
  espSerial.println(cmd.length());
  
  if (espSerial.find(">")) {
    espSerial.print(cmd);
  } else {
    espSerial.println("AT+CIPCLOSE");
  }
}

void processCommand(String command) {
  command.trim();
  
  if (command == "FEED_NOW") {
    dispenseFood();
    lastFeedTime = millis() / 1000 / 60 / 60;
  } else if (command.startsWith("SET_INTERVAL:")) {
    int newInterval = command.substring(13).toInt();
    if (newInterval > 0 && newInterval <= 24) {
      feedInterval = newInterval;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Interval Set:");
      lcd.setCursor(0, 1);
      lcd.print(String(feedInterval) + " hours");
      delay(2000);
    }
  } else if (command.startsWith("SET_AMOUNT:")) {
    int newAmount = command.substring(11).toInt();
    if (newAmount > 0 && newAmount <= 180) {
      feedAmount = newAmount;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Amount Set:");
      lcd.setCursor(0, 1);
      lcd.print(String(feedAmount) + " deg");
      delay(2000);
    }
  }
}