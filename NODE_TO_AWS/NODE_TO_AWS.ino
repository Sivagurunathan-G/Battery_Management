#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "secrets.h"
#define TIME_ZONE -5

String received_data,str;
String strs[20];
int StringCount = 0;
int entry_num=1;
float t,current,voltage;
unsigned long lastMillis = 0;
unsigned long previousMillis = 0;
const long interval = 5000;
 
#define AWS_IOT_PUBLISH_TOPIC   "bms/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "bms/sub"
 
WiFiClientSecure net;
 
BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);
 
PubSubClient client(net);
 
time_t now;
time_t nowish = 1510592825;
 
 
void NTPConnect(void)
{
  Serial.print("Setting time using SNTP");
  configTime(TIME_ZONE * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while (now < nowish)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("done!");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}
 
 
void messageReceived(char *topic, byte *payload, unsigned int length)
{
  char message[9];
  int i=0,j=0;
 /* Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");*/
  for (i = 0,j=0; i < length && j < length; i++,j++)
  {
    //Serial.print((char)payload[i]);
    message[j]=(char)payload[i];
  }
  message[j]='$';
  Serial.print(String(message));
  //Serial.println();
}
 
 
void connectAWS()
{
  delay(3000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println(String("Attempting to connect to SSID: ") + String(WIFI_SSID));
 
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
 
  NTPConnect();
 
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);
 
  client.setServer(MQTT_HOST, 8883);
  client.setCallback(messageReceived);
 
 
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(1000);
  }
 
  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
 
  Serial.println("AWS IoT Connected!");
}
 
 
void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["entry_num"]=entry_num;
  doc["current"]=current;
  doc["voltage"]=voltage;
  doc["time"] = millis();
  doc["temperature"] = t;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}
 
 
void setup()
{
  Serial.begin(9600);
  connectAWS();
}
 
 
void loop()
{
  //h = dht.readHumidity();
  //t = dht.readTemperature();
int StringCount = 0;
if (Serial.available()) {
 received_data=Serial.readStringUntil('$');
 str=received_data;
 //Serial.println(received_data+" "+ received_data.length());
 while (str.length() > 0)
  {
    int index = str.indexOf(' ');
    if (index == -1) // No space found
    {
      strs[StringCount++] = str;
      break;
    }
    else
    {
      strs[StringCount++] = str.substring(0, index);
      str = str.substring(index+1);
    }
  }
}
  current=strs[1].toFloat();
  voltage=strs[3].toFloat();
  t=strs[5].toFloat();
  if (isnan(current) || isnan(t) || isnan(voltage))  // Check if any reads failed and exit early (to try again).
  {
    Serial.println(F("Failed to read from sensor!"));
    return;
  }
  /*Serial.print(F("Current: "));
  Serial.print(current);
  Serial.print(F(" Volatge: "));
  Serial.print(voltage);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));*/
 
  now = time(nullptr);
 
  if (!client.connected())
  {
    connectAWS();
  }
  else
  {
    client.loop();
    if (millis() - lastMillis > 10000)
    {
      lastMillis = millis();
      publishMessage();
    }
  }
}