#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h> 

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid     = "Airtel_Black";
const char* password = "12345678";
String APIKEY = "API-OF-OPENWEATHERMAP";
String CityID = "111111";

WiFiClient client;
char servername[] = "api.openweathermap.org";
String result;

int counter = 30;

String weatherDescription = "";
String weatherLocation = "";
String Country;
float Temperature;
float Humidity;
float Pressure;

void setup() {
  Serial.begin(9600);
  int cursorPosition = 0;
  lcd.init();
  lcd.backlight();
  lcd.print("   Connecting");  
  Serial.println("Connecting");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.setCursor(cursorPosition, 2); 
    lcd.print(".");
    cursorPosition++;
  }
  lcd.clear();
  lcd.print("   Connected!");
  Serial.println("Connected");
  delay(1000);
}

void loop() {
  if (counter == 30)
  {
    counter = 0;
    displayGettingData();
    delay(1000);
    getWeatherData();
  } else {
    counter++;
    displayWeather(weatherLocation, weatherDescription);
    delay(5000);
    displayConditions(Temperature, Humidity, Pressure);
    delay(5000);
  }
}

void getWeatherData() {
  if (client.connect(servername, 80)) {
    client.println("GET /data/2.5/weather?id=" + CityID + "&units=metric&APPID=" + APIKEY);
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  } 
  else {
    Serial.println("Connection failed");
    Serial.println();
  }

  while(client.connected() && !client.available()) delay(1);
  while (client.connected() || client.available()) {
    char c = client.read();
    result = result + c;
  }

  client.stop();
  result.replace('[', ' ');
  result.replace(']', ' ');
  Serial.println(result);

  DynamicJsonDocument json_doc(1024);
  DeserializationError error = deserializeJson(json_doc, result);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
  } else {
    String location = json_doc["name"].as<String>();
    String country = json_doc["sys"]["country"].as<String>();
    float temperature = json_doc["main"]["temp"].as<float>();
    float humidity = json_doc["main"]["humidity"].as<float>();
    String description = json_doc["weather"][0]["description"].as<String>();
    float pressure = json_doc["main"]["pressure"].as<float>();

    weatherDescription = description;
    weatherLocation = location;
    Country = country;
    Temperature = temperature;
    Humidity = humidity;
    Pressure = pressure;
  }
}

void displayWeather(String location, String description) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(location);
  lcd.print(", ");
  lcd.print(Country);
  lcd.setCursor(0, 1);
  lcd.print(description);
}

void displayConditions(float Temperature, float Humidity, float Pressure) {
  lcd.clear();
  lcd.print("S: "); 
  lcd.print(Temperature, 1);
  lcd.print((char)223);
  lcd.print("C ");
 
  // Printing Humidity
  lcd.print(" H:");
  lcd.print(Humidity, 0);
  lcd.print(" %");
 
  // Printing Pressure
  lcd.setCursor(0, 1);
  lcd.print("P: ");
  lcd.print(Pressure, 1);
  lcd.print(" hPa");
}

void displayGettingData() {
  lcd.clear();
  lcd.print("Getting Data");
}
