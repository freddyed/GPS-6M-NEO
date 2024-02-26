#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int RXPin = 10, TXPin = 11;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

int TimeZoneHour = 5; //Zona horaria Peru(diferencia horaria Londres GMT-Peru)
int DaysInMonth[12]={31,28,31,30,31,30,31,31,30,31,30,31}; //Declaramos la cantidad de dias en cada mes del año


void setup()
{

  Serial.begin(9600);
  ss.begin(GPSBaud);
  Serial.println(F("  Latitud    Longitud       TimeStamp       Altitud "));
  Serial.println(F("----------------------------------------------------"));
}

void loop()
{
  smartDelay(1000);
  Serial.print(" ");
  printlatlon(gps.location.lat(), 6);
  printlatlon(gps.location.lng(), 6);
  printDateTime(gps.date, gps.time);
  printalt(gps.altitude.meters(), 2);
  Serial.println(" ");
  //delay(800);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}

static void smartDelay(unsigned long ms)  //esta funcion "alimenta" al GPS para que funcione, como el activador
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static void printlatlon(float val, int prec)
{
  if (!gps.location.isValid())
  {
    Serial.print("*********"); //Aqui escribimos el coodigo que queremos enviar si hay error de lectura
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    Serial.print(' ');
  }
}

static void printalt(float val, int prec)
{
  if (!gps.altitude.isValid())
  {
    Serial.print("*******"); //Aqui escribimos el coodigo que queremos enviar si hay error de lectura
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    Serial.print(' ');
  }
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  byte second=t.second();
  byte minute=t.minute();
  byte hour=t.hour();
  byte day=d.day();
  byte month=d.month();
  int year=d.year();
  if (!d.isValid() && !t.isValid() )
  {
    Serial.print(F("********** ******** ")); //Aqui escribimos el coodigo que queremos enviar si hay error de lectura
  }
  else
  {
     //Agregando correccion al horario 
    if(hour >= TimeZoneHour) {
       hour = hour - TimeZoneHour ;
     } else {
       // Correct for period when London is a day ahead
       hour = (hour + 24) - TimeZoneHour ;
       day  = day - 1 ;
       if( day == 0)
       {
         day   = DaysInMonth[ month - 1] ;  // 0 - 11
         month = month - 1 ;  // January in London still December Westward
         if( month == 0)           // GPS months are 01 through 12
         {
           month = 12 ;
           day   = DaysInMonth[ 11 ] ;  // ultimo dia de diciembre
         }
        // En el caso del mes de febrero para años bisciestos
         if( month == 2 && (year % 4)==0 ) day = day + 1 ;
       }
     }
  //Finalizando codigo agregado  
    char sz[32];
    sprintf(sz, "%02d-%02d-%02dT%02d:%02d:%02dZ ", year, month, day, hour, minute, second);//2020-08-28T17:13:40Z
    Serial.print(sz);
  }
}
