#define BLYNK_PRINT Serial

// Replace with your Blynk credentials
#define BLYNK_TEMPLATE_ID "TMPL3BatfKCoX"
#define BLYNK_TEMPLATE_NAME "Realy Control"
#define BLYNK_AUTH_TOKEN "f-0pyLzJvTOYQF7BvMsCzDk--TZfZK8K"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Replace with your WiFi credentials
char ssid[] = "shiv";          // Your WiFi Name
char pass[] = "11111111";      // Your WiFi Password
char auth[] = BLYNK_AUTH_TOKEN; // Blynk Auth Token

#define RELAY1 D5  // GPIO14 - First Relay (Bulb 1)
#define RELAY2 D6  // GPIO12 - Second Relay (Bulb 2)

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\nStarting ESP8266...");
  Serial.println("Connecting to WiFi...");
  
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Blynk.begin(auth, ssid, pass);
  Serial.println("Connected to Blynk!");

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  
  digitalWrite(RELAY1, HIGH); // Ensure Relay 1 starts OFF
  digitalWrite(RELAY2, HIGH); // Ensure Relay 2 starts OFF

  Serial.println("\nCommands:");
  Serial.println("1 - Turn ON Bulb 1");
  Serial.println("2 - Turn OFF Bulb 1");
  Serial.println("3 - Turn ON Bulb 2");
  Serial.println("4 - Turn OFF Bulb 2");

  Serial.println("\nAll bulbs are OFF at startup!");
}

// Blynk control for Relay 1 (Bulb 1)
BLYNK_WRITE(V0) {
  int relay1State = param.asInt();
  digitalWrite(RELAY1, relay1State ? LOW : HIGH);
  Serial.print("Blynk - Relay 1: ");
  Serial.println(relay1State ? "ON" : "OFF");
}

// Blynk control for Relay 2 (Bulb 2)
BLYNK_WRITE(V1) {
  int relay2State = param.asInt();
  digitalWrite(RELAY2, relay2State ? LOW : HIGH);
  Serial.print("Blynk - Relay 2: ");
  Serial.println(relay2State ? "ON" : "OFF");
}

void loop() {
  Blynk.run();

  // Serial control
  if (Serial.available()) {
    char command = Serial.read();
    switch (command) {
      case '1':
        digitalWrite(RELAY1, LOW);
        Serial.println("Serial - Relay 1: ON");
        break;
      case '2':
        digitalWrite(RELAY1, HIGH);
        Serial.println("Serial - Relay 1: OFF");
        break;
      case '3':
        digitalWrite(RELAY2, LOW);
        Serial.println("Serial - Relay 2: ON");
        break;
      case '4':
        digitalWrite(RELAY2, HIGH);
        Serial.println("Serial - Relay 2: OFF");
        break;
      default:
        Serial.println("Invalid command! Use 1, 2, 3, or 4.");
        break;
    }
  }
}
