// Simulation --> https://wokwi.com/projects/342032431249883731

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* apiKey = "2562fa1c196e6b0fd7c164cef8e38ad4";
const char* city = "Bangkok"; // Change to your city
const char* countryCode = "TH"; // Change to your country code
const char* weatherURL = "http://api.openweathermap.org/data/2.5/weather?q=%s,%s&appid=%s&units=metric";

#define BTN_PIN 5
#define TFT_DC 2
#define TFT_CS 15
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

void fetchWeatherData(char* weatherInfo, size_t size) {
  HTTPClient http;
  char url[256];
  snprintf(url, sizeof(url), weatherURL, city, countryCode, apiKey);

  http.begin(url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      DynamicJsonDocument doc(2048);
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        float temperature = doc["main"]["temp"];
        float humidity = doc["main"]["humidity"];
        const char* description = doc["weather"][0]["description"];
        
        snprintf(weatherInfo, size, "Temp: %.2f C\nHumidity: %.2f%%\n%s", temperature, humidity, description);
      } else {
        snprintf(weatherInfo, size, "Error parsing JSON");
      }
    } else {
      snprintf(weatherInfo, size, "HTTP GET error: %d", httpCode);
    }
  } else {
    snprintf(weatherInfo, size, "HTTP request failed");
  }

  http.end();
}

void displayWeather() {
  tft.setTextColor(ILI9341_WHITE);
  tft.println("\nLoading weather...");

  char weatherInfo[128];
  fetchWeatherData(weatherInfo, sizeof(weatherInfo));
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_GREEN);
  tft.println(weatherInfo);
}

void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);

  WiFi.begin(ssid, password, 6);

  tft.begin();
  tft.setRotation(1);

  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    tft.print(".");
  }

  tft.print("\nOK! IP=");
  tft.println(WiFi.localIP());

  displayWeather();
}

void loop() {
  if (digitalRead(BTN_PIN) == LOW) {
    delay(50); // debounce delay
    while (digitalRead(BTN_PIN) == LOW); // wait for button release

    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0, 0);
    displayWeather();
  }

  delay(100);
}
