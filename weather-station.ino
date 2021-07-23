#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include "WiFi.h" //connection to local wifi
#include "ESPAsyncWebServer.h" //web server
#include "SPIFFS.h"

#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

Adafruit_BMP280 bmp;

const char* ssid = "your-wifi-name";
const char* password =  "your-wifi-password";

AsyncWebServer server(80); //async web server

void connect_to_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.println();
}

void initialize_SPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}

void check_bmp280() {
  Serial.println(F("BMP280 test"));
  if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                     "try a different address!"));
    while (1) delay(10);
  } else {
    Serial.println("BMP test: OK");
    }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  Serial.println();
}

float get_temperature() {
  return bmp.readTemperature();
}

float get_pressure() {
  return bmp.readPressure();
}

float get_altitude() {
  return bmp.readAltitude(1013.25);
}



void setup() {
  Serial.begin(115200);
  initialize_SPIFFS();
  check_bmp280();
  connect_to_wifi();
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html");
  });

  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/styles.css", "text/css");
  });
  
  //sending plain text to the server
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(get_temperature()).c_str());
  });

  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(get_pressure()).c_str());
  });

  server.on("/altitude", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(get_altitude()).c_str());
  });

  
  // Start server
  server.begin();
}

void loop() {
  float temperature, pressure, altitude;
  
  temperature = get_temperature();
  Serial.print("temperature: ");
  Serial.print(temperature);
  Serial.println(" *C    ");
  
  pressure = get_pressure();
  Serial.print("pressure: ");
  Serial.print(pressure);
  Serial.println(" Pa    ");
  
  altitude = get_altitude();
  Serial.print("approx altitude: ");
  Serial.print(altitude);
  Serial.println(" m");
  
  Serial.println();
  delay(5000);
}
