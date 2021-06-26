// PROYECTO DESARROLLADO POR SEAMAN SRL
// EL PRESENTE CODIGO SE ENTREGA SIN GARANTIA ALGUNA
// A REVISAR: SI BIEN LOS CALCULOS SE REALIZAN USANDO LA TABLA DE ESPECIFICACION DEL MQ135 ESTOS NO FUERON VERIFICADOS CON RIGUROSIDAD
// Y PODRIAN REQUERIR AJUSTES ADICIONALES EN LAS LIBRERIAS MQ135.H

#include "DHT.h"
#include "MQ135.h"
#include <LiquidCrystal_I2C.h>

// DECLARO PANTALLA DE DOS LINEAS
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// DEFINO TIPO DE SENSOR DE TEMPERATURA Y HUMEDAD
#define DHTTYPE DHT11
// #define DHTTYPE DHT22 // ES UN SENSOR MUCHO MEJOR

// DEFINO EL PIN DONDE SE CONECTARA EL SENSOR MQ135
#define MQ135_PIN 0

// DEFINO EL PIN DONDE SE CONECTARA EL SENSOR DHT11
#define DHT_PIN 5

// DEFINO EL PIN DONDE SE CONECTARA EL BEEPER ACTIVO
#define BEEPER_PIN 3

// DEFINO UN OFFSET PARA EL SENSOR DHT11, O SEA, HAY QUE MODIFICARLO PARA AJUSTAR LO MEDIDO EN EL PRESENTE PROYECTO CON LO MEDIDO EN UN RERMOMETRO PATRON
#define DHT_OFFSET -4

// PARAMETRIZO SENSOR DHT
DHT dht(DHT_PIN, DHTTYPE);

// DECLARO EL MQ-135
MQ135 MQ135_SENSOR = MQ135(MQ135_PIN);

// DECLARO LAS VARIABLES DE QUE SE USARAN PARA MEDIR 
float humedad = 0;
float temperatura = 0;
float CO2_PPM = 0; 

// VARIABLES QUE MIDE EL TIEMPO ENTRE CADA BEEP SEGUN NIVEL DE CONCENTRACIÓN DE CO2
unsigned long startMillis;
unsigned long currentMillis;
unsigned long elapsedMillis;

// VARIABLES QUE MIDE EL TIEMPO ENTRE CADA BEEP SEGUN NIVEL DE CONCENTRACIÓN DE CO2
unsigned long ReadStartMillis;
unsigned long ReadCurrentMillis;
unsigned long ReadElapsedMillis;

// SIMBOLO DE GRADOS

byte CaraMuyFeliz[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B11111,
  B10001,
  B01110,
  B00000
};

byte CaraFeliz[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B10001,
  B01110,
  B00000,
  B00000
};

byte CaraNeutral[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B00000,
  B11111,
  B00000,
  B00000
};

byte CaraTriste[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B01110,
  B10001,
  B00000,
  B00000
};

byte CaraMuyTriste[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B00000,
  B10101,
  B01010,
  B00000
};

byte CaraGritando[] = {
  B00000,
  B01010,
  B00000,
  B00100,
  B01010,
  B10001,
  B01010,
  B00100
};

byte Gota[] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B11111,
  B11111,
  B01110,
  B00000
};

byte Termometro[] = {
  B00100,
  B01010,
  B01010,
  B01010,
  B01110,
  B11111,
  B11111,
  B01110
};

byte Aire[] = {
  B00000,
  B00000,
  B01000,
  B10101,
  B00010,
  B01000,
  B10101,
  B00010
};

// FUNCION PARA HACER SONAR EL BEEPER ACTIVO
void beep()
{
    digitalWrite(BEEPER_PIN, LOW);
    delay(100); 
    digitalWrite(BEEPER_PIN, HIGH);
    delay(100); 
    digitalWrite(BEEPER_PIN, LOW);
    delay(100); 
    digitalWrite(BEEPER_PIN, HIGH);  
}

