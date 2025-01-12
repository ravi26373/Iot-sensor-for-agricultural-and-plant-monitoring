#define BLYNK_TEMPLATE_ID "TMPL3OF_IDuIU"
#define BLYNK_TEMPLATE_NAME "Smart Plant Monitoring System"
#include <DHT.h>
#include <BlynkSimpleEsp8266.h>
// #include <BlynkTimer.h>

// Pin Definitions
#define MOISTURE_SENSOR_PIN A0 // Analog pin for soil moisture sensor
#define DHT_PIN D2            // Digital pin for DHT sensor
#define DHT_TYPE DHT11        // Change to DHT22 if using DHT22
#define RELAY_PIN D5          // Digital pin for relay module
#define LED_PIN D4            // Digital pin for LED

// Blynk credentials
char auth[] = "CgSz4ok0_Ae0T7qMs2cAXXYRbQEPjBjm";  // Replace with your Blynk Auth Token
char ssid[] = "Nokia 1";    // Replace with your WiFi SSID
char pass[] = "rabi@7352"; // Replace with your WiFi Password

// Initialize DHT sensor
DHT dht(DHT_PIN, DHT_TYPE);
BlynkTimer timer; // Initialize BlynkTimer

// Threshold values
const int moistureThreshold = 500; // Adjust based on calibration
const float tempThresholdLow = 10.0; // Minimum temperature in Celsius
const float tempThresholdHigh = 40.0; // Maximum temperature in Celsius
const float humidityThresholdLow = 30.0; // Minimum humidity in percentage
const float humidityThresholdHigh = 70.0; // Maximum humidity in percentage

// Relay control variable
bool relayState = false;

// Function to read sensors and send data to Blynk
void readSensors() {
  // Read soil moisture level
  int soilMoisture = analogRead(MOISTURE_SENSOR_PIN);
  Serial.print("Soil Moisture: ");
  Serial.println(soilMoisture);
  Blynk.virtualWrite(V2, soilMoisture); // Send soil moisture data to Blynk Virtual Pin V2

  // Read temperature and humidity
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check for read errors
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Sensor Error: Failed to read temperature or humidity.");
    return; // Skip further processing if sensors fail
  }

  // Log values to Serial Monitor and send to Blynk
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");
  Blynk.virtualWrite(V0, temperature); // Send temperature data to Blynk Virtual Pin V0

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  Blynk.virtualWrite(V1, humidity); // Send humidity data to Blynk Virtual Pin V1

  // Uncomment the following logic if you want to automate the relay and LED based on thresholds
  /*
  if (!relayState) { // Only automate relay if manual override is off
    if (soilMoisture < moistureThreshold || temperature < tempThresholdLow || 
        temperature > tempThresholdHigh || humidity < humidityThresholdLow || 
        humidity > humidityThresholdHigh) {
      Serial.println("Threshold exceeded. Activating relay and LED.");
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(RELAY_PIN, HIGH); // Activate relay
    } else {
      Serial.println("All parameters normal. Deactivating relay and LED.");
      digitalWrite(LED_PIN, LOW);
      digitalWrite(RELAY_PIN, LOW); // Deactivate relay
    }
  }
  */
}

// Blynk virtual write for relay control
BLYNK_WRITE(V7) {
  relayState = param.asInt(); // Get button value (0 or 1)
  digitalWrite(RELAY_PIN, relayState ? LOW : HIGH); // Adjust for active low logic
  Serial.print("Relay state set to: ");
  Serial.println(relayState ? "ON" : "OFF");
}


void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("System Initializing...");

  // Initialize DHT sensor
  dht.begin();
  Serial.println("DHT sensor initialized.");

  // Set LED and relay pins as output
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Ensure relay is off initially
  Serial.println("Pins configured.");

  // Connect to Blynk
  Blynk.begin(auth, ssid, pass);
  Serial.println("Connected to Blynk.");

  // Set up a timer to read sensors every 2 seconds
  timer.setInterval(1000L, readSensors);
}

void loop() {
  Blynk.run();
  timer.run();
}

