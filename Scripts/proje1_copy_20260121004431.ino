#include <Servo.h>
#include <DHT.h>

#define TRIG_PIN 2
#define ECHO_PIN 3
#define BUZZER_PIN 4
#define DHTPIN 5
#define DHTTYPE DHT11
#define SERVO_PIN 9
#define CURRENT_PIN A0

DHT dht(DHTPIN, DHTTYPE);
Servo brakeServo;

// --- Sabitler ve Parametreler (Konstanten) ---
const unsigned long DHT_INTERVAL = 2000;
const unsigned long SERIAL_INTERVAL = 50; 
const int DEAD_ZONE = 4;
const float ALPHA = 0.25; // EMA Filtre katsayısı
const float SENSITIVITY = 0.185; // ACS712 5A hassasiyeti
const int START_ANGLE = 0; // Güvenli başlangıç açısı [cite: 2026-01-19]

// --- Değişkenler ---
unsigned long lastDHTRead = 0;
unsigned long lastSerial = 0;
float smoothedDist = 40.0;
int lastServoAngle = START_ANGLE; 
int errorCounter = 0;

void setup() {
  Serial.begin(38400);
  
  // KRİTİK: Attach öncesi write komutu 90 derece sıçramasını engeller [cite: 2026-01-01]
  brakeServo.write(START_ANGLE); 
  brakeServo.attach(SERVO_PIN);
  
  dht.begin();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // LED Konfigürasyonu (Statusanzeige)
  for(int i=7; i<=12; i++) if(i!=9) pinMode(i, OUTPUT);
  
  // Sistem Hazır (Ready Signal)
  digitalWrite(8, HIGH); 
  delay(500); // Sensör stabilizasyonu için bekleme
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. Termal Veri Okuma (Temperaturkompensation)
  static float temp = 25.0;
  if (currentMillis - lastDHTRead >= DHT_INTERVAL) {
    float t = dht.readTemperature();
    if (!isnan(t)) temp = t;
    lastDHTRead = currentMillis;
  }

  // 2. Sensör ve Akım Verisi (Datenerfassung)
  float speedSound = (331.5 + (0.6 * temp)) / 10000.0;
  float rawDist = readDistance(speedSound);
  float current = getAmps();

  // 3. Veri Filtreleme (EMA Filter)
  if (rawDist > 0 && rawDist < 450) {
    smoothedDist = (ALPHA * rawDist) + ((1.0 - ALPHA) * smoothedDist);
    digitalWrite(7, HIGH); // Sensör OK
    errorCounter = 0;
  } else {
    // Fail-Safe Modu: 15 okuma hatasında sensör hatası yak [cite: 2025-11-21]
    if (++errorCounter > 15) digitalWrite(7, LOW);
  }

  // 4. Mekanik Kontrol (Bremsschleife)
  int targetAngle = (int)calculateAngle(smoothedDist);
  if (abs(targetAngle - lastServoAngle) >= DEAD_ZONE) {
    brakeServo.write(targetAngle);
    lastServoAngle = targetAngle;
  }
  
  updateLEDs(smoothedDist);
  handleBuzzer(smoothedDist);

  // 5. Telemetri (Python Logger için CSV formatı) [cite: 2026-01-19]
  if (currentMillis - lastSerial >= SERIAL_INTERVAL) {
    Serial.print(smoothedDist); 
    Serial.print(",");
    Serial.print(lastServoAngle);
    Serial.print(",");
    
    // Akım verisindeki gürültüyü (noise) temizle ve ölçeklendir
    float plotCurrent = (abs(current) < 0.03) ? 0.0 : current * 100.0;
    Serial.println(plotCurrent); 
    
    lastSerial = currentMillis;
  }
}

// --- Fonksiyonlar ---

float getAmps() {
  long sumADC = 0;
  for(int i=0; i<60; i++) sumADC += analogRead(CURRENT_PIN);
  float voltage = ( (sumADC / 60.0) * 5.0) / 1023.0;
  return (voltage - 2.5) / SENSITIVITY;
}

float readDistance(float ss) {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long d = pulseIn(ECHO_PIN, HIGH, 15000);
  return (d <= 0) ? -1 : (d * ss) / 2.0;
}

float calculateAngle(float d) {
  if (d <= 4.0) return 180; // Tam Fren
  if (d >= 40.0) return 0;   // Fren Açık
  float norm = (d - 4.0) / (40.0 - 4.0);
  // Sinüzoidal Frenleme Eğrisi (Sanfte Bremsung) [cite: 2026-01-01]
  return 180.0 * (1.0 - sin(norm * PI / 2.0));
}

void updateLEDs(float d) {
  digitalWrite(10, d > 30.0 ? HIGH : LOW);
  digitalWrite(11, (d <= 30.0 && d >= 15.0) ? HIGH : LOW);
  digitalWrite(12, d < 15.0 ? HIGH : LOW);
}

void handleBuzzer(float d) {
  if (d < 30.0) {
    int freq = map(d * 10, 40, 300, 3500, 800);
    int interval = map(d * 10, 40, 300, 25, 250);
    static unsigned long lb = 0;
    if (millis() - lb > (unsigned long)interval) {
      tone(BUZZER_PIN, constrain(freq, 800, 3800), 15);
      lb = millis();
    }
  } else noTone(BUZZER_PIN);
}