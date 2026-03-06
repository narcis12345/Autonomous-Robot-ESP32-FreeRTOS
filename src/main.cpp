#include <Arduino.h>

// Definim pinul pentru butonul BOOT (GPIO0)
#define BOOT_BUTTON_PIN 0

void setup() {
  Serial.begin(115200);
  Serial.println("\n--- Test Citire Buton BOOT ---");
  
  // Configurăm pinul ca intrare simplă.
  // Ne bazăm pe rezistența de pull-up externă de pe placă.
  pinMode(BOOT_BUTTON_PIN, INPUT);
}

void loop() {
  // Citim starea pinului
  int buttonState = digitalRead(BOOT_BUTTON_PIN);

  // Afișăm starea în Serial Monitor
  if (buttonState == HIGH) {
    Serial.println("Butonul NU este apasat (Stare: HIGH)");
  } else {
    Serial.println(">>>> BUTON APASAT! (Stare: LOW)");
  }

  // Așteptăm puțin pentru a nu umple consola prea repede
  delay(500);
}