// INICIALIZO EL SCRIPT
void setup() {
  Serial.begin(57600);
  
  lcd.begin(16, 2);
  lcd.backlight();

  dht.begin();
  
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);

  // CREO CARACTERES ESPECIALES
  lcd.createChar(1, CaraMuyFeliz);
  lcd.createChar(2, CaraFeliz);
  lcd.createChar(3, CaraNeutral);
  lcd.createChar(4, CaraTriste);
  lcd.createChar(5, CaraMuyTriste);
  lcd.createChar(6, CaraGritando);
  lcd.createChar(7, Gota);
  lcd.createChar(8, Termometro);
  lcd.createChar(9, Aire);
  
  // EL SENSOR DEBE CALENTAR PARA REALIZAR MEDICIONES CORRECTAR POR LO CUAL LE DOY 30 SEGUNDOS DE TIEMPO PARA DICHA TAREA
  lcd.home();
  lcd.print("Iniciando...");
  
  for (int porcentual = 0; porcentual <= 100; porcentual++)
  {
    lcd.setCursor(0, 1);
    lcd.print(String(porcentual) + "%");
    delay(300);
  }
  
  lcd.clear();
}

void loop() {
  // LOS CICLOS DE REFRESCO SON CADA 1 SEGUNDOS
  delay(10);

  ReadCurrentMillis = millis();
  ReadElapsedMillis = ReadCurrentMillis - ReadStartMillis;

  // LEO LOS SENSORES CADA 5 SEGUNDOS
  
  if (ReadElapsedMillis >= 5000)
  {
    ReadStartMillis = millis();

    if (DHTTYPE == DHT11)
    {
      // EL DHT11 ES UNA BASURA ASI QUE HACEMOS ALGUNAS MAGIAS COMO PARA QUE LA HUMEDAD ESTE MAS CERCA DE LA REALIDAD
      humedad = dht.readHumidity();
      humedad = (100/ 40 * humedad) - 25;

      if (humedad > 100)
        humedad = 100;
    }
    else
    {
      humedad = dht.readHumidity();
    }

    humedad = dht.readHumidity();
    temperatura = dht.readTemperature() + DHT_OFFSET;

    // IMPRIMO LA RZERO DE CALIBRACION
    Serial.print("RZero: ");
    Serial.println(MQ135_SENSOR.getRZero());
    
    // DEPENDE DEL SENSOR LA ESCALA PUEDE QUE SE REQUIERA AJUSTAR ASI QUE.... BUENO, ESO...
    // MULTIPLICO DE A 10 HASTA QUE LA MEDICION DE MAYOR A 100 YA QUE NUNCA HAY MENOS DE 100 PPM
    // EN LA ATMOSFERA... MAL POR LOS HUMANOS PERO ES ASI.
    
    for (int Escala=1; Escala <=10000; Escala = Escala * 10)
    {
        CO2_PPM = MQ135_SENSOR.getCorrectedPPM(temperatura, humedad) * Escala; 

        if (CO2_PPM > 100)
          break;
    }
    
    char str[3];
   
    // PRESENTO LA INFORMACION EN PANTALLA
    // ICONO DE AIRE
    lcd.setCursor(0, 0);
    lcd.write(9);
  
    lcd.setCursor(1, 0);
    lcd.print("              ");
    lcd.setCursor(1, 0);
    lcd.print(String(CO2_PPM) + " ppm");
  
    // ICONO DE GOTA
    lcd.setCursor(0, 1);
    lcd.write(7);
    
    lcd.setCursor(1, 1);
    lcd.print(String(dtostrf(humedad, 2, 1, str)) + "%");
  
    // ICONO TERMOMETRO
    lcd.setCursor(7, 1);
    lcd.write(8);
  
    lcd.setCursor(8, 1);
    lcd.print(String(dtostrf(temperatura, 2, 1, str)) +  "\337c");
  }

  // HAGO SONAR UNA ALERTA SI LA CONSENTRACION DE CO2 SUPERRA LOS 800, AUMENTANDO LA FRECUENCIA DE PITIDOS SEGUN CONCENTRACIÓN
  currentMillis = millis();
  elapsedMillis = (currentMillis - startMillis);

  if (CO2_PPM > 5000)
  {
    if (elapsedMillis > 1000)
    {
      beep();
      startMillis = millis();
    }

    lcd.setCursor(15, 0);
    lcd.write(6);

    return;
  }
  
  if (CO2_PPM > 3000 && elapsedMillis > 2000)
  {
    if (elapsedMillis > 2000)
    {
      beep();
      startMillis = millis();
    }

    lcd.setCursor(15, 0);
    lcd.write(6);

    return;
  }
  
  if (CO2_PPM > 2500)
  {
    if (elapsedMillis > 5000)
    {
      beep();
      startMillis = millis();
    }

    lcd.setCursor(15, 0);
    lcd.write(6);

    return;
  }
  
  if (CO2_PPM > 2000)
  {
    if (elapsedMillis > 10000)
    {
      beep();
      startMillis = millis();
    }

    lcd.setCursor(15, 0);
    lcd.write(5);

    return;
  }
  
  if (CO2_PPM > 1500)
  {
    if (elapsedMillis > 15000)
    {
      beep();
      startMillis = millis();
    }

    lcd.setCursor(15, 0);
    lcd.write(4);

    return;
  }
  
  if (CO2_PPM > 1000)
  {
    if (elapsedMillis > 30000)
// PROYECTO DESARROLLADO POR SEAMAN SRL
// EL PRESENTE CODIGO SE ENTREGA SIN GARANTIA ALGUNA
// A REVISAR: SI BIEN LOS CALCULOS SE REALIZAN USANDO LA TABLA DE ESPECIFICACION DEL MQ135 ESTOS NO FUERON VERIFICADOS CON RIGUROSIDAD
// Y PODRIAN REQUERIR AJUSTES ADICIONALES EN LAS LIBRERIAS MQ135.H

#include "DHT.h"
#include "MQ135.h"
#include <LiquidCrystal_I2C.h>

// DECLARO PANTALLA DE DOS LINEAS
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// DEFINO TIPO DE SENSOR DE TEMPERATURA Y HUMEDAD
#define DHTTYPE DHT11
// #define DHTTYPE DHT22 // ES UN SENSOR MUCHO MEJOR

// DEFINO EL PIN DONDE SE CONECTARA EL SENSOR MQ135
#define MQ135_PIN 0

// DEFINO EL PIN DONDE SE CONECTARA EL SENSOR DHT11
#define DHT_PIN 5

// DEFINO EL PIN DONDE SE CONECTARA EL BEEPER ACTIVO
#define BEEPER_PIN 3

// DEFINO UN OFFSET PARA EL SENSOR DHT11, O SEA, HAY QUE MODIFICARLO PARA AJUSTAR LO MEDIDO EN EL PRESENTE PROYECTO CON LO MEDIDO EN UN RERMOMETRO PATRON
#define DHT_OFFSET -4

// PARAMETRIZO SENSOR DHT
DHT dht(DHT_PIN, DHTTYPE);

// DECLARO EL MQ-135
MQ135 MQ135_SENSOR = MQ135(MQ135_PIN);

// DECLARO LAS VARIABLES DE QUE SE USARAN PARA MEDIR 
float humedad = 0;
float temperatura = 0;
float CO2_PPM = 0; 

// VARIABLES QUE MIDE EL TIEMPO ENTRE CADA BEEP SEGUN NIVEL DE CONCENTRACIÓN DE CO2
unsigned long startMillis;
unsigned long currentMillis;
unsigned long elapsedMillis;

// VARIABLES QUE MIDE EL TIEMPO ENTRE CADA BEEP SEGUN NIVEL DE CONCENTRACIÓN DE CO2
unsigned long ReadStartMillis;
unsigned long ReadCurrentMillis;
unsigned long ReadElapsedMillis;

// SIMBOLO DE GRADOS

byte CaraMuyFeliz[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B11111,
  B10001,
  B01110,
  B00000
};

byte CaraFeliz[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B10001,
  B01110,
  B00000,
  B00000
};

byte CaraNeutral[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B00000,
  B11111,
  B00000,
  B00000
};

byte CaraTriste[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B01110,
  B10001,
  B00000,
  B00000
};

byte CaraMuyTriste[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B00000,
  B10101,
  B01010,
  B00000
};

byte CaraGritando[] = {
  B00000,
  B01010,
  B00000,
  B00100,
  B01010,
  B10001,
  B01010,
  B00100
};

byte Gota[] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B11111,
  B11111,
  B01110,
  B00000
};

byte Termometro[] = {
  B00100,
  B01010,
  B01010,
  B01010,
  B01110,
  B11111,
  B11111,
  B01110
};

byte Aire[] = {
  B00000,
  B00000,
  B01000,
  B10101,
  B00010,
  B01000,
  B10101,
  B00010
};

// FUNCION PARA HACER SONAR EL BEEPER ACTIVO
void beep()
{
    digitalWrite(BEEPER_PIN, LOW);
    delay(100); 
    digitalWrite(BEEPER_PIN, HIGH);
    delay(100); 
    digitalWrite(BEEPER_PIN, LOW);
    delay(100); 
    digitalWrite(BEEPER_PIN, HIGH);  
}

// INICIALIZO EL SCRIPT
void setup() {
  Serial.begin(57600);
  
  lcd.begin(16, 2);
  lcd.backlight();

  dht.begin();
  
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);

  // CREO CARACTERES ESPECIALES
  lcd.createChar(1, CaraMuyFeliz);
  lcd.createChar(2, CaraFeliz);
  lcd.createChar(3, CaraNeutral);
  lcd.createChar(4, CaraTriste);
  lcd.createChar(5, CaraMuyTriste);
  lcd.createChar(6, CaraGritando);
  lcd.createChar(7, Gota);
  lcd.createChar(8, Termometro);
  lcd.createChar(9, Aire);
  
  // EL SENSOR DEBE CALENTAR PARA REALIZAR MEDICIONES CORRECTAR POR LO CUAL LE DOY 30 SEGUNDOS DE TIEMPO PARA DICHA TAREA
  lcd.home();
  lcd.print("Iniciando...");
  
  for (int porcentual = 0; porcentual <= 100; porcentual++)
  {
    lcd.setCursor(0, 1);
    lcd.print(String(porcentual) + "%");
    delay(300);
  }
  
  lcd.clear();
}

