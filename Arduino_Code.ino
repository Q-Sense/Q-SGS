#include <SPI.h>
#include <Ethernet.h>

int gas_sensor = 0;
int co_sensor = 0;
unsigned char sensorPin1 = 0;
unsigned char sensorPin2 = 5;
unsigned int SAMPLES = 3;
unsigned int setPoint = 500; 
unsigned int interval = 100; 
unsigned long counter = 0;

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x08, 0xE1 };
char serverName[] = "myhorta.zapto.org" ;
EthernetClient client;

void setup() {
   Serial.begin(9600);
   pinMode(2,OUTPUT);
   pinMode(3,OUTPUT);
   digitalWrite(2, HIGH);
   digitalWrite(3,HIGH);
   pinMode(sensorPin1,INPUT);
   pinMode(sensorPin2,INPUT);   
   counter = millis();
   Serial.println("Started");
   while((unsigned long)(millis()-counter) < 10*1000);
   Serial.println("3 minitues");
   counter = millis();
   
   if(Ethernet.begin(mac) == 0) 
   {
      Serial.println("MAC, failed");
      while(true);
   }
   delay(1000);
}
 
void loop() 
{  
  if((unsigned long)(millis()-counter) < 30*1000)
    return;
  counter = millis();
  Ethernet.maintain();
  gas_sensor = readGasSensor(sensorPin1, SAMPLES);
  co_sensor = readCoSensor(sensorPin2, SAMPLES);
  char data[64];
  sprintf(data,"co_sensor=%u&gas_sensor=%u&pass=Lagoa^2",co_sensor, gas_sensor);
  if (!client.connected()) 
  {
    Serial.println("Not connect.");
    int result = 0;
    if(result = client.connect(serverName, 8080) <0)
    {
      Serial.println("Connect failed.");
      Serial.println(result);
    }
  }
  char outBuf[64];
  client.println("POST /ReceiveValues.php HTTP/1.1");
  client.print("Host: ");
  client.println(serverName);
  client.println("Connection: close\r\nContent-Type: application/x-www-form-urlencoded");
  sprintf(outBuf,"Content-Length: %u\r\n",strlen(data));
  client.println(outBuf);
  client.print(data);
  Serial.println(data);
  client.stop();
}
 
//======================================
int readGasSensor(int PIN, int SAMPLES) {
  // The process of counting detector LPG sensors on analog port 0
  int avgValue;
  int val[SAMPLES];
  int runningTotal = 0;
 
  for(int x = 0; x < SAMPLES; x++)
  {
    val[x] = analogRead(PIN);
    runningTotal = runningTotal + val[x];
    delay(interval);
  }
  avgValue = runningTotal/SAMPLES;
  return avgValue;
}

int readCoSensor(int PIN, int SAMPLES) {
  int avgValue;
  int val[SAMPLES];
  int runningTotal = 0;
 
  for(int x = 0; x < SAMPLES; x++)
  {
    val[x] = analogRead(PIN);
    runningTotal = runningTotal + val[x];
    delay(interval);
  }
  avgValue = runningTotal/SAMPLES;
  return avgValue;
}
