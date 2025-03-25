#ifndef FIR_FILTER_H
#define FIR_FILTER_H

#include <stdint.h>

/**
 * @file fir_filter.h
 * @brief Biblioteca de filtro digital FIR em C puro.
 *
 * Este arquivo contém a declaração das funções e da estrutura utilizada para implementar
 * um filtro digital do tipo FIR (Finite Impulse Response) utilizando arrays de tamanho fixo,
 * sem o uso de alocação dinâmica de memória.
 */

/**
 * @brief Estrutura que representa um filtro FIR.
 *
 * A estrutura FIRFilter armazena os coeficientes do filtro, o buffer circular que guarda as
 * últimas amostras e o índice atual do buffer.
 */
typedef struct {
    float *buffer;       ///< Ponteiro para o array que armazena as amostras.    
    float *coeffiArray;  ///< Ponteiro para o array de coeficientes do filtro.
    uint8_t coeffiSize;  ///< Número de coeficientes (taps) do filtro.
    uint8_t index;       ///< Índice atual do buffer circular.
} FIRFilter;

/**
 * @brief Inicializa o filtro FIR.
 *
 * Esta função inicializa a estrutura FIRFilter, definindo os coeficientes, zerando o buffer
 * e configurando o número de taps. Os arrays para os coeficientes e o buffer devem ser alocados
 * estaticamente e possuir pelo menos 'taps' elementos.
 *
 * @param filter Ponteiro para a estrutura FIRFilter a ser inicializada.
 * @param buffer Array para armazenar as amostras. Deve ter 'taps' elementos.
 * @param coeffiArray Array com os coeficientes do filtro. Deve ter 'taps' elementos.* 
 * @param coeffiSize Número de coeficientes (taps) do filtro.
 */
void FIRFilter_init(FIRFilter *filter, float *buffer, float *coeffiArray, uint8_t coeffiSize) {
    filter->buffer = buffer;
    filter->coeffiArray = coeffiArray;
    filter->coeffiSize = coeffiSize;
    filter->index = 0;
    // Inicializa o buffer com zeros.
    for (uint8_t i = 0; i < coeffiSize; i++) {
        filter->buffer[i] = 0.0f;
    }
}

/**
 * @brief Processa uma nova amostra de entrada pelo filtro FIR.
 *
 * Insere a nova amostra no buffer circular e realiza a operação de convolução utilizando os
 * coeficientes do filtro para calcular a saída filtrada.
 *
 * @param filter Ponteiro para a estrutura FIRFilter.
 * @param input Nova amostra de entrada.
 * @return float Valor filtrado resultante da operação.
 */
float FIRFilter_process(FIRFilter *filter, float input) {
    // Insere a nova amostra no buffer na posição atual.
    filter->buffer[filter->index] = input;
    
    float output = 0.0f;
    uint8_t j = filter->index;

    // Realiza a operação de convolução: soma ponderada das amostras pelos coeficientes.
    for (uint8_t i = 0; i < filter->coeffiSize; i++) {
        output += filter->coeffiArray[i] * filter->buffer[j];
        // Atualiza o índice circularmente.
        if (j == 0) {
            j = filter->coeffiSize - 1;
        } else {
            j--;
        }
    }

    // Atualiza o índice do buffer para a próxima amostra.
    filter->index++;
    if (filter->index >= filter->coeffiSize) {
        filter->index = 0;
    }

    return output;
}

#endif // FIR_FILTER_H