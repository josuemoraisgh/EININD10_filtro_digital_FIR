import numpy as np
from scipy.signal import butter, freqz
from matplotlib.pyplot import subplots, show
from findAnalogFilterByTargetFreq import findAnalogFilterByTargetFreq

def findDigitalFilterIIRByTargetFreq(fDesejada, ordem, fs, filterType, desvio, isBP):
    """
    Calcula os coeficientes de um filtro IIR digital utilizando a frequência de corte 
    de um filtro analógico Butterworth, plota a resposta em frequência com:
    - Frequência desejada
    - Frequência de corte
    - Borda da banda de interesse
    - Borda da banda oposta (frequência complementar)

    Retorna:
      b, a : Coeficientes do filtro IIR.
    """
    # Obtém frequência de corte, borda da banda e complementar
    fc_analog, freq_limite, f_complementar_hz = findAnalogFilterByTargetFreq(
        fDesejada, ordem, filterType, desvio, isBP
    )

    # Frequência de corte normalizada
    nyq = 0.5 * fs
    normal_fc = fc_analog / nyq

    # Coeficientes do filtro digital IIR Butterworth
    b, a = butter(ordem, normal_fc, btype=filterType)

    # Resposta em frequência
    w, h = freqz(b, a, worN=2048)
    freqs_hz = w * fs / (2 * np.pi)
    h_mag = np.abs(h)

    # Plot
    fig, ax = subplots(figsize=(14, 5))
    ax.plot(freqs_hz, h_mag, label='Resposta do Filtro IIR', color='blue')
    ax.set_xlabel('Frequência (Hz)')
    ax.set_ylabel('Magnitude')
    ax.set_title('Filtro IIR - Resposta em Frequência')

    # Linhas verticais
    print("Frequência Desejada [Amarelo]:", fDesejada)
    ax.axvline(fDesejada, color='yellow', linestyle='--', label='Frequência Desejada')

    print("Frequência de Corte [Verde]:", fc_analog)
    ax.axvline(fc_analog, color='green', linestyle='--', label='Frequência de Corte')

    if isBP:
        print("Frequência limite da Banda de Passagem [Vermelho]:", freq_limite)
        ax.axvline(freq_limite, color='red', linestyle='--', label='Limite da Banda de Passagem')
        print("Frequência Complementar (Rejeição) [Roxa]:", f_complementar_hz)
        ax.axvline(f_complementar_hz, color='purple', linestyle='--', label='Limite da Banda de Rejeição')
    else:
        print("Frequência limite da Banda de Rejeição [Vermelho]:", freq_limite)
        ax.axvline(freq_limite, color='red', linestyle='--', label='Limite da Banda de Rejeição')
        print("Frequência Complementar (Passagem) [Roxa]:", f_complementar_hz)
        ax.axvline(f_complementar_hz, color='purple', linestyle='--', label='Limite da Banda de Passagem')

    # Ajuste do eixo x
    f_min_plot = max(0, min(fDesejada, fc_analog, freq_limite, f_complementar_hz) * 0.8)
    f_max_plot = min(fs / 2, max(fDesejada, fc_analog, freq_limite, f_complementar_hz) * 1.5)
    ax.set_xlim(f_min_plot, f_max_plot)
    ax.set_ylim(0, 1.1)

    ax.grid(True)
    ax.legend()
    show()

    return b, a

# Exemplo de uso:
if __name__ == '__main__':
    b, a = findDigitalFilterIIRByTargetFreq(
        fDesejada=100,         # Frequência de interesse
        ordem=10,
        fs=1600,               # Frequência de amostragem
        filterType='lowpass',   # 'lowpass' ou 'highpass'
        desvio=0.05,
        isBP=True              # fDesejada está na banda de REJEIÇÃO
    )
    print("\nCoeficientes do numerador (b):")
    print(", ".join(f"{coef:.6f}" for coef in b))
    print("Coeficientes do denominador (a):")
    print(", ".join(f"{coef:.6f}" for coef in a))