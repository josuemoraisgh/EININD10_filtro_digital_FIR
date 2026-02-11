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


#define NUM_COEFFS 11 ///< Número de coeficientes do filtro
// Coeficientes do filtro FIR (exemplo: filtro média móvel de 5 pontos)
static float firCoeffs[NUM_COEFFS] = {-0.004678, -0.011138, 0.002427, 0.096975, 0.251133, 0.330564, 0.251133, 0.096975, 0.002427, -0.011138, -0.004678};// Buffer estático para armazenar as últimas amostras
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