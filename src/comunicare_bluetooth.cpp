#include <string> // Exemplu de include C++ // Folosim std::string pentru a lucra mai ușor cu text

// Include-uri C
extern "C" {
  #include "freertos/FreeRTOS.h"  // Necesar pentru sistemul de operare (task-uri)
  #include "freertos/task.h"  // Necesar pentru managementul task-urilor
  #include "freertos/semphr.h" // Necesar pentru semafoare
  #include "driver/gpio.h"
  #include "esp_log.h"
  // Adaugă aici TOATE celelalte include-uri C (din drivere, etc.)
  #include "motoare.h"
  #include "servo.h"
  #include "utilitare.h"
  #include "detector.h"
  #include "config.h"
}

// Include-uri C++ sau header-e proprii care sunt C++-safe
#include "BluetoothSerial.h"  // Biblioteca principală pentru Bluetooth Serial
#include "comunicare_bluetooth.h" // Presupunând că e C++-safe
#include "comunicare_bluetooth.h" 


// O verificare la compilare pentru a ne asigura că Bluetooth este activat în configurarea proiectului
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `pio run -t menuconfig` to and enable it
#endif

// --- Variabile Globale pentru acest fișier (statice) ---

// Creăm obiectul principal pentru comunicarea Bluetooth.
// 'static' înseamnă că acest obiect este vizibil doar în interiorul acestui fișier.
static BluetoothSerial SerialBT;

// --- Task-ul Principal de Comunicare ---

// Acest task va rula în paralel cu logica robotului.
// Rolul său este să asculte non-stop după comenzi de la aplicația mobilă.
void task_comunicare_bluetooth(void *pvParameter) {
    // Declaram o variabilă pentru a stoca mesajul primit.
    // Folosim std::string pentru că este mai flexibil decât un char array.
    std::string mesajPrimit = "";
    
    printf("Task-ul de comunicare Bluetooth a pornit. Asteptam comenzi...\n");

    // O buclă infinită care menține task-ul "în viață"
    while (1) {
        // Verificăm dacă există date noi trimise de la telefon
        if (SerialBT.available()) {
            // Citim tot textul disponibil ca un String și îl convertim la std::string
            mesajPrimit = SerialBT.readString().c_str();

            // Verificăm dacă mesajul primit este comanda de START
            if (mesajPrimit == "START_CMD") {
                printf("Comanda START primita de la aplicatie!\n");
                // "Dăm drumul" la semaforul care deblochează task-ul principal al robotului.
                // Această linie este esențială pentru a porni mișcarea.
                xSemaphoreGive(semafor_semnal_start); 
            }

            // Verificăm dacă mesajul primit este comanda de STOP
            if (mesajPrimit == "STOP_CMD") {
                printf("Comanda STOP primita de la aplicatie!\n");
                // Aici se poate implementa o logică de oprire de urgență.
                // De exemplu, setarea unui flag global sau oprirea directă a motoarelor.
                // Momentan, doar trimitem o confirmare înapoi.
                flag_oprire_urgenta = true; 
                trimite_mesaj_bluetooth("OPRIRE DE URGENTA!");
            }
        }
        
        // Punem o mică pauză în task pentru a permite altor task-uri să ruleze.
        // Fără această pauză, task-ul ar consuma 100% din procesor.
        vTaskDelay(pdMS_TO_TICKS(50)); // Verificăm de 20 de ori pe secundă
    }
}


// --- Implementarea Funcțiilor Publice (declarate în .h) ---

// Funcția pe care o vom apela din alte părți ale codului pentru a trimite mesaje.
void trimite_mesaj_bluetooth(const char* mesaj) {
    // Verificăm dacă un client (telefonul) este conectat înainte de a trimite.
    if (SerialBT.hasClient()) {
        SerialBT.println(mesaj);
    }
    // Afișăm mesajul și în consola serială, pentru debug.
    printf("[BT_TRIMIS] %s\n", mesaj);
}

// Funcția principală de inițializare a serviciului Bluetooth.
void init_bluetooth() {
    // Pornim Bluetooth-ul și îi dăm un nume.
    // Acesta este numele care va apărea în lista de dispozitive a telefonului.
    SerialBT.begin("Vehicul_Autonom_NARCIS");
    printf("Serviciul Bluetooth a pornit. Nume vizibil: Vehicul_Autonom_NARCIS\n");
    
    // Creăm și pornim task-ul de comunicare pe care l-am definit mai sus.
    // Parametrii: funcția task, nume task (pt debug), dimensiune stack, parametri, prioritate, handle.
    xTaskCreate(task_comunicare_bluetooth, "task_bt_comm", 4096, NULL, 5, NULL);
}