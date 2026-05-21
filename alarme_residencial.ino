/*
  CHECKPOINT 02 - Edge Computing

  Állex Augusto de Souza Brandão - RM571383
  Enzo Ribeiro Abreu - RM572995
  Erick Nathan Capito Pereira - RM573635
  Henrique Caruso Bueno - RM570591
  Murilo Vasconcelos Gomes - RM570941
*/

#include <LiquidCrystal.h>
#include <DHT.h>

LiquidCrystal lcd(13, 11, 9, 8, 7, 6);

int pinoLDR = A5;
int ledVerde = 2;
int ledAmarelo = 3;
int ledVermelho = 4;
int pinoBuzzer = 5;

int pinoDHT = 10;
int tipoDHT = DHT22;

DHT dht(pinoDHT, tipoDHT);

float tempMin = 10.0;
float tempMax = 15.0;

float umidMin = 50.0;
float umidMax = 70.0;

int limiteMuitoClaro = 335;
int limiteMeiaLuz = 670;

int quantidadeLeituras = 5; // Quantidade de leituras para calcular uma media de sensores

unsigned long ultimoTempo = 0;
unsigned long intervaloLeitura = 5000; 

int telaAtual = 0; // Usado para alternar as informacoes no LCD

void setup() {
  pinMode(pinoLDR, INPUT);

  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(pinoBuzzer, OUTPUT);

  Serial.begin(9600);

  lcd.begin(16, 2);
  dht.begin();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mansur Pixels");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(2000);
  lcd.clear();
}

void loop() {
  unsigned long tempoAtual = millis(); // unsigned para não pegar valores negativos

  if (tempoAtual - ultimoTempo >= intervaloLeitura) { 

    ultimoTempo = tempoAtual;

    float mediaLDR = lerMediaLDR();
    float mediaTemperatura = lerMediaTemperatura();
    float mediaUmidade = lerMediaUmidade();

    Serial.print("Media LDR: ");
    Serial.println(mediaLDR);

    Serial.print("Media Temperatura: ");
    Serial.println(mediaTemperatura);

    Serial.print("Media Umidade: ");
    Serial.println(mediaUmidade);

    atualizarSistema(mediaLDR, 
                    mediaTemperatura, 
                    mediaUmidade);
  }
}

float lerMediaLDR() {

  float soma = 0;

  for (int i = 0; i < quantidadeLeituras; i++) {
    soma += analogRead(pinoLDR);
    delay(100);
  }

  return soma / quantidadeLeituras;
}

float lerMediaTemperatura() {
  float soma = 0;
  int leiturasValidas = 0;

  for (int i = 0; i < quantidadeLeituras; i++) {
    float temperatura = dht.readTemperature();

    if (!isnan(temperatura)) {
      soma += temperatura;
      leiturasValidas++;
    }

    delay(100);
  }

  if (leiturasValidas == 0) {
    return -999;
  }

  return soma / leiturasValidas;
}

float lerMediaUmidade() {
  float soma = 0;
  int leiturasValidas = 0;

  for (int i = 0; i < quantidadeLeituras; i++) {
    float umidade = dht.readHumidity();

    if (!isnan(umidade)) {
      soma += umidade;
      leiturasValidas++;
    }

    delay(100);
  }

  if (leiturasValidas == 0) {
    return -999;
  }

  return soma / leiturasValidas;
}

