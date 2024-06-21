 // Include the library files
#define BLYNK_TEMPLATE_ID "TMPL6T4rMN1uh"
#define BLYNK_TEMPLATE_NAME "Plant watering system"

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <ESP32Servo.h> // Include Servo library for servo motor control

#define sensorMoisturePin 33
#define sensorHumidityPin 34  // Pin for the humidity sensor
#define relayPin 4

// Initialize the LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2);

BlynkTimer timer;

// Enter your Auth token
char auth[] = "Cf4kIovIReSn0wzM382fDhemdcm-cd4i";

// Enter your WIFI SSID and password
char ssid[] = "Kaveesh's S10";
char pass[] = "01234567";

#define DHTPIN 27          // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11      // DHT 11

DHT dht(DHTPIN, DHTTYPE);

bool manualControl = false; // Variable to store manual control state

Servo servo; // Declare Servo object

void setup() {
  // Debug console
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  lcd.init();
  lcd.backlight();
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

  lcd.setCursor(1, 0);
  lcd.print("System Loading");
  for (int a = 0; a <= 15; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(200);
  }
  lcd.clear();

  // Initialize DHT sensor
  dht.begin();

  // Set up Blynk button widget for manual control
  Blynk.virtualWrite(V4, !manualControl); // Update button state on app

  servo.attach(15); // Attach servo to pin 15 (NodeMCU D8)
}

//Get the soil moisture values
void soilMoisture() {
  int value = analogRead(sensorMoisturePin);
  value = map(value, 0, 4095, 0, 100);
  value = (value - 100) * -1;
  Blynk.virtualWrite(V0, value);
  Serial.println(value);
  lcd.setCursor(0, 0);
  lcd.print("Moisture :");
  lcd.print(value);
  lcd.print(" ");
}

void humidity() {
  float humidityValue = dht.readHumidity();  // Read humidity value from DHT sensor
  if (!isnan(humidityValue)) {
    Blynk.virtualWrite(V3, humidityValue);   // Send humidity value to Blynk
    Serial.print("Humidity: ");
    Serial.print(humidityValue);
    Serial.println("%");
  } else {
    Serial.println("Failed to read humidity from DHT sensor");
  }
}

void temperature() {
  float temperatureValue = dht.readTemperature();  // Read temperature value from DHT sensor
  if (!isnan(temperatureValue)) {
    Blynk.virtualWrite(V2, temperatureValue);   // Send temperature value to Blynk
    Serial.print("Temperature: ");
    Serial.print(temperatureValue);
    Serial.println("°C");
  } else {
    Serial.println("Failed to read temperature from DHT sensor");
  }
}

//Get the button value
BLYNK_WRITE(V1) {
  bool Relay = param.asInt();
  if (!manualControl) { // If not in manual control mode
    if (Relay == 1) {
      digitalWrite(relayPin, LOW);
      lcd.setCursor(0, 1);
      lcd.print("Motor is ON ");
    } else {
      digitalWrite(relayPin, HIGH);
      lcd.setCursor(0, 1);
      lcd.print("Motor is OFF");
    }
  }
}

void loop() {
  soilMoisture(); // Get the soil moisture value
  humidity();     // Get the humidity value
  temperature();  // Get the temperature value
  Blynk.run();    // Run the Blynk library

  delay(200);
}

BLYNK_WRITE(V5) {
  int angle = param.asInt(); // Get the angle from Blynk
  servo.write(angle); // Move the servo to the specified angle
}
