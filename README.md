*[Read this in English](README_EN.md)*

# Despertador Biometrico Inteligente

Este projeto implementa o prototipo de um despertador inteligente utilizando a plataforma Arduino. O sistema exige que o usuario prove que esta acordado atraves de um "desafio biometrico", simulado pela leitura de batimentos cardiacos, alem de detectar presenca fisica antes de desativar o alarme.

## Arquitetura e Logica de Controle

O software foi estruturado utilizando uma Maquina de Estados Finitos (FSM) controlada pela variavel `enum Estado`, garantindo que o microcontrolador execute apenas as rotinas pertinentes a cada fase do processo:

* **ESPERA (Standby):** Estado inicial. O sistema aguarda a configuracao ou o acionamento do alarme.
* **CONFIGURACAO:** Acessado pelo botao Select. Permite ajustar o limiar minimo de Batimentos Por Minuto (BPM) exigido para desligar o alarme (entre 50 e 120 BPM). O LED RGB acende em Azul.
* **ALARME TOCANDO:** Dispara o aviso sonoro intermitente e acende o LED em Vermelho. O sistema so avanca se o sensor de movimento (PIR) detectar presenca.
* **DESAFIO BIOMETRICO:** O usuario tem um tempo limite de 10 segundos para realizar a medicao da frequencia cardiaca (simulada via potenciometro). Se o valor lido for igual ou superior ao limiar configurado e o botao Select for pressionado, o alarme e desativado. Caso contrario, o sistema retorna ao estado de alerta. O LED acende em Laranja.
* **DESLIGADO:** O alarme e validado e encerrado com sucesso. O LED acende em Verde e uma melodia de confirmacao e reproduzida.

## Pinagem e Conexoes

| Componente | Pino Arduino | Configuracao | Descricao |
| :--- | :---: | :---: | :--- |
| **Botao Select** | `Pin 1 (TX)` | INPUT_PULLUP | Confirma acoes e acessa o menu (*Nota: O uso do pino TX exige que a comunicacao Serial esteja desativada*) |
| **Botao Up** | `Pin 2` | INPUT_PULLUP | Incrementa variaveis e simula o disparo do alarme |
| **Botao Down** | `Pin 3` | INPUT_PULLUP | Decrementa o limiar de BPM configurado |
| **Sensor PIR** | `Pin 7` | INPUT | Detecta a movimentacao do usuario |
| **Buzzer** | `Pin 8` | OUTPUT | Emissor de bipes e melodias |
| **LED RGB (Vermelho)** | `Pin 9` | OUTPUT | Controle do canal vermelho via PWM |
| **LED RGB (Verde)** | `Pin 10` | OUTPUT | Controle do canal verde via PWM |
| **LED RGB (Azul)** | `Pin 11` | OUTPUT | Controle do canal azul via PWM |
| **Sensor FC (Potenciometro)**| `A0` | ANALOG IN | Simula a leitura analogica de um sensor de batimentos cardiacos |
| **LCD I2C (SDA/SCL)** | `A4 / A5` | I2C | Comunicacao com o display LCD 16x2 |

## Esquema do Circuito

Abaixo esta a disposicao dos componentes e cabeamento projetados na plataforma Tinkercad:

![Circuito Despertador Biometrico](Interactive_Alarm.png)
