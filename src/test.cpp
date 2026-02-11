#include <SoftwareSerial.h>
#include "ESP01SimulideTCP.h"

// Ligue: ESP01.TX -> D2 (RX), ESP01.RX <- D3 (TX)
SoftwareSerial esp(2,3);
SimuESP01_TCP_Min net(esp, &Serial, 8000);

const char* SSID = "APJosue";
const char* PASS = "josue32154538";
const char* HOST = "192.168.0.10";   // IP do PC onde o Python vai rodar
const uint16_t PORT = 5023;

void setup(){
  Serial.begin(115200);
  esp.begin(57600);                   // 57k6 é mais estável no SoftwareSerial
  if(!net.begin())                  { Serial.println("AT FAIL"); while(1); }
  if(!net.wifiJoin(SSID,PASS))      { Serial.println("WIFI FAIL"); while(1); }
  if(!net.connectTCP(HOST, PORT))   { Serial.println("TCP FAIL"); while(1); }
  Serial.println("Conectado. Enviando linhas para Teleplot via Python.");
}

void loop(){
  // exemplo: envia leitura em formato Teleplot: "adc: <valor>\n"
  static uint32_t t=0;
  if (millis()-t >= 10){ // ~100 Hz
    int v = analogRead(A0);
    String line = "adc: " + String(v) + "\n";
    net.print(line);
    t = millis();
  }
  net.poll(); // se vier algo do PC, fica disponível (não usado aqui)
  while (net.available()) (void)net.read();
}
