//includes
#include <Arduino.h>
#include <stdio.h>
#include <ESP8266WiFi.h>

//defines
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

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