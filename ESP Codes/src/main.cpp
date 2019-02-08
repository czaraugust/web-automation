
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Ticker.h>
//#include "DHT.h"

// Set these to run example.
#define FIREBASE_HOST "automacaoresidencial-87865.firebaseio.com"
#define FIREBASE_AUTH "1eGHV4GmQSf3VRCyvLoxulwHmZI8GUd25DciQCs2"
#define WIFI_SSID "automacao"
#define WIFI_PASSWORD "isabela16"

#define LAMP_PIN 12
#define PRESENCE_PIN

#define TRIGGER 5
#define ECHO    4

// Publique a cada 5 min
#define PUBLISH_INTERVAL 1000*5


Ticker ticker;
bool publishNewState = true;

void publish(){
  publishNewState = true;
}

void setupPins(){

  pinMode(LAMP_PIN, OUTPUT);
  digitalWrite(LAMP_PIN, LOW);




}

void setupWifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
}

void setupFirebase(){
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.setBool("lamp", false);
  Firebase.setBool("presence", false);
  if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());
      return;
  }
}

void setup() {
  Serial.begin(9600);

  setupPins();
  setupWifi();

  setupFirebase();
  pinMode(TRIGGER, OUTPUT);
 pinMode(ECHO, INPUT);
 pinMode(BUILTIN_LED, OUTPUT);

  // Registra o ticker para publicar de tempos em tempos
  ticker.attach_ms(PUBLISH_INTERVAL, publish);


}

void loop() {

  // Apenas publique quando passar o tempo determinado
  if(publishNewState){
    Serial.println("Publish new State");
    // Obtem os dados do sensor DHT

    // //if(!isnan(humidity) && !isnan(temperature)){
    //   // Manda para o firebase

      long duration, distance;
      digitalWrite(TRIGGER, LOW);
      delayMicroseconds(2);

      digitalWrite(TRIGGER, HIGH);
      delayMicroseconds(10);

      digitalWrite(TRIGGER, LOW);
      duration = pulseIn(ECHO, HIGH);
      distance = (duration/2) / 29.1;
      Serial.println(distance);

       Firebase.pushFloat("distancia", distance);
       if (distance <20){
         Firebase.setBool("presence", true);

       }
       else{
         Firebase.setBool("presence", false);
       }

    //   publishNewState = false;
    // }else{
    //   Serial.println("Error Publishing");
    // }
  }

  // Verifica o valor do sensor de presença
  // LOW sem movimento
  // HIGH com movimento
  // int presence = digitalRead(PRESENCE_PIN);
  // Firebase.setBool("presence", presence == HIGH);

  // Verifica o valor da lampada no firebase
  bool lampValue = Firebase.getBool("lamp");
  Serial.println(lampValue);
  digitalWrite(LAMP_PIN, lampValue ? HIGH : 0);

  delay(200);
}
