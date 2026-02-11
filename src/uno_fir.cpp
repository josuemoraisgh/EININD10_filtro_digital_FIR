/**
 * @file uno_fir.cpp
 * @brief Exemplo de utilização do filtro FIR implementado em C puro.
 *
 * Este sketch demonstra como utilizar a biblioteca de filtro FIR em C puro para processar
 * um sinal de entrada (por exemplo, de um sensor analógico) e obter a saída filtrada.
 */

#include <Arduino.h>
#include "fir_filter.h"

#define pinANALOG A5 // Configura o pino de leitura
uint32_t timeDelayMS = 10;
uint32_t expiresDelayMS = 0;


#define NUM_COEFFS 58 ///< Número de coeficientes do filtro
// Coeficientes do filtro FIR (exemplo: filtro média móvel de 5 pontos)
static float firCoeffs[NUM_COEFFS] = {0.000476, 0.000776, -0.000694, -0.000956, 0.001170, 0.001281, -0.002020, -0.001673, 0.003360, 0.002012, -0.005305, -0.002139, 0.007967, 0.001850, -0.011469, -0.000875, 0.015978, -0.001152, -0.021777, 0.004802, 0.029442, -0.011126, -0.040347, 0.022566, 0.058499, -0.047000, -0.101124, 0.134235, 0.463245, 0.463245, 0.134235, -0.101124, -0.047000, 0.058499, 0.022566, -0.040347, -0.011126, 0.029442, 0.004802, -0.021777, -0.001152, 0.015978, -0.000875, -0.011469, 0.001850, 0.007967, -0.002139, -0.005305, 0.002012, 0.003360, -0.001673, -0.002020, 0.001281, 0.001170, -0.000956, -0.000694, 0.000776, 0.000476};// Buffer estático para armazenar as últimas amostras
static float firBuffer[NUM_COEFFS];
// Estrutura que representa o filtro FIR
static FIRFilter myFIRFilter;


void setup()
{
  Serial.begin(9600);
  // Inicializa o filtro FIR com os coeficientes e o buffer estático
  FIRFilter_init(&myFIRFilter, firBuffer, firCoeffs, NUM_COEFFS);
}

void loop()
{
  if ((millis() - expiresDelayMS) >= timeDelayMS)
  {
    expiresDelayMS = millis();
    // Lê uma amostra do sensor (exemplo: sensor analógico) e normaliza o valor
    float inputSample = analogRead(pinANALOG) / 1023.0f;
    // Processa a amostra através do filtro FIR
    float filteredOutput = FIRFilter_process(&myFIRFilter, inputSample);
    // Exibe o valor filtrado no Monitor Serial
    Serial.print(">graf:");
    Serial.print(expiresDelayMS);
    Serial.print(":");
    Serial.print(filteredOutput);
    Serial.println("|g");
  }
}