void atualizarSistema(float ldr, float temperatura, float umidade) {
  apagarTudo();

  lcd.clear();

  bool temperaturaInvalida = temperatura == -999;
  bool umidadeInvalida = umidade == -999;

  if (temperaturaInvalida || umidadeInvalida) {
    ligarLed(ledVermelho);
    ligarBuzzer();

    lcd.setCursor(0, 0);
    lcd.print("Erro sensor");
    lcd.setCursor(0, 1);
    lcd.print("Verifique DHT");
    return;
  }

  bool tempBaixa = temperatura < tempMin;
  bool tempAlta = temperatura > tempMax;
  bool tempOk = temperatura >= tempMin && temperatura <= tempMax;

  bool umidBaixa = umidade < umidMin;
  bool umidAlta = umidade > umidMax;
  bool umidOk = umidade >= umidMin && umidade <= umidMax;

  bool ambienteMuitoClaro = ldr < limiteMuitoClaro;
  bool ambienteMeiaLuz = ldr >= limiteMuitoClaro && ldr < limiteMeiaLuz;
  bool ambienteEscuro = ldr >= limiteMeiaLuz;

  bool alertaTemperatura = tempAlta || tempBaixa; 
  bool alertaUmidade = umidAlta || umidBaixa; 
  bool alertaLuminosidade = ambienteMuitoClaro; 

  // Prioridade 1: Umidade fora da faixa
  if (umidAlta) {
    ligarLed(ledVermelho);
    ligarBuzzer();

    lcd.setCursor(0, 0);
    lcd.print("Umidade Alta");
    lcd.setCursor(0, 1);

    if (alertaTemperatura || alertaLuminosidade) { 
      lcd.print("Outros alertas");
    } else {
      lcd.print("U: ");
      lcd.print(umidade, 1);
      lcd.print("%");
    }

    return;
  }

  if (umidBaixa) {
    ligarLed(ledVermelho);
    ligarBuzzer();

    lcd.setCursor(0, 0);
    lcd.print("Umidade Baixa");
    lcd.setCursor(0, 1);

    if (alertaTemperatura || alertaLuminosidade) { 
      lcd.print("Outros alertas");
    } else {
      lcd.print("U: ");
      lcd.print(umidade, 1);
      lcd.print("%");
    }

    return;
  }

  // Prioridade 2: Temperatura fora da faixa
  if (tempAlta) {
    ligarLed(ledAmarelo);
    ligarBuzzer();

    lcd.setCursor(0, 0);
    lcd.print("Temp. Alta");
    lcd.setCursor(0, 1);

    if (alertaUmidade || alertaLuminosidade) { 
      lcd.print("Outros alertas");
    } else {
      lcd.print("T: ");
      lcd.print(temperatura, 1);
      lcd.print(" C");
    }

    return;
  }

  if (tempBaixa) {
    ligarLed(ledAmarelo);
    ligarBuzzer();

    lcd.setCursor(0, 0);
    lcd.print("Temp. Baixa");
    lcd.setCursor(0, 1);

    if (alertaUmidade || alertaLuminosidade) { 
      lcd.print("Outros alertas");
    } else {
      lcd.print("T: ");
      lcd.print(temperatura, 1);
      lcd.print(" C");
    }

    return;
  }

  // Prioridade 3: Ambiente muito claro
  if (ambienteMuitoClaro) {
    ligarLed(ledVermelho);
    ligarBuzzer();

    lcd.setCursor(0, 0);
    lcd.print("Amb. muito claro");
    lcd.setCursor(0, 1);
    lcd.print("LDR: ");
    lcd.print(ldr, 0);
    return;
  }

  // Prioridade 4: Ambiente a meia luz
  if (ambienteMeiaLuz) {
    ligarLed(ledAmarelo);

    lcd.setCursor(0, 0);

    if (telaAtual == 0) { 
      lcd.print("Amb. meia luz");
      lcd.setCursor(0, 1);
      lcd.print("LDR: ");
      lcd.print(ldr, 0);
    } else {
      lcd.print("T:");
      lcd.print(temperatura, 1);
      lcd.print("C OK");
      lcd.setCursor(0, 1);
      lcd.print("U:");
      lcd.print(umidade, 1);
      lcd.print("% OK");
    }

    telaAtual = (telaAtual + 1) % 2; 
    return;
  }

  // Prioridade 5: Ambiente escuro
  if (ambienteEscuro) {
    ligarLed(ledVerde);

    lcd.setCursor(0, 0);

    if (telaAtual == 0) { 
      lcd.print("Temp OK:");
      lcd.print(temperatura, 1);
      lcd.setCursor(0, 1);
      lcd.print("Umid OK:");
      lcd.print(umidade, 1);
      lcd.print("%");
    } else {
      lcd.print("Ambiente escuro");
      lcd.setCursor(0, 1);
      lcd.print("LDR: ");
      lcd.print(ldr, 0);
    }

    telaAtual = (telaAtual + 1) % 2; 
    return;
  }


  if (tempOk && umidOk) {
    ligarLed(ledVerde);

    lcd.setCursor(0, 0);
    lcd.print("Temperatura OK");
    lcd.setCursor(0, 1);
    lcd.print("Umidade OK");
  }
}

void apagarTudo() {
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledAmarelo, LOW);
  digitalWrite(ledVermelho, LOW);
  noTone(pinoBuzzer);
}

void ligarLed(int led) {
  digitalWrite(led, HIGH);
}

void ligarBuzzer() {
  tone(pinoBuzzer, 261);
}