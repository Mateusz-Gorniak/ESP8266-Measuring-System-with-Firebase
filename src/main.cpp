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
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
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

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  //Initialaize Serial
  Serial.begin(115200);
  //Initialize Wi-Fi
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
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
  Serial.println(WiFi.getHostname());

}

void loop()
{
  // put your main code here, to run repeatedly:
}