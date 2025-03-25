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
static float firCoeffs[NUM_COEFFS] = {-0.000065, 0.000288, 0.000536, 0.000555, 0.000298, -0.000171, -0.000664, -0.000930, -0.000762, -0.000125, 0.000767, 0.001492, 0.001598, 0.000851, -0.000570, -0.002060, -0.002812, -0.002225, -0.000285, 0.002275, 0.004212, 0.004334, 0.002169, -0.001626, -0.005370, -0.007058, -0.005365, -0.000496, 0.005621, 0.010011, 0.010019, 0.004766, -0.004047, -0.012521, -0.016154, -0.012042, -0.000689, 0.013554, 0.023898, 0.024001, 0.011221, -0.011234, -0.034437, -0.046437, -0.036730, -0.000804, 0.057102, 0.124364, 0.183569, 0.218181, 0.218181, 0.183569, 0.124364, 0.057102, -0.000804, -0.036730, -0.046437, -0.034437, -0.011234, 0.011221, 0.024001, 0.023898, 0.013554, -0.000689, -0.012042, -0.016154, -0.012521, -0.004047, 0.004766, 0.010019, 0.010011, 0.005621, -0.000496, -0.005365, -0.007058, -0.005370, -0.001626, 0.002169, 0.004334, 0.004212, 0.002275, -0.000285, -0.002225, -0.002812, -0.002060, -0.000570, 0.000851, 0.001598, 0.001492, 0.000767, -0.000125, -0.000762, -0.000930, -0.000664, -0.000171, 0.000298, 0.000555, 0.000536, 0.000288, -0.000065};
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