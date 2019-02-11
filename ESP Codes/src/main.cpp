// INCLUINDO BIBLIOTECAS


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Ticker.h>
//#include "DHT.h"

/// DEFININDO PARÂMETROS DO FIREABSE
#define FIREBASE_HOST "automacaoresidencial-87865.firebaseio.com"
#define FIREBASE_AUTH "1eGHV4GmQSf3VRCyvLoxulwHmZI8GUd25DciQCs2"
// DEFININDO A REDE WIFI QUE O ESP VAI SE CONECTAR
#define WIFI_SSID "automacao"
#define WIFI_PASSWORD "isabela16"
//https://www.filipeflop.com/blog/esp8266-nodemcu-como-programar/node-mcu-esp-12e-pin-out-diagram2-jpg/
// DEFININDO O PINO DO RELÉ.
#define LAMP_PIN 12 //D6

#define TRIGGER 5 // D1
#define ECHO    4 // D2

// Publique a cada 5 SEGUNDOS (1000 MS * 5) = 5 SEGUNDOS
#define PUBLISH_INTERVAL 1000*5

// VARIÁVEL DA BIBLIOTECA TICKER. SERVE PARA MARCAR O TEMPO
Ticker ticker;
bool publishNewState = true;

void publish(){
  publishNewState = true;
}
// FUNÇÃO PARA DEFINIR CONFIGURAÇÃO DOS PINOS
void setupPins(){
  //PINO DO RELÉ COMO SAÍDA
  pinMode(LAMP_PIN, OUTPUT);
  // COMEÇANDO COM O RELÉ DESLIGADO
  digitalWrite(LAMP_PIN, LOW);
  // DEFINE PINO DO SENSOR DE DISPARO DO SENSOR DE ULTRASSOM COMO SAÍDA
  pinMode(TRIGGER, OUTPUT);
  // DEFINE PINO DO SENSOR DE RECEPÇÃO DO SENSOR DE ULTRASSOM COMO ENTRADA
  pinMode(ECHO, INPUT);
  // LED DO ESP COMO SAÍDA
  pinMode(BUILTIN_LED, OUTPUT);




}
//FUNÇAO PARA CONECTAR NA REDE WIFI
void setupWifi(){
  //INICIA COM NOME E SENHA DA REDE
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  // SÓ SAI DESSSE WHILE QUANDO CONECTAR
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  //IMPRIME O IP DA REDE
  Serial.println(WiFi.localIP());
}
// FUNÇAO DE CONFIGURAR FIREBASE
void setupFirebase(){
  //INICIAR COM HOST E SENHA
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //CRIA AS VARIÁVEIS DA LAMPADA E DO SENSOR DE PRESENÇA NO BANCO
  Firebase.setBool("lamp", false);
  Firebase.setBool("presence", false);
  // SE DER ERRO IMPRIME O TIPO DE ERRO
  if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());
      return;
  }
}

void setup() {
  //DEFINE A VELOCIDADE DA CONEXÃO SERIAL
  Serial.begin(9600);
  //CHAMA A FUNÇÃO DE CONFIGURAR
  setupPins();
  //CHAMA A FUNÇÃO DE CONECTAR COM O WIFI
  setupWifi();
  // CHAMA A FUNCAO DE CONECTAR COM O FIREBASE
  setupFirebase();


  // REGISTRA A FUNÇAO DE CONTAR O TEMPO
  ticker.attach_ms(PUBLISH_INTERVAL, publish);


}

void loop() {

  // Apenas publique quando passar o tempo determinado
  if(publishNewState){
    Serial.println("Publish new State");
    // Obtem os dados do sensor DHT

      //VARIAVEIS DO SENSOR DE PRESENÇA
      long duration, distance;
      digitalWrite(TRIGGER, LOW);
      delayMicroseconds(2);

      digitalWrite(TRIGGER, HIGH);
      //PULSO DE 10 MICRO SEGUNDOS
      delayMicroseconds(10);

      digitalWrite(TRIGGER, LOW);
      // QUANTO TEMPO LEVOU PARA O PINO ECHO RECEBER O PULSO
      duration = pulseIn(ECHO, HIGH);
      //29.4 ms para o som percorrer um centímetro. DIVIDE POR 2 POR QUE É IDA E VOLTA E QUEREMOS SABER APENAS A DISTÂNCIA
      distance = (duration/2) / 29.4;
      //DISTÂNCIA EM CM
      Serial.println(distance);

      //MANDA PARA O FIREBASE A DISTÂNCIA EM QUE SE ENCONTRA O OBJETO
       Firebase.pushFloat("distancia", distance);
       // SE ESTIVER A MENOS DE 20 CM CONFIRMA A PRESENÇA DE ALGO
       if (distance <20){
         //ENVIA PARA O FIREBASE
         Firebase.setBool("presence", true);

       }
       else{
         //SENÃO NÃO CONFIRMA PRESENÇAO
         Firebase.setBool("presence", false);
       }


  }


  // Verifica o valor dO INTERRUPTOR no firebase
  bool lampValue = Firebase.getBool("lamp");
  Serial.println(lampValue);
  //SE ESTIVER ACIONADO. LIGA A LÂMPADA (HIGH) , SENÃO PERMANECE DESLIGADA (LOW)
  digitalWrite(LAMP_PIN, lampValue ? HIGH : LOW);
  delay(10);
}
