#include <Wire.h> // SDA = A4
#include <RTClib.h> // SCL = A5

RTC_DS3231 rtc;
bool rtcDisponible = false;

const unsigned long INTERVALO_MS = 100UL;
unsigned long ultimaImpresion = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }
  Serial.println(F("=== Prueba RTC DS3231 ==="));

  if (!rtc.begin()) {
    Serial.println(F("[ERROR] No se detecto el modulo RTC. Revisa cableado I2C (SDA/SCL)."));
    rtcDisponible = false;
    return;
  }
  rtcDisponible = true;

  if (rtc.lostPower()) {
    Serial.println(F("[AVISO] El RTC perdio la hora (pila de respaldo agotada/ausente, o primera vez)"));
    Serial.println(F("        Ajustando el RCT a la hora de copilacion del sketch..."));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  } else {
    Serial.println(F("[OK] El RTC ya tenia la hora guardada (la pila la sostuvo)."));
  }
}

void loop() {
  if (!rtcDisponible) {
    return; //nada que mostrar si el modulo no respondio en setup()
  }

  unsigned long ahora = millis();
  if (ahora - ultimaImpresion >= INTERVALO_MS) {
    ultimaImpresion = ahora;
    DateTime n = rtc.now();

    char buf[20];
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
            n.year(), n.month(), n.day(), n.hour(), n.minute(), n.second());
    Serial.println(buf);
  }
}
