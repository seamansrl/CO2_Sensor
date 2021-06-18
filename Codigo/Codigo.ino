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

// DEFINO EL PIN DONDE SE CONECTARA EL SENSOR MQ135
#define MQ135_PIN 0

// DEFINO EL PIN DONDE SE CONECTARA EL SENSOR DHT11
#define DHT_PIN 5

// DEFINO EL PIN DONDE SE CONECTARA EL BEEPER ACTIVO
#define BEEPER_PIN 3

// PARAMETRIZO SENSOR DHT
DHT dht(DHT_PIN, DHTTYPE);

// VARIABLES GLOBALES PARA PrintLCD
String Last_Line1 = "";
String Last_Line2 = "";

int Line1_Position = 0;
int Line2_Position = 0;

// FUNCION CREADA PARA HACERME LA VIDA MAS SIMPLE CON LAS PANTALLAS LCD
void PrintLCD(String Line1 = "", String Line2 = "")
{
  String Temporal_Line1 = "";
  String Temporal_Line2 = "";
  bool NeedReferesh = false;
  
  if (Last_Line1 != Line1 || Last_Line2 != Line2)
  {    
    if (Line1 == "" && Line2 == "")
      lcd.clear();
    else
    {
      if (Line1 == "$LAST")
        Line1 = Last_Line1;

      if (Line2 == "$LAST")
        Line2 = Last_Line2;
    
      if (Last_Line1 != Line1)
      {
        Line1_Position = 0;
        NeedReferesh = true;
        lcd.home();
        lcd.print("                ");
      }
      
      if (Last_Line2 != Line2)
      {
        Line2_Position = 0;
        NeedReferesh = true;
        lcd.setCursor(0, 1);
        lcd.print("                ");
      }
      
      Temporal_Line1 = Line1;
      Temporal_Line2 = Line2;
    }

    Last_Line1 = Line1;
    Last_Line2 = Line2;
  }

  if (Line1.length() > 16)
  {
    if (Line1_Position > 5)
    {
      Temporal_Line1 = String(Line1 + "                     ").substring(Line1_Position - 5);
      NeedReferesh = true;
      lcd.home();
      lcd.print("               ");
    }
    else
      Temporal_Line1 = Line1;
    
    if (Line1_Position > String(Line1 + "                     ").length() - 16)
      Line1_Position = 0;
    else
      Line1_Position++;
  }
  else
  {
    Temporal_Line1 = Line1;
  }

  if (Line2.length() > 16)
  {
    if (Line2_Position > 5)
    {
      Temporal_Line2 = String(Line2 + "                     ").substring(Line2_Position - 5);
      NeedReferesh = true;
      lcd.setCursor(0, 1);
      lcd.print("                ");
    }
    else
      Temporal_Line2 = Line2;
    
    if (Line2_Position > String(Line2 + "                     ").length() - 16)
      Line2_Position = 0;
    else
      Line2_Position++;
  }
  else
  {
    Temporal_Line2 = Line2;
  }

  if (NeedReferesh == true)
  {
    lcd.home();
    lcd.print(Temporal_Line1);
    lcd.setCursor(0, 1);
    lcd.print(Temporal_Line2);

    delay(250);
  }
}

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

  // EL SENSOR DEBE CALENTAR PARA REALIZAR MEDICIONES CORRECTAR POR LO CUAL LE DOY 30 SEGUNDOS DE TIEMPO PARA DICHA TAREA
  PrintLCD("CALENTANDO SENSOR...");
  
  for (int porcentual = 0; porcentual <= 100; porcentual++)
  {
    PrintLCD("$LAST", String(porcentual) + "%");
    delay(300);
  }

  // AVISO QUE SE INICIO
  beep();
}

void loop() {
  // OBTENGO DATOS DE TEMPERATURA Y HUMEDAD DEL DHT11
  float humedad = dht.readHumidity();
  float temperatura = dht.readTemperature();

  // USO LOS DATOS DEL DTH11 PARA HACER CORRECCIONES DE MEDICION EN EL SENSOR MQ135 Y OBTENGO LAS PARTES POR MILLON DE CO2
  float CO2_PPM = MQ135(MQ135_PIN).getCorrectedPPM(temperatura, humedad); 

  // PRESENTO LA INFORMACION EN PANTALLA
  PrintLCD("CO2: " + String(CO2_PPM) + " PPM", String(humedad) + "%  " + String(temperatura) + "* C");

  // HAGO SONAR UNA ALERTA SI LA CONSENTRACION DE CO2 SUPERRA LOS 800, VALOR CONSIDERADO COMO EL MAXIMO SUPERIOR ACEPTABLE
  if (CO2_PPM > 800)
    beep();

  // LOS CICLOS DE REFRESCO SON CADA 2 SEGUNDOS
  delay(2000);
}
