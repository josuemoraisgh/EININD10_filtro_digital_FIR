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
#define NUM_COEFFS 100 ///< Número de coeficientes do filtro
// Coeficientes do filtro FIR (exemplo: filtro média móvel de 5 pontos)
static float firCoeffs[NUM_COEFFS] = {-0.000388, -0.000518, -0.000542, -0.000441, -0.000213, 0.000121, 0.000504, 0.000852, 0.001058, 0.001024, 0.000686, 0.000054, -0.000775, -0.001615, -0.002225, -0.002365, -0.001874, -0.000731, 0.000899, 0.002672, 0.004121, 0.004767, 0.004251, 0.002462, -0.000381, -0.003718, -0.006743, -0.008572, -0.008468, -0.006073, -0.001573, 0.004249, 0.010107, 0.014454, 0.015834, 0.013269, 0.006597, -0.003332, -0.014665, -0.024793, -0.030794, -0.030010, -0.020637, -0.002197, 0.024202, 0.055914, 0.089101, 0.119333, 0.142344, 0.154773, 0.154773, 0.142344, 0.119333, 0.089101, 0.055914, 0.024202, -0.002197, -0.020637, -0.030010, -0.030794, -0.024793, -0.014665, -0.003332, 0.006597, 0.013269, 0.015834, 0.014454, 0.010107, 0.004249, -0.001573, -0.006073, -0.008468, -0.008572, -0.006743, -0.003718, -0.000381, 0.002462, 0.004251, 0.004767, 0.004121, 0.002672, 0.000899, -0.000731, -0.001874, -0.002365, -0.002225, -0.001615, -0.000775, 0.000054, 0.000686, 0.001024, 0.001058, 0.000852, 0.000504, 0.000121, -0.000213, -0.000441, -0.000542, -0.000518, -0.000388 };
// Buffer estático para armazenar as últimas amostras
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