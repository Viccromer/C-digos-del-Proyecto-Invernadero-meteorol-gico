#include <Wire.h>
#include <RTClib.h>
#include <DHT.h>
#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal_I2C.h>

// Configuración de la Pantalla LCD I2C (Dirección 0x27 y tamaño 16x2)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Configuración de Pines
#define PIN_DHT 3            // Digital: Sensor DHT11
#define PIN_BUZZER 4         // Digital: Buzzer Pasivo (KY-006)
const uint8_t PIN_SD_CS = 10;// Pin CS del Módulo Lector MicroSD (SPI: 10, 11, 12, 13)
#define PIN_LDR A0           // Analógico: Módulo LDR (Pin AO)
#define PIN_SUELO A1         // Analógico: Sensor Humedad de Suelo (FC-28)

#define TIPO_DHT DHT11

DHT dht(PIN_DHT, TIPO_DHT);
RTC_DS3231 rtc;

bool rtcDisponible = false;
bool sdDisponible = false;

// Configuración del archivo de almacenamiento
const char* NOMBRE_ARCHIVO = "BITACORA.CSV"; 

// Intervalos de tiempo independientes (No bloqueantes)
const unsigned long INTERVALO_SD = 600000UL;   // Guardar en la MicroSD cada 1 minuto
const unsigned long INTERVALO_LCD = 1000UL;   // Actualizar LCD y Serial cada 1 segundo

unsigned long ultimaLecturaSD = 0;
unsigned long ultimaLecturaLCD = 0;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BUZZER, OUTPUT);

  // Inicializar Pantalla LCD
  Wire.begin();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(" Invernadero OK ");
  delay(1500);
  lcd.clear();

  // Inicializar el Reloj en Tiempo Real (RTC)
  if (rtc.begin()) {
    rtcDisponible = true;
    
    // === ¡PASO DE ACTUALIZACIÓN FORZADA DE HORA! ===
    // Al compilar, esta línea toma la hora exacta de tu PC (las 12:50 o la actual) y la graba en el chip.
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    
  } else {
    Serial.println(F("Error: Módulo RTC DS3231 no detectado."));
  }

  // Inicializar el sensor de Temperatura y Humedad
  dht.begin();

  // Inicializar Tarjeta SD
  if (SD.begin(PIN_SD_CS)) {
    sdDisponible = true;
    Serial.println(F("SD Lista. Guardando cada 10 minutos en BITACORA.CSV"));
  } else {
    Serial.println(F("Error: Tarjeta SD no detectada."));
  }

  // Pitido inicial de arranque exitoso
  tone(PIN_BUZZER, 1000, 100);
  lcd.setCursor(0, 0);
  lcd.print("Sistema Listo!  ");
  delay(1000);
  lcd.clear();
}

void loop() {
  unsigned long ahora = millis();

  // 1. OBTENCIÓN DE DATOS EN TIEMPO REAL
  char tiempoStr[20] = "0000-00-00 00:00:00";
  char horaCortaStr[9] = "00:00:00"; 
  
  if (rtcDisponible) {
    DateTime ahoraRTC = rtc.now();
    // Formato largo para SD y Serial
    snprintf(tiempoStr, sizeof(tiempoStr), "%04d-%02d-%02d %02d:%02d:%02d",
             ahoraRTC.year(), ahoraRTC.month(), ahoraRTC.day(),
             ahoraRTC.hour(), ahoraRTC.minute(), ahoraRTC.second());
             
    // Formato corto para el espacio limitado de la LCD (8 caracteres)
    snprintf(horaCortaStr, sizeof(horaCortaStr), "%02d:%02d:%02d",
             ahoraRTC.hour(), ahoraRTC.minute(), ahoraRTC.second());
  }

  // Lectura continua de los sensores
  float temperaturaC = dht.readTemperature();
  float humedadAirePct = dht.readHumidity();
  int valorLuz = analogRead(PIN_LDR);
  int humedadSueloPct = constrain(map(analogRead(PIN_SUELO), 1023, 350, 0, 100), 0, 100);

  // Clasificación del estado según la temperatura
  String estadoCilantro = "";
  if (isnan(temperaturaC) || isnan(humedadAirePct)) {
    estadoCilantro = "Error DHT";
  } else if (temperaturaC < 15.0) {
    estadoCilantro = "Frio";
  } else if (temperaturaC >= 15.0 && temperaturaC <= 24.0) {
    estadoCilantro = "OK";
  } else {
    estadoCilantro = "Calor";
  }


  // === TAREA 1: REFRESCAR PANTALLA LCD Y MONITOR SERIAL (Cada 1 Segundo) ===
  if (ahora - ultimaLecturaLCD >= INTERVALO_LCD) {
    ultimaLecturaLCD = ahora;

    // Impresión en Monitor Serial
    Serial.print(F("[")); Serial.print(tiempoStr); Serial.print(F("] "));
    Serial.print(F("T: ")); Serial.print(temperaturaC, 1);
    Serial.print(F("C | H.Aire: ")); Serial.print(humedadAirePct, 1);
    Serial.print(F("% | Suelo: ")); Serial.print(humedadSueloPct);
    Serial.print(F("% | Estado: ")); Serial.println(estadoCilantro);

    // LCD - Línea 1: Temperatura y Humedad
    lcd.setCursor(0, 0);
    if (isnan(temperaturaC)) {
      lcd.print("ERR SENSOR DHT11");
    } else {
      lcd.print("T:");
      lcd.print(temperaturaC, 1);
      lcd.print((char)223); // Carácter de grados (°)
      lcd.print("C H:");
      lcd.print(humedadAirePct, 0);
      lcd.print("%   ");
    }

    // LCD - Línea 2: Estado a la izquierda (8 espacios) y Hora Real a la derecha (8 espacios)
    lcd.setCursor(0, 1);
    lcd.print("        "); // Limpia fantasmas visuales previos
    lcd.setCursor(0, 1);
    lcd.print(estadoCilantro);
    
    lcd.setCursor(8, 1); // Posiciona el cursor a la mitad exacta
    lcd.print(horaCortaStr); // Imprime la hora corregida segundo a segundo
  }


  // === TAREA 2: GUARDADO EN TARJETA SD Y ALERTA DE BUZZER (Cada 1 Minuto) ===
  if (ahora - ultimaLecturaSD >= INTERVALO_SD) {
    ultimaLecturaSD = ahora;

    if (sdDisponible && !isnan(temperaturaC)) {
      File archivo = SD.open(NOMBRE_ARCHIVO, FILE_WRITE);
      if (archivo) {
        // Escritura estructurada en formato CSV
        archivo.print(tiempoStr); archivo.print(F(","));
        archivo.print(temperaturaC, 1); archivo.print(F(","));
        archivo.print(humedadAirePct, 1); archivo.print(F(","));
        archivo.print(valorLuz); archivo.print(F(","));
        archivo.print(humedadSueloPct); archivo.print(F(","));
        archivo.println(estadoCilantro);
        archivo.close(); 
        
        // Pitido limpio y corto del Buzzer (No bloqueante)
        tone(PIN_BUZZER, 2000, 150); 
        Serial.println(F(">>> Datos guardados con éxito en la MicroSD + Buzzer Activado! <<<"));
      } else {
        Serial.println(F("Error crítico: No se pudo abrir el archivo CSV."));
      }
    }
  }
}
