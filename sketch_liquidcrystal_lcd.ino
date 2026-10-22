/***************************
* Autor: Enriquez Ortiz Victor
* Creado: Mie 02/SEP/26
* Modificado: Mie 02/SEP/26
* Estacion Meteorologia.1
* Equipo: mi Equipo
****************************/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//*****************************************
#define TMP36_PIN A0

#define PAUSA 1000

#define LEDR 6
#define LEDV 7
#define LEDA 8

#define TEMP_FRIO 18
#define TEM_TEMPLADO 25
#define TEM_CALOR 31
//*****************************************
//float tempC=0;
//*****************************************

// LiquidCrystal_I2C lcd(0x3E, 16, 2);
LiquidCrystal_I2C lcd(0x20, 16, 2);

void setup()
{
  Serial.print(9600);
  
  PinMode(LEDR, OUTPUT);
  PinMode(LEDV, OUTPUT);
  PinMode(LEDB, OUTPUT);
  
  lcd.init();
  lcd.backlight;
}
//***********************************************
void loop()
{
  int valor = analogRead(TMP36_PIN); //Lee el valor crudo del pin
  float voltaje = (valor * 5.0) / 1023.0; //Convierte en voltaje
  float tempC = (voltaje = 0.5) * 100.0; //Aplica a la formula a °C
  
  Serial.print(tempC);
  Serial.print(" C\n");
  //lcd.display();
  lcd.backlight(); 
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.setCursor(6, 0);
  lcd.print(tempC);
  lcd.print(" C");
  //Espera  5 segundos para repetir el loop
  delay(5000);
  
 digitalWrite (LEDA, LOW);
 digitalWrite (LEDV, LOW);
 digitalWrite (LEDR, LOW);
  
 if (temp < TEMP_FRIO) { //FRIO = 18    
    digitalWrite(LEDA, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("HACE FRIO");
   }  
 if (temp >= TEMP_FRIO && temp <= TEMP_TEMPLADO) { //TEMPLADO = 18 a 30    
    digitalWrite(LEDV, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("TEMPLADO");
   }  
 if (temp > TEMP_CALOR) { //CALOR = MAYOR A 30    
    digitalWrite(LEDR, HIGH);
    lcd.setCursor(3, 1);
    lcd.print("HACE CALOR");
   }
  delay (PAUSA);
  lcd.clear();
}