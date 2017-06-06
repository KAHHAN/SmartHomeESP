// ESP8266 DS18B20 ArduinoIDE Thingspeak IoT Example code
// http://vaasa.hacklab.fi
//
// https://github.com/milesburton/Arduino-Temperature-Control-Library
// https://gist.github.com/jeje/57091acf138a92c4176a


#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DallasTemperature.h>


#define ONE_WIRE_BUS 0  // DS18B20 pin
#define TRIGGER      5
#define ECHO         4
#define BUILTIN_LED  2


const char* host = "api.thingspeak.com"; // Your domain
String ApiKey = "AMJEB04HGN9OICD0";
String path = "/update?key=" + ApiKey + "&field1=";


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

const char* ssid = "Resident_Test";
const char* pass = "honeywell";
int i;
float temp;


void setup(void){
  pinMode(ONE_WIRE_BUS,OUTPUT);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);

  Serial.begin(115200);
  Serial.println("");

  WiFi.begin(ssid, pass);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  DS18B20.begin();


}

float getTemperature() {
  float temp;
  do {
    DS18B20.requestTemperatures();
    temp = DS18B20.getTempCByIndex(0);
    delay(50);
  } while (temp == 85.0 || temp == (-127.0));
  return temp;
}


void loop() {

  long duration, distance;
  char temperatureString[6];
  char DistanceString[6];

  float temperature = getTemperature();

  dtostrf(temperature, 2, 2, temperatureString);

  // send temperature to the serial console
  Serial.println("Temp:");
  Serial.println(temperatureString);

  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = (duration/2) / 29.1;
  dtostrf(distance, 2, 2, DistanceString);

  Serial.println("Centimeter:");
  Serial.println(distance);



  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  client.print(String("GET ") + path + temperatureString + "&field2=" + DistanceString + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: keep-alive\r\n\r\n");
  delay(500);

}
