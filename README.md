# 🍷 Mansur Pixels — Monitor de Adega Inteligente

> **Checkpoint 02 — Edge Computing | FIAP**

Sistema embarcado de monitoramento ambiental desenvolvido com Arduino, projetado para controle de temperatura, umidade e luminosidade em ambientes sensíveis (como adegas de vinho). O sistema realiza leituras contínuas dos sensores, exibe informações em display LCD e emite alertas visuais e sonoros conforme as condições do ambiente.

---

## 👥 Integrantes

| Nome | RM |
|---|---|
| Állex Augusto de Souza Brandão | RM571383 |
| Enzo Ribeiro Abreu | RM572995 |
| Erick Nathan Capito Pereira | RM573635 |
| Henrique Caruso Bueno | RM570591 |
| Murilo Vasconcelos Gomes | RM570941 |

---

## 📋 Descrição do Projeto

O sistema monitora continuamente três variáveis ambientais:

- 🌡️ **Temperatura** — via sensor DHT22
- 💧 **Umidade** — via sensor DHT22
- 💡 **Luminosidade** — via sensor LDR

Com base nas leituras (calculadas por média de 5 amostras), o sistema ativa LEDs coloridos e um buzzer para indicar o estado do ambiente, além de exibir mensagens informativas no display LCD 16x2.

---

## 🔧 Hardware Utilizado

| Componente | Quantidade |
|---|---|
| Arduino Uno (ou compatível) | 1 |
| Sensor DHT22 | 1 |
| Sensor LDR | 1 |
| Display LCD 16x2 | 1 |
| LED Verde | 1 |
| LED Amarelo | 1 |
| LED Vermelho | 1 |
| Buzzer | 1 |
| Resistores | conforme necessário |

---

## 📌 Mapeamento de Pinos

| Componente | Pino Arduino |
|---|---|
| LCD (RS, EN, D4, D5, D6, D7) | 13, 11, 9, 8, 7, 6 |
| Sensor LDR | A5 |
| LED Verde | 2 |
| LED Amarelo | 3 |
| LED Vermelho | 4 |
| Buzzer | 5 |
| Sensor DHT22 | 10 |

---

## ⚙️ Lógica de Funcionamento

### Leitura dos Sensores

A cada **5 segundos**, o sistema coleta **5 leituras** de cada sensor e calcula a **média**, minimizando ruídos e leituras espúrias.

### Faixas Ideais

| Parâmetro | Mínimo | Máximo |
|---|---|---|
| Temperatura | 10°C | 15°C |
| Umidade | 50% | 70% |

### Limiares de Luminosidade (LDR)

| Condição | Valor LDR |
|---|---|
| Muito claro | < 335 |
| Meia luz | 335 – 669 |
| Escuro (ideal) | ≥ 670 |

---

## 🚦 Sistema de Alertas (Prioridades)

O sistema segue uma hierarquia de prioridades para exibição de alertas:

```
Prioridade 1 → Erro no sensor DHT          → LED Vermelho + Buzzer
Prioridade 2 → Umidade fora da faixa       → LED Vermelho + Buzzer
Prioridade 3 → Temperatura fora da faixa   → LED Amarelo  + Buzzer
Prioridade 4 → Ambiente muito claro        → LED Vermelho + Buzzer
Prioridade 5 → Ambiente a meia luz         → LED Amarelo (sem buzzer)
Prioridade 6 → Tudo OK / Ambiente escuro   → LED Verde
```

> Quando há múltiplos alertas simultâneos, o LCD exibe "Outros alertas" na segunda linha, indicando que existem outros problemas além do principal.

---

## 📺 Comportamento do LCD

O display LCD alterna automaticamente entre telas de informação a cada ciclo de leitura (5 segundos):

- **Tela 0:** Dados do sensor principal em alerta (temperatura, umidade ou LDR)
- **Tela 1:** Informações complementares do ambiente

---

## 📦 Dependências (Bibliotecas)

Instale as seguintes bibliotecas pela Arduino IDE (Sketch → Incluir Biblioteca → Gerenciar Bibliotecas):

- [`LiquidCrystal`](https://www.arduino.cc/reference/en/libraries/liquidcrystal/) — nativa da Arduino IDE
- [`DHT sensor library`](https://github.com/adafruit/DHT-sensor-library) — by Adafruit

---

## 🚀 Como Executar

1. Clone ou baixe este repositório
2. Abra o arquivo `.ino` na **Arduino IDE**
3. Instale as bibliotecas listadas acima
4. Conecte o Arduino ao computador via USB
5. Selecione a porta correta em **Ferramentas → Porta**
6. Faça o upload do código com o botão **→ (Upload)**
7. Abra o **Monitor Serial** (9600 baud) para acompanhar as leituras em tempo real

---

## 📡 Saída Serial

A cada ciclo de leitura, o sistema imprime no Monitor Serial:

```
Media LDR: 512.00
Media Temperatura: 13.40
Media Umidade: 62.80
```

---

## 🧠 Decisões de Projeto

- **Média de leituras:** Reduz interferências e leituras incorretas dos sensores analógicos e digitais.
- **`millis()` sem `delay()` no loop principal:** Garante que o sistema não trave entre leituras, mantendo responsividade.
- **Hierarquia de prioridades:** Garante que o alerta mais crítico seja sempre exibido primeiro no LCD.
- **Alternância de telas (`telaAtual`):** Maximiza as informações exibidas no display de 16 colunas sem sobrecarregar o usuário.
