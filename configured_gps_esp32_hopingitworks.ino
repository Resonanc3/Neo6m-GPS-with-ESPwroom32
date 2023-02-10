#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#include <FirebaseESP8266.h>
#endif

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "PLDTHOMEFIBR8fab0"
#define WIFI_PASSWORD "PLDTWIFIt27q5"

//FBI Channel 90210
//PWNHw5j_48=3$sKv

#define API_KEY "AIzaSyByLvbyyMDj1_2CFMv-1CxgkXxDsPKAyik"
#define DATABASE_URL "https://firstproject-eb079-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
static const int RXPin = 16, TXPin = 17;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  Serial.begin(9600);
  ss.begin(GPSBaud);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi... ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else
  {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop()
{
  smartdelay_gps(1000);

  if (gps.location.isValid())
  {
    float latitude = gps.location.lat();
    float longitude = gps.location.lng();
    String beef_name = "beef_1";

    uint32_t time_value = gps.time.value();
    uint32_t date_value = gps.date.value();    

    if(Firebase.setFloat(fbdo, "/GPS/01/data/location/f_latitude", latitude))
    {print_ok();}
    else
    {print_fail();}

    if(Firebase.setFloat(fbdo, "/GPS/01/data/location/f_longitude", longitude))
    {print_ok();}
    else
    {print_fail();}

    if(Firebase.setString(fbdo, "/GPS/01/data/tag_name", beef_name))
    {print_ok();}
    else
    {print_fail();}

    if(Firebase.setInt(fbdo, "/GPS/01/data/time", time_value))
    {print_ok();}
    else
    {print_fail();}

    if(Firebase.setInt(fbdo, "GPS/01/data/date", date_value))
    {print_ok();}
    else
    {print_fail();}
  }
  else
  {
    Serial.println("No valid GPS data found");
  }   
}

static void smartdelay_gps(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

void print_ok()
{
    Serial.println("------------------------------------");
    Serial.println("OK");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
    Serial.println("ETag: " + fbdo.ETag());
    Serial.println("------------------------------------");
    Serial.println();
}

void print_fail()
{
    Serial.println("------------------------------------");
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
}