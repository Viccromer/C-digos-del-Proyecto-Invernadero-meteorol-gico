#include <DHT.h>
#define DHT_PIN 2
#define DHTTYPE DHT11

#define LEDA 12
#define LEDV 11
#define LEDR 10

#define TEMP_FRIO 25.3
#define TEMP_TEMPLADO 25.5
#define TEMP_CALOR 25.8

#define PAUSA 1000

DHT dht(DHT_PIN, DHT11); // Modelo del sensor

float temp = 0;
float hum = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode (LEDA, OUTPUT);
  pinMode (LEDV, OUTPUT);
  pinMode (LEDR, OUTPUT);
}

void loop() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(hum) || isnan(temp)) {
    Serial.println("Error en el sensor DHT11");
  } else { 
 Serial.print("Temp: ");
 Serial.println(temp);
 Serial.print(" °C\tHumedad: ");
 Serial.println(hum);
 Serial.print(" %");
 delay(PAUSA);
 } 
  
  digitalWrite (LEDA, LOW);
  digitalWrite (LEDV, LOW);
  digitalWrite (LEDR, LOW);

  if (temp < TEMP_FRIO) { //FRIO = 18
    digitalWrite(LEDA, HIGH);
    Serial.print ("FRIO");
  }

  if (temp >= TEMP_FRIO && temp <= TEMP_TEMPLADO) { //TEMPLADO = 18 a 30
    digitalWrite(LEDV, HIGH);
    Serial.print("TEMPLADO");
  }

  if (temp > TEMP_CALOR) { //CALOR = MAYOR A 30
    digitalWrite(LEDR, HIGH);
    Serial.print("CALOR");
  }
}