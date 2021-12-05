//includes
#include <Arduino.h>
#include <stdio.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <BlueDot_BME280.h>
#include "DHTesp.h"
#include "OneWire.h"
#include <DallasTemperature.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

//defines
#define WIFI_SSID "ARRIS-85BF"
#define WIFI_PASSWORD "F56548EE0798580A"
#define EMAIL ""
#define PASSWORD ""

#define API_KEY "AIzaSyD0U8kw0nuIvuHegfdQAFug6IakObHsZM4"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://esp-firebase-c201c-default-rtdb.europe-west1.firebasedatabase.app/"

#define ADC_LH_DEF (100.0f / 1024.0f)
//Firebase Data Objects
FirebaseData data;
FirebaseAuth auth;
FirebaseConfig config;

//variables
bool isFirebaseConnected;
unsigned long sendDataPrevMillis = 0;
int c;
float temp = 0.f;
float temp1 = 0.f;
float pres = 0.f;
float att = 0.f;
uint8_t DHTPin = D7;
uint8_t WaterSensorPin = A0;
uint8_t WaterSensorPowerPin = D5;
uint8_t DallasPin = D6;

//Object of the class BluBlueDot_BME280, instances of sensor bme280
BlueDot_BME280 bme280 = BlueDot_BME280();
DHTesp dht;

//Objects of the class DallasTemperature instances of sensor DS18B20
OneWire oneWire(DallasPin);
DallasTemperature sensors(&oneWire);

void setup()
{
  //######BASIC SETTINGS##########
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
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
  /*BMP280*/
  Serial.println(F("BME280 test"));
  Wire.begin(0x76);                   //sda scl init I2C interface on 0x76 address (addres of sensor)
  bme280.parameter.communication = 0; //communication protocol set as I2C
  bme280.parameter.I2CAddress = 0x76; //Choose I2C Address
  bme280.parameter.sensorMode = 0b11; //Choose sensor mode as NORMAL
  /*The IIR (Infinite Impulse Response) filter suppresses high frequency fluctuations
  In short, a high factor value means less noise, but measurements are also less responsive*/
  bme280.parameter.IIRfilter = 0b100;          //Setup for IIR Filter
  bme280.parameter.tempOversampling = 0b101;   //Setup Temperature Ovesampling
  bme280.parameter.pressOversampling = 0b101;  //Setup Pressure Ovesampling
  bme280.parameter.tempOutsideCelsius = 15;    //default value of 15°C
  bme280.parameter.pressureSeaLevel = 1013.25; // pressure on sea level in Poland has 1013,25 hPa
  bme280.init();
  /*DHT11*/
  dht.setup(DHTPin, DHTesp::DHT11); //
  /*OneWire*/
  sensors.begin();
  /*Water Sensor*/
  pinMode(WaterSensorPowerPin, OUTPUT); //power the sensor only when reading data
  digitalWrite(WaterSensorPowerPin, LOW);
}

void loop()
{
  if (Firebase.ready() && isFirebaseConnected && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();
    /*BME280 READOUT*/
    if (Firebase.RTDB.setFloat(&data, "BME280/Temp *C", temp) && Firebase.RTDB.setFloat(&data, "BME280/Temp F", temp1) && Firebase.RTDB.setFloat(&data, "BME280/Pressure hPa", pres) && Firebase.RTDB.setFloat(&data, "BME280/Altitude m", att))
    {
      temp = bme280.readTempC();
      temp1 = bme280.readTempF();
      pres = bme280.readPressure();
      att = bme280.readAltitudeMeter();

      Serial.println("------BME280 readout------");
      Serial.print("Temp in *C : \t");
      Serial.print(temp);
      Serial.print("\tTemp in F : ");
      Serial.print(temp1);
      Serial.print("\tPressure: ");
      Serial.print(pres);
      Serial.print("\tAltitude: ");
      Serial.print(att);
      Serial.println("\t\t->SEND!");
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + data.errorReason());
    }
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();
    if (Firebase.RTDB.setFloat(&data, "DHT11/Temp C", temperature) && Firebase.RTDB.setFloat(&data, "DHT11/Humidity", humidity))
    {
      Serial.println("------DHT11 readout------");
      Serial.print("Humidity (%): ");
      Serial.print(humidity);
      Serial.print("\t");
      Serial.print("Temperature (C): ");
      Serial.print(temperature);
      Serial.print("\t");
      Serial.println();
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + data.errorReason());
    }

    sensors.requestTemperatures();
    float tempDallasC = sensors.getTempCByIndex(0);
    float tempDallasF = sensors.getTempFByIndex(0);

    if (Firebase.RTDB.setFloat(&data, "Dallas DS12B20/Temp C", tempDallasC) && Firebase.RTDB.setFloat(&data, "Dallas DS12B20/Temp F", tempDallasF))
    {
      Serial.println("------Dallas readout------");
      Serial.print("Temp in *C : \t");
      Serial.print(tempDallasC);
      Serial.print("ºC");
      Serial.print("\tTemp in F : ");
      Serial.print(tempDallasF);
      Serial.println("ºF");
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + data.errorReason());
    }

    digitalWrite(WaterSensorPowerPin, HIGH);
    delay(10);
    int waterValue = 0;
    waterValue = analogRead(WaterSensorPin);
    digitalWrite(WaterSensorPowerPin, LOW);

    if (Firebase.RTDB.setFloat(&data, "WaterSensor/ADC Readout value", waterValue))
    {
      Serial.println("------Water Sensor readout------");
      Serial.print("Water sensor value: ");
      Serial.println(waterValue);
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + data.errorReason());
    }
    delay(100);
    int lightness = 0;
    lightness = ADC_LH_DEF * analogRead(WaterSensorPin);

    if (Firebase.RTDB.setFloat(&data, "Fotoresistor/Ligntness percent", lightness))
    {
      Serial.println("------Fotoresistor readout------");
      Serial.print("Fotoresistor value: ");
      Serial.print(lightness);
      Serial.println("%");
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + data.errorReason());
    }
  }
}