#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT22 // DHT 22 (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
const int current_sensor = A0;
int adc;
float voltage,current;
float V1 = 0.0;
float Vout = 0.0;
float R1 = 30000.0;
float R2 = 7500.0;
int value = 0;
const int voltagesensor = A1;
const int relay = 3;
String str,message;

void setup()
{
Serial.begin(115200);
Serial1.begin(115200);
pinMode(relay,OUTPUT);
dht.begin();
delay(2000);
}
void loop()
{
//current sensor code
adc = analogRead(A0);
voltage = adc * 5 / 1023.0;
current = ((voltage - 2.5) / 0.100)-13.43;

//voltage sensor code
value = analogRead(voltagesensor);
V1 = (value*5.0)/1024.0;
Vout = (V1/(R2/(R1+R2)))-0.7;

// Read temperature as Celsius (the default)
float t = dht.readTemperature();
if(Serial1.available()){
  message = Serial1.readStringUntil('$');
  Serial.println(String("From NodeMCU :") + message);
  if(message.equals("TURN ON")) digitalWrite(relay,LOW);
  if(message.equals("TURN OFF")) digitalWrite(relay,HIGH);
}
Serial.print("Current : ");
Serial.print(current);
Serial.print("Voltage : ");
Serial.println(Vout);
Serial.print(" T: ");
Serial.print(t);
Serial.println("C");
str = String("Current= ")+String(current)+" "+String("Voltage= ")+String(Vout)+" "+String("Temperature= ")+String(t)+"$";
//Serial.println("STR: "+str);
Serial1.print(str);
delay(1000);
}