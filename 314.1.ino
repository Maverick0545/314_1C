#include <WiFiNINA.h>
#include "DHT.h"

#define DHTPIN 2       // Pin connected to DHT22
#define DHTTYPE DHT22  // DHT22 sensor

DHT dht(DHTPIN, DHTTYPE);

// Replace with your network credentials
const char* ssid = "NETGEAR21";       // Your WiFi network name
const char* password = "69699696";    // Your WiFi password

// ThingSpeak settings
const char* server = "api.thingspeak.com";
String apiKey = "UK791937HOZ64B65";

WiFiClient client;

void setup() {
  Serial.begin(9600);
  dht.begin();
  delay(1000);  // Stabilize sensor

  // Connect to WiFi
  connectToWiFi();
}

void loop() {
  // Check WiFi connection status
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected. Reconnecting...");
    connectToWiFi();
    return; // Exit loop until WiFi is connected
  }

  // Read temperature and humidity
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print sensor data to Serial Monitor
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  // Connect to ThingSpeak
  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(temperature);
    postStr += "&field2=";
    postStr += String(humidity);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    delay(500); // Give some time for the server to respond

    // Check the response from the server
    while (client.available()) {
      String response = client.readString();
      Serial.println("Response from server: " + response);
    }

    Serial.println("Data sent to ThingSpeak");
  } else {
    Serial.println("Connection to ThingSpeak failed.");
  }

  client.stop();
  delay(20000);  // Wait for 20 seconds before the next reading
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  int attempts = 0;  // Counter for connection attempts
  while (WiFi.status() != WL_CONNECTED) {
    attempts++;
    delay(10000);
    Serial.print(".");
    
    // Try reconnecting up to 10 times
    if (attempts > 10) {
      Serial.println("\nFailed to connect to WiFi after 10 attempts. Please check your network.");
      // Optionally, you can use a software reset by going into an infinite loop
      while (true) {
        delay(1000); // Prevent the program from continuing without WiFi
      }
    }
  }

  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

