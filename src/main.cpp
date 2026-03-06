#include <Arduino.h>

// Definim pinul pentru butonul BOOT (GPIO0)
#define BOOT_BUTTON_PIN 0

void setup() {
  Serial.begin(115200);
  Serial.println("\n--- Test Citire Buton BOOT ---");
  pinMode(BOOT_BUTTON_PIN, INPUT);
}

void loop() {
  int buttonState = digitalRead(BOOT_BUTTON_PIN);
  
  if (buttonState == HIGH) {
    Serial.println("Butonul NU este apasat (Stare: HIGH)");
  } else {
    Serial.println(">>>> BUTON APASAT! (Stare: LOW)");
  }

  delay(500);

}
