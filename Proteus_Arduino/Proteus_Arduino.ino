#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11 // DHT 22 (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
const int current_sensor = A3;
int adc;
float voltage,current;
float Vout = 0.0;
float R1 = 30000.0;
float R2 = 7500.0;
int value = 0;
const int voltagesensor = A1;
const int relay = 3;
String str,message;

void setup()
{
Serial.begin(9600);
Serial1.begin(9600);
pinMode(relay,OUTPUT);
dht.begin();
delay(2000);
}
void loop()
{
//current sensor code
adc = analogRead(current_sensor);
voltage = adc * 5 / 1023.0;
current = ((voltage - 2.5) / 0.066);

//voltage sensor code
value = analogRead(voltagesensor);
Vout = (value*5.0)/1024.0;

// Read temperature as Celsius (the default)
float t = dht.readTemperature();
if(Serial1.available()){
  message = Serial1.readStringUntil('$');
  Serial.println(String("From NodeMCU :") + message);
  if(message.equals("TURN ON")) digitalWrite(relay,LOW);
  if(message.equals("TURN OFF")) digitalWrite(relay,HIGH);
}
/*Serial.print("Current : ");
Serial.print(current);
Serial.print("Voltage : ");
Serial.println(Vout);
Serial.print(" T: ");
Serial.print(t);
Serial.println("C");*/
str = String("Current= ")+String(current)+" "+String("Voltage= ")+String(Vout)+" "+String("Temperature= ")+String(t)+"$";
Serial.println("STR: "+str);
Serial1.print(str);
delay(1000);
}