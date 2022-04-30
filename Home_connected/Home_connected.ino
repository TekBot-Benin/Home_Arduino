#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"

#include "addons/RTDBHelper.h"

#define WIFI_SSID "flemme02"
#define WIFI_PASSWORD "M22062002e"

#define API_KEY "AIzaSyDtQcMZIgc7ZyxMUCEeF-nXfi3_-PfrJkg"
#define DATABASE_URL "https://console.firebase.google.com/u/0/project/my-home-d35d6/database/my-home-d35d6-default-rtdb/data/~2F/" 

#include <ShiftRegister74HC595.h>

ShiftRegister74HC595<1> sr(21, 22, 23);

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

bool signupOK = false;

int count = 0;

#define LED1 "Led1Status"
#define LED2 "Led2Status"
#define LED3 "Led3Status"
#define LED4 "Led4Status"

void setWifi() {
  
}

void setFirebase() {
  
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
    if (Firebase.ready() && signupOK){
    if (Firebase.RTDB.getString(&fbdo, "Led1Status")) {
      if (fbdo.dataType() == "string") {
        int intValue = fbdo.intData();
        sr.setAllHigh(); // set all pins HIGH
        sr.set(0, LOW);
        Serial.println(intValue);
      }
    }
    if (Firebase.RTDB.getInt(&fbdo, LED2)) {
      if (fbdo.dataType() == "int") {
        int intValue = fbdo.intData();
        Serial.println(intValue);
      }
    }
    if (Firebase.RTDB.getInt(&fbdo, LED3)) {
      if (fbdo.dataType() == "int") {
        int intValue = fbdo.intData();
        Serial.println(intValue);
      }
    }
    if (Firebase.RTDB.getInt(&fbdo, LED4)) {
      if (fbdo.dataType() == "int") {
        int intValue = fbdo.intData();
        Serial.println(intValue);
      }
    }
}
}