void loop() {
  // LOS CICLOS DE REFRESCO SON CADA 1 SEGUNDOS
  delay(10);

  ReadCurrentMillis = millis();
  ReadElapsedMillis = ReadCurrentMillis - ReadStartMillis;

  // LEO LOS SENSORES CADA 5 SEGUNDOS
  
  if (ReadElapsedMillis >= 5000)
  {
    ReadStartMillis = millis();

    if (DHTTYPE == DHT11)
    {
      // EL DHT11 ES UNA BASURA ASI QUE HACEMOS ALGUNAS MAGIAS COMO PARA QUE LA HUMEDAD ESTE MAS CERCA DE LA REALIDAD
      humedad = dht.readHumidity();
      humedad = (100/ 40 * humedad) - 25;

      if (humedad > 100)
        humedad = 100;
    }
    else
    {
      humedad = dht.readHumidity();
    }

    humedad = dht.readHumidity();
    temperatura = dht.readTemperature() + DHT_OFFSET;

    // DEPENDE DEL SENSOR LA ESCALA PUEDE QUE SE REQUIERA AJUSTAR ASI QUE.... BUENO, ESO...
    // MULTIPLICO DE A 10 HASTA QUE LA MEDICION DE MAYOR A 100 YA QUE NUNCA HAY MENOS DE 100 PPM
    // EN LA ATMOSFERA... MAL POR LOS HUMANOS PERO ES ASI.
    
    for (int Escala=1; Escala <=10000; Escala = Escala * 10)
    {
        CO2_PPM = MQ135_SENSOR.getCorrectedPPM(temperatura, humedad) * Escala; 

        if (CO2_PPM > 100)
          break;
    }

    // IMPRIMO LA RZERO DE CALIBRACION
    Serial.print(MQ135_SENSOR.getRZero());
    Serial.print("|");

    // IMPRIMO LA TEMPERATURA EN CASO QUE SE QUIERA USAR EL EQUIPO CONECTADO A UNA PC PARA HACER LOGIN
    Serial.print(temperatura);
    Serial.print("|");

    // IMPRIMO LA HUMEDAD EN CASO QUE SE QUIERA USAR EL EQUIPO CONECTADO A UNA PC PARA HACER LOGIN
    Serial.print(humedad);
    Serial.print("|");

    // IMPRIMO EL CO2 EN CASO QUE SE QUIERA USAR EL EQUIPO CONECTADO A UNA PC PARA HACER LOGIN
    Serial.print(CO2_PPM);
    Serial.println();
    
    char str[3];
   
    // PRESENTO LA INFORMACION EN PANTALLA
    // ICONO DE AIRE
    lcd.setCursor(0, 0);
    lcd.write(9);
  
    lcd.setCursor(1, 0);
    lcd.print("              ");
    lcd.setCursor(1, 0);
    lcd.print(String(CO2_PPM) + " ppm");
  
    // ICONO DE GOTA
    lcd.setCursor(0, 1);
    lcd.write(7);
    
    lcd.setCursor(1, 1);
    lcd.print(String(dtostrf(humedad, 2, 1, str)) + "%");
  
    // ICONO TERMOMETRO
    lcd.setCursor(7, 1);
    lcd.write(8);
  
    lcd.setCursor(8, 1);
    lcd.print(String(dtostrf(temperatura, 2, 1, str)) +  "\337c");
  }

  // HAGO SONAR UNA ALERTA SI LA CONSENTRACION DE CO2 SUPERRA LOS 800, AUMENTANDO LA FRECUENCIA DE PITIDOS SEGUN CONCENTRACIÓN
  currentMillis = millis();
  elapsedMillis = (currentMillis - startMillis);

  if (CO2_PPM > 5000)
  {
    if (elapsedMillis > 1000)
    {
      beep();
      startMillis = millis();
    }

    lcd.setCursor(15, 0);
    lcd.write(6);

    return;
  }
  
  if (CO2_PPM > 3000 && elapsedMillis > 2000)
  {
    if (elapsedMillis > 2000)
    {
      beep();
      startMillis = millis();
    }

    lcd.setCursor(15, 0);
    lcd.write(6);

    return;
  }
  
  if (CO2_PPM > 2500)
  {
    if (elapsedMillis > 5000)
    {
      beep();
      startMillis = millis();
    }

    lcd.setCursor(15, 0);
    lcd.write(6);

    return;
  }
  
  if (CO2_PPM > 2000)
  {
    if (elapsedMillis > 10000)
    {
      beep();
      startMillis = millis();
    }

    lcd.setCursor(15, 0);
    lcd.write(5);

    return;
  }
  
  if (CO2_PPM > 1500)
  {
    if (elapsedMillis > 15000)
    {
      beep();
      startMillis = millis();
    }

    lcd.setCursor(15, 0);
    lcd.write(4);

    return;
  }
  
  if (CO2_PPM > 1000)
  {
    if (elapsedMillis > 30000)
    {
      beep();
      startMillis = millis();
    }

    lcd.setCursor(15, 0);
    lcd.write(3);

    return;
  }
  
  if (CO2_PPM > 800)
  {
    if (elapsedMillis > 60000)
    {
      beep();
      startMillis = millis();
    }
    
    lcd.setCursor(15, 0);
    lcd.write(3);

    return;
  }

  if (CO2_PPM <= 450)
  {
    startMillis = millis();
    
    lcd.setCursor(15, 0);
    lcd.write(2);

    return;
  }
  
  if (CO2_PPM <= 800)
  {
    startMillis = millis();

    lcd.setCursor(15, 0);
    lcd.write(2);

    return;
  }
}
