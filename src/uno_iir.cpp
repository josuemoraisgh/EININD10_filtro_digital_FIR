/**
 * @file uno_iir.cpp
 * @brief Exemplo de utilização do filtro IIR implementado em C puro.
 *
 * Este sketch demonstra como utilizar a biblioteca de filtro IIR para processar um sinal de entrada
 * (por exemplo, de um sensor analógico) e obter a saída filtrada.
 */

 #include <Arduino.h>
 
 #include "iir_filter.h"
 
 // Definição dos números de coeficientes para os filtros feedforward (b) e feedback (a)
 #define NB_COEFFS 11  ///< Número de coeficientes feedforward (numerador) + 1
 #define NA_COEFFS 11  ///< Número de coeficientes feedback (denominador) + 1 
 #define IIR_ORDER ((NB_COEFFS > NA_COEFFS ? NB_COEFFS : NA_COEFFS) - 1)// Ordem do filtro = max(NB_COEFFS, NA_COEFFS) - 1.
 #define pinANALOG A5 // Configura o pino de leitura
 
 uint32_t timeDelayMS = 10;
 uint32_t expiresDelayMS = 0;
 
 // Exemplo de coeficientes para um filtro IIR (a[0] deve ser 1)
 // Estes coeficientes podem representar, por exemplo, um filtro passa-baixas
 static float bCoeffs[NB_COEFFS] = {0.000000, 0.000001, 0.000003, 0.000009, 0.000016, 0.000019, 0.000016, 0.000009, 0.000003, 0.000001, 0.000000};
 static float aCoeffs[NA_COEFFS] = {1.000000, -7.195702, 23.601485, -46.402544, 60.494341, -54.593176, 34.512482, -15.081737, 4.357580, -0.751330, 0.058678};
 
 // Vetor de estado para o filtro (tamanho = IIR_ORDER)
 static float iirState[IIR_ORDER];
 
 // Estrutura que representa o filtro IIR
 static IIRFilter myIIRFilter;
 
 void setup() {
     Serial.begin(9600);
     // Inicializa o filtro IIR com os coeficientes e o vetor de estado
     IIRFilter_init(&myIIRFilter, iirState, bCoeffs, aCoeffs, NB_COEFFS, NA_COEFFS);
 }
 
 void loop() {
     if ((millis() - expiresDelayMS) >= timeDelayMS)
     {
       expiresDelayMS = millis();
       // Lê uma amostra do sensor (exemplo: sensor analógico) e normaliza o valor
       float inputSample = analogRead(pinANALOG) / 1023.0f;
       // Processa a amostra através do filtro IIR
       float filteredOutput = IIRFilter_process(&myIIRFilter, inputSample);
       // Exibe o valor filtrado no Monitor Serial
       Serial.print(">graf:");
       Serial.print(expiresDelayMS);
       Serial.print(":");
       Serial.print(filteredOutput);
       Serial.println("|g");
     }
 }