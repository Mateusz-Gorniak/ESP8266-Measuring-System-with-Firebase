//includes
#include <Arduino.h>
#include <stdio.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

//defines
#define WIFI_SSID "ARRIS-85BF"
#define WIFI_PASSWORD "F56548EE0798580A"
#define EMAIL ""
#define PASSWORD ""
// Insert Firebase project API Key
#define API_KEY "AIzaSyCb8ebThdsGcFKk0XHxxmYez177m-a-pXA"
// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://esp-firebase-1903d-default-rtdb.europe-west1.firebasedatabase.app/"

//Firebase Data Objects
FirebaseData data;
FirebaseAuth auth;
FirebaseConfig config;

//variables
bool isFirebaseConnected;
unsigned long sendDataPrevMillis = 0;
int c;

void setup()
{
  //######BASIC SETTINGS##########
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  //Initialaize Serial
  Serial.begin(115200);
  //Initialize Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  //Checking the Wi-Fi connection
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(200);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.getMode());
  Serial.println();
  //######FIREBASE SETTINGS##########
  //write parameters to Firebase config structure
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  //Firebase authentication - default set as Anonymous
  isFirebaseConnected = Firebase.signUp(&config, &auth, "", "");

  if (isFirebaseConnected)
  {
    Serial.println("Connection with Firebase succeed!");
  }
  else
  {
    Serial.println("Connection failed.");
  }
  config.token_status_callback = tokenStatusCallback;
  //Firebase init
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  //######SENSORS SETTINGS##########
  //
}

void loop()
{
  if (Firebase.ready() && isFirebaseConnected && (millis() - sendDataPrevMillis > 6000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();
    if (Firebase.RTDB.setFloat(&data, "Test var", c++))
    {
      Serial.println("PASSED\t");
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + data.errorReason());
    }
  }
}