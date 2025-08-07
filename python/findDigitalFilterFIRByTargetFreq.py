import numpy as np
from matplotlib.pyplot import subplots, show
from scipy.signal import firwin, freqz

def findDigitalFilterFIRByTargetFreq(fDesejada, ordem, fs, filterType="lowpass", desvio=0.05, isBP=True):
    if not (0 < fDesejada < fs / 2):
        raise ValueError(f"fDesejada={fDesejada} Hz deve estar entre 0 e fs/2 ({fs/2} Hz).")

    M_target = 1 - desvio if isBP else desvio

    # Define a faixa de busca de fc de acordo com o tipo e banda
    epsilon = 1e-3
    nyq = fs / 2
    if (filterType == "lowpass" and isBP) or (filterType == "highpass" and not isBP):
        fc_range = np.linspace(fDesejada, nyq - epsilon, 10000)
    else:
        fc_range = np.linspace(1, min(fDesejada, nyq - epsilon), 10000)

    melhor_fc = None
    melhor_erro = np.inf

    for fc in fc_range:
        taps = firwin(numtaps=ordem, cutoff=fc, fs=fs, window="hamming", pass_zero=filterType)
        w, h = freqz(taps, worN=4096)
        f_hz = w * fs / (2 * np.pi)
        h_mag = np.abs(h)

        # Erro em relação à magnitude desejada em fDesejada
        idx = np.argmin(np.abs(f_hz - fDesejada))
        mag = h_mag[idx]
        erro = abs(mag - M_target)

        if erro < melhor_erro:
            melhor_erro = erro
            melhor_fc = fc

    fc_encontrado = melhor_fc

    # Recalcula com fc encontrado
    taps = firwin(numtaps=ordem, cutoff=fc_encontrado, fs=fs, window="hamming", pass_zero=filterType)
    w, h = freqz(taps, worN=4096)
    f_hz = w * fs / (2 * np.pi)
    h_mag = np.abs(h)

    # Encontra f_banda e f_comp com erro mínimo
    if isBP:
        erro_banda = np.abs(h_mag - (1 - desvio))
        erro_comp  = np.abs(h_mag - desvio)
    else:
        erro_banda = np.abs(h_mag - desvio)
        erro_comp  = np.abs(h_mag - (1 - desvio))

    idx_banda = np.argmin(erro_banda)
    idx_comp  = np.argmin(erro_comp)

    f_banda = f_hz[idx_banda]
    f_comp  = f_hz[idx_comp]

    # Impressões
    print("✅ Resultado:")
    print("Frequência Desejada [Amarelo]:", fDesejada)
    print("Frequência de Corte [Verde]:", fc_encontrado)
    print(("Limite da Banda de Passagem" if isBP else "Limite da Banda de Rejeição") + " [Vermelho]:", f_banda)
    print("Frequência Complementar (banda oposta) [Roxa]:", f_comp)

    # Plot
    # Gráfico com rótulos corretos
    fig, ax = subplots(figsize=(14, 5))
    ax.plot(f_hz, h_mag, label='Resposta em Frequência', color='blue')
    ax.axvline(fDesejada, color='yellow', linestyle='--', label='Frequência Desejada')
    ax.axvline(fc_encontrado, color='green', linestyle='--', label='Frequência de Corte')

    # Limite da banda de interesse (principal)
    if isBP:
        ax.axvline(f_banda, color='red', linestyle='--', label='Limite da Banda de Passagem')
        ax.axvline(f_comp, color='purple', linestyle='--', label='Limite da Banda de Rejeição')
    else:
        ax.axvline(f_banda, color='red', linestyle='--', label='Limite da Banda de Rejeição')
        ax.axvline(f_comp, color='purple', linestyle='--', label='Limite da Banda de Passagem')


    f_plot_min = max(0, min(f_banda, f_comp, fDesejada, fc_encontrado) * 0.8)
    f_plot_max = min(fs / 2, max(f_banda, f_comp, fDesejada, fc_encontrado) * 1.2)
    ax.set_xlim(f_plot_min, f_plot_max)
    ax.set_ylim(0, 1.1)
    ax.set_xlabel("Frequência (Hz)")
    ax.set_ylabel("Magnitude")
    ax.set_title("Filtro FIR - Resposta em Frequência")
    ax.grid(True)
    ax.legend()
    show()

    return taps

# Exemplo de uso
if __name__ == '__main__':
    taps = findDigitalFilterFIRByTargetFreq(
        fDesejada=100,     # Frequência alvo para rejeição ou passagem
        ordem=11,         # Ordem do filtro FIR
        fs=1600,           # Frequência de amostragem
        filterType="lowpass",  # 'lowpass' ou 'highpass'
        desvio=0.05,
        isBP=True          # False = fDesejada está na banda de rejeição
    )

    print("\nCoeficientes do filtro:")
    print(", ".join(f"{coef:.6f}" for coef in taps))
