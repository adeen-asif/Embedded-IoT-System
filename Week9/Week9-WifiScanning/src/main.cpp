#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);   // Station mode
  WiFi.disconnect();     // Not connected to any network
  delay(1000);
}

void loop() {
  Serial.println("Scanning WiFi networks...");

  int count = WiFi.scanNetworks();  // Scan request

  if (count <= 0) {
    Serial.println("No networks found");
  } else {
    Serial.print("Found ");
    Serial.print(count);
    Serial.println(" networks:");
  }

  for (int i = 0; i < count; i++) {
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(WiFi.SSID(i));
    Serial.print(" (");
    Serial.print(WiFi.RSSI(i));
    Serial.println(" dBm)");
  }

  Serial.println();
  delay(5000);   // Scan every 5 seconds
}