#include <DHT.h>
#define PIN_DHT 2

// const uint8_t PIN_DHT = 2;
// #define TIPO_DHT_ DHT22
#define TIPO_DHT DHT11
DHT dht(PIN_DHT, TIPO_DHT);


const unsigned long INTERVALO_MS = 2000UL;
unsigned long ultimaLectura = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }
  Serial.println(F("=== Prueba DHT22 ==="));
  Serial.println(F("Si ves NaN de forma constante, revisa el cableado (dato/VVC/GND)"));
  Serial.println(F("y que el pin de datos sea el 2, o cambia PIN_DHT arrib si usaste otro"));
  dht.begin();
}

void loop() {
  unsigned long ahora = millis();
  if (ahora - ultimaLectura >= INTERVALO_MS) {
    ultimaLectura = ahora;

    float temperaturaC = dht.readTemperature();
    float humedadPct = dht.readHumidity();

    if (isnan(temperaturaC) || isnan(humedadPct)) {
      Serial.println(F("[ERROR] Lectura invalida (NaN). Revisa cableado o espera al siguiente ciclo."));
      return;
    }

    Serial.print(F("Temperatura: "));
    Serial.print(temperaturaC, 1);
    Serial.print(F(" C Humedad: "));
    Serial.print(humedadPct, 1);
    Serial.println(F(" %"));
  }
}
