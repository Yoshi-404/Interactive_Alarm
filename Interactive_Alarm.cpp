#include <Adafruit_LiquidCrystal.h>
// O 0 indica o endereço I2C padrão do Tinkercad
Adafruit_LiquidCrystal lcd(0);

// --- Definição de Pinos ---
const int pinBuzzer = 8;
const int pinPIR = 7;
const int pinLedR = 9;
const int pinLedG = 10;
const int pinLedB = 11;
const int pinSensorFC = A0; // Potenciômetro simulando o sensor

// Pinos dos Botões do Menu
const int pinBotaoSelect = 1; // D1 (Atenção: Pino TX da Serial)
const int pinBotaoUp = 2;     // D2
const int pinBotaoDown = 3;   // D3

// --- Definição dos Estados ---
enum Estado { 
  ESPERA, 
  CONFIGURACAO, 
  ALARME_TOCANDO, 
  DESAFIO_BIOMETRICO, 
  DESLIGADO 
};

void mudarEstado(Estado novoEstado);

Estado estadoAtual = ESPERA;

// --- Variáveis de Controle ---
unsigned long tempoInicioDesafio = 0;
const unsigned long TIMEOUT_DESAFIO = 10000; 
int bpmAtual = 0;

// Variáveis de Configuração do Usuário
int limiarBPM = 80; 

void setup() {
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinPIR, INPUT);
  pinMode(pinLedR, OUTPUT);
  pinMode(pinLedG, OUTPUT);
  pinMode(pinLedB, OUTPUT);
  
  // Configura os 3 botões com resistores internos
  pinMode(pinBotaoSelect, INPUT_PULLUP);
  pinMode(pinBotaoUp, INPUT_PULLUP);
  pinMode(pinBotaoDown, INPUT_PULLUP);

  // Inicializa o LCD I2C
lcd.begin(16, 2);
  lcd.setBacklight(1);
  
  // A Serial.begin(9600) foi removida para não interferir com o botão no pino D1 (TX).
  
  mudarEstado(ESPERA);
}

void loop() {
  switch (estadoAtual) {
    
    case ESPERA:
      // Pressionar SELECT entra no menu de configuração
      if (digitalRead(pinBotaoSelect) == LOW) { 
        delay(200); // Debounce
        mudarEstado(CONFIGURACAO);
      }
      // Pressionar UP simula que o horário do alarme chegou
      if (digitalRead(pinBotaoUp) == LOW) {
        delay(200);
        mudarEstado(ALARME_TOCANDO);
      }
      break;

    case CONFIGURACAO:
      // Botão UP: Aumenta a exigência do BPM
      if (digitalRead(pinBotaoUp) == LOW) {
        limiarBPM++;
        if (limiarBPM > 120) limiarBPM = 120; // Limite máximo
        atualizarTelaMenu();
        delay(200);
      }
      // Botão DOWN: Diminui a exigência do BPM
      if (digitalRead(pinBotaoDown) == LOW) {
        limiarBPM--;
        if (limiarBPM < 50) limiarBPM = 50; // Limite mínimo
        atualizarTelaMenu();
        delay(200);
      }
      // Botão SELECT: Salva e sai do menu
      if (digitalRead(pinBotaoSelect) == LOW) {
        delay(200);
        lcd.clear();
        lcd.print("Salvo!");
        delay(1000);
        mudarEstado(ESPERA);
      }
      break;

    case ALARME_TOCANDO:
      tocarBuzzerIntenso();
      if (digitalRead(pinPIR) == HIGH) {
        noTone(pinBuzzer); 
        mudarEstado(DESAFIO_BIOMETRICO);
      }
      break;

    case DESAFIO_BIOMETRICO:
      if (millis() - tempoInicioDesafio > TIMEOUT_DESAFIO) {
        mudarEstado(ALARME_TOCANDO); 
        break;
      }
      lerFrequenciaCardiaca();
      
      // O botão Select é usado para confirmar o desafio aqui
      if (digitalRead(pinBotaoSelect) == LOW) {
        delay(200); 
        if (bpmAtual >= limiarBPM) { // Compara com a variável configurada!
          mudarEstado(DESLIGADO);
        } else {
          mudarEstado(ESPERA); 
        }
      }
      break;

    case DESLIGADO:
      // Fica desligado até ser reiniciado
      break;
  }
}

// --- Funções Auxiliares ---

void mudarEstado(Estado novoEstado) {
  estadoAtual = novoEstado;
  lcd.clear();
  lcd.setCursor(0, 0);
  
  switch (novoEstado) {
    case ESPERA:
      lcd.print("Aguardando...");
      corLed(0, 0, 0); 
      break;
      
    case CONFIGURACAO:
      corLed(0, 0, 255); // LED Azul para modo configuração
      atualizarTelaMenu();
      break;

    case ALARME_TOCANDO:
      lcd.print("ACORDE!");
      corLed(255, 0, 0); 
      break;
      
    case DESAFIO_BIOMETRICO:
      lcd.print("Medindo FC...");
      tempoInicioDesafio = millis(); 
      corLed(255, 100, 0); 
      break;
      
    case DESLIGADO:
      lcd.print("Bom dia!");
      corLed(0, 255, 0); 
      tocarJingleConfirmacao();
      break;
  }
}

void atualizarTelaMenu() {
  lcd.setCursor(0, 0);
  lcd.print("Ajustar Limiar:");
  lcd.setCursor(0, 1);
  lcd.print("BPM Minimo: ");
  lcd.print(limiarBPM);
  lcd.print("  "); // Limpa caracteres fantasmas
}

void tocarBuzzerIntenso() {
  if ((millis() / 200) % 2 == 0) {
    tone(pinBuzzer, 2000);
  } else {
    noTone(pinBuzzer);
  }
}

void lerFrequenciaCardiaca() {
  bpmAtual = map(analogRead(pinSensorFC), 0, 1023, 40, 120);
  lcd.setCursor(0, 1);
  lcd.print("BPM: ");
  lcd.print(bpmAtual);
  lcd.print("   "); 
}

void tocarJingleConfirmacao() {
  tone(pinBuzzer, 523, 150); delay(150);
  tone(pinBuzzer, 659, 150); delay(150);
  tone(pinBuzzer, 784, 300); delay(300);
  noTone(pinBuzzer);
}

void corLed(int r, int g, int b) {
  analogWrite(pinLedR, r);
  analogWrite(pinLedG, g);
  analogWrite(pinLedB, b);
}