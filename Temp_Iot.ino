#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Wi-Fi credentials
const char* ssid = "KeralaStartupMission_2G";       // Replace with your Wi-Fi SSID
const char* password = "K$um$t@6tupM$n"; // Replace with your Wi-Fi password

// DS18B20 configuration
#define ONE_WIRE_BUS 4 // GPIO 4 for DS18B20 data pin
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Server URL
const char* serverURL = "https://api.startupmission.in/webhook/temp-sensor"; // Replace with your server's API endpoint

// Built-in LED
#define WIFI_LED 2 // GPIO 2 for built-in LED

void setup() {
  // Start serial communication
  Serial.begin(115200);

  // Initialize DS18B20
  sensors.begin();

  // Initialize built-in LED
  pinMode(WIFI_LED, OUTPUT);
  digitalWrite(WIFI_LED, LOW);

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(WIFI_LED, LOW); // LED off during connection attempts
  }

  Serial.println("\nWi-Fi connected!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(WIFI_LED, HIGH); // LED on when connected
}

void loop() {
  // Check Wi-Fi connection
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(WIFI_LED, HIGH); // Ensure LED is ON when connected

    // Request temperature readings
    sensors.requestTemperatures();
    float temperatureC = sensors.getTempCByIndex(0);

    // Print temperature to Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperatureC);
    Serial.println(" °C");

    // Send temperature data to the remote server
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    // Prepare JSON payload
    String payload = "{\"temperature\": " + String(temperatureC) + "}";

    // POST the data
    int httpResponseCode = http.POST(payload);

    // Print the response
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response Code: ");
      Serial.println(httpResponseCode);
      Serial.println("Server Response: ");
      Serial.println(http.getString());
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(http.errorToString(httpResponseCode).c_str());
    }

    // End the HTTP connection
    http.end();
  } else {
    digitalWrite(WIFI_LED, LOW); // Turn LED off if disconnected
    Serial.println("Wi-Fi disconnected! Attempting to reconnect...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWi-Fi reconnected!");
  }

  delay(60000); // Wait 60 seconds before next reading
}
