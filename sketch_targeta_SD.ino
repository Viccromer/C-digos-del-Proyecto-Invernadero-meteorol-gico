#include <SPI.h>
#include <SD.h>

const uint8_t PIN_SD_CS = 4;
bool sdDisponible = false;
int contador =0;

const unsigned long INTERVALO_MS = 5000UL;
unsigned long ultimaPrueba = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }
  Serial.println(F("=== Prueba modulo SD ==="));

  if(!SD.begin(PIN_SD_CS)) {
    Serial.println(F("[ERROR] No se detecto la tergeta SD. Revisa cableado SPI y el pin CS."));
    sdDisponible = false;
    return;
  }
  sdDisponible = true;
  Serial.println(F("[OK] Tarjeta SD detectada."));

  //Forzada la primera prueba de escritura+lectura de inmediato
  ultimaPrueba = millis() - INTERVALO_MS;
}

void loop() {
  if (!sdDisponible) {
    return;
  }

  unsigned long ahora = millis();
  if (ahora - ultimaPrueba >= INTERVALO_MS) {
    ultimaPrueba = ahora;
    contador++;

    // --- Escribir una linea nueva ---
    File archivo = SD.open("prueba.txt", FILE_WRITE);
    if(archivo) {
      archivo.print(F("Linea de prueba #"));
      archivo.println(contador);
      archivo.close();
      Serial.print(F("[OK] Escrita linea #"));
    } else {
      Serial.println(F("[ERROR] No se pudo escribir prueba.txt."));
      return;
    }
  }
}