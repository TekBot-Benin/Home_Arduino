#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"

#include "addons/RTDBHelper.h"

#define WIFI_SSID "momodiaby"
#define WIFI_PASSWORD "mrlavilaine"

#define API_KEY "AIzaSyAQVAi05r5gw-sBU_dcKLpQsDB6dzA01Ns"
#define DATABASE_URL "https://maison-connectee-990b6-default-rtdb.europe-west1.firebasedatabase.app/" 

class Led {
  public:
    int pin;
    char *path;
};

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

bool signupOK = false;

int count = 0;

Led led1  = {0, "Led1Status"};
Led led2  = {1, "Led2Status"};
Led led3  = {2, "Led3Status"};
Led led4  = {3, "Led4Status"};

const int SIG = 2;
const int EN = 7;
const int controlPin[4] = {3, 4, 5, 6};

int loopDelay = 1000;

const int muxTable[16][4] = {
  // s0, s1, s2, s3     channel
  {0,  0,  0,  0}, // 0
  {1,  0,  0,  0}, // 1
  {0,  1,  0,  0}, // 2
  {1,  1,  0,  0}, // 3
  {0,  0,  1,  0}, // 4
  {1,  0,  1,  0}, // 5
  {0,  1,  1,  0}, // 6
  {1,  1,  1,  0}, // 7
  {0,  0,  0,  1}, // 8
  {1,  0,  0,  1}, // 9
  {0,  1,  0,  1}, // 10
  {1,  1,  0,  1}, // 11
  {0,  0,  1,  1}, // 12
  {1,  0,  1,  1}, // 13
  {0,  1,  1,  1}, // 14
  {1,  1,  1,  1}  // 15
};

void writeOnChannel(int channel, int to_write)
{
  digitalWrite(controlPin[0], muxTable[channel][0]);
  digitalWrite(controlPin[1], muxTable[channel][1]);
  digitalWrite(controlPin[2], muxTable[channel][2]);
  digitalWrite(controlPin[3], muxTable[channel][3]);
  digitalWrite(SIG, to_write);
}

void LedState (Led led)
{
  if (Firebase.RTDB.getInt(&fbdo, led.path)) {
      if (fbdo.dataType() == "int") {
        int intValue = fbdo.intData();
        writeOnChannel(led.pin, intValue);
      }
   }
}

void allLights(void)
{
  LedState(led1);
  LedState(led2);
  LedState(led3);
  LedState(led4);
}

void setWifi() {
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
}

void setFirebase() {
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

void setup() {
  setWifi();
  setFirebase();
  for (int i = 0; i < 4; i++)
  {
    pinMode(controlPin[i], OUTPUT);
    digitalWrite(controlPin[i], HIGH);
  }
  pinMode(SIG, OUTPUT);
  digitalWrite(SIG, LOW);

  pinMode(EN, OUTPUT);
  digitalWrite(EN, LOW);
  Serial.begin(9600);
}

void loop() 
{
  if (Firebase.ready() && signupOK){
    allLights();
  }
}
