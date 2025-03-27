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


#define NUM_COEFFS 100 ///< Número de coeficientes do filtro
// Coeficientes do filtro FIR (exemplo: filtro média móvel de 5 pontos)
static float firCoeffs[NUM_COEFFS] = {-0.000443, -0.000289, -0.000061, 0.000210, 0.000485, 0.000707, 0.000813, 0.000745, 0.000469, -0.000007, -0.000620, -0.001249, -0.001732, -0.001900, -0.001621, -0.000850, 0.000341, 0.001748, 0.003060, 0.003914, 0.003987, 0.003081, 0.001207, -0.001376, -0.004173, -0.006535, -0.004173, -0.006535, -0.007789, -0.007400, -0.005132, -0.001163, 0.003882, 0.009000, 0.012971, 0.014614, 0.013063, 0.008027, -0.000039, -0.009889, -0.019613, -0.026908, -0.029466, -0.025415, -0.013725, 0.005506, 0.030939, 0.060104, 0.089715, 0.116149, 0.136004, 0.146650, 0.146650, 0.136004, 0.116149, 0.089715, 0.060104, 0.030939, 0.005506, -0.013725, -0.025415, -0.029466, -0.026908, -0.019613, -0.009889, -0.000039, 0.008027, 0.013063, 0.014614, 0.012971, 0.009000, 0.003882, -0.001163, -0.005132, -0.007400, -0.007789, -0.006535, -0.004173, -0.001376, 0.001207, 0.003081, 0.003987, 0.003914, 0.003060, 0.001748, 0.000341, -0.000850, -0.001621, -0.001900, -0.001732, -0.001249, -0.000620, -0.000007, 0.000469, 0.000745, 0.000813, 0.000707, 0.000485, 0.000210, -0.000061, -0.000289, -0.000443};
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