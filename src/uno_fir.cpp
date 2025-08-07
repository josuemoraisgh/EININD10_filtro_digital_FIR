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
uint32_t timeDelayMS = 625;
uint32_t expiresDelayMS = 0;


#define NUM_COEFFS 11 ///< Número de coeficientes do filtro
// Coeficientes do filtro FIR (exemplo: filtro média móvel de 5 pontos)
static float firCoeffs[NUM_COEFFS] = {-0.00383703, -0.01252527, -0.00503055,  0.08976579,  0.25798701, 0.3472801 ,  0.25798701,  0.08976579, -0.00503055, -0.01252527, -0.00383703};// Buffer estático para armazenar as últimas amostras
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
  if ((micros() - expiresDelayMS) >= timeDelayMS)
  {
    expiresDelayMS = micros();
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