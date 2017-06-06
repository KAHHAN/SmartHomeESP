// ESP8266 DS18B20 ArduinoIDE Thingspeak IoT Example code
// http://vaasa.hacklab.fi
//
// https://github.com/milesburton/Arduino-Temperature-Control-Library
// https://gist.github.com/jeje/57091acf138a92c4176a


#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DallasTemperature.h>


#define ONE_WIRE_BUS_IN  4  // DS18B20 pin
#define ONE_WIRE_BUS_OUT 5  // DS18B20 pin
#define BUILTIN_LED  2


const char* host = "api.thingspeak.com"; // Your domain
String ApiKey = "AMJEB04HGN9OICD0";
String path = "/update?key=" + ApiKey + "&field1=";


OneWire oneWireIn(ONE_WIRE_BUS_IN);
DallasTemperature DS18B20_IN(&oneWireIn);

OneWire oneWireOut(ONE_WIRE_BUS_OUT);
DallasTemperature DS18B20_OUT(&oneWireOut);


const char* ssid = "Resident_Test";
const char* pass = "honeywell";
int i;
float temp;

float getTemperature(int i) {
  float temp;
  if (i==ONE_WIRE_BUS_IN)
  {
    do {
      DS18B20_IN.requestTemperatures();
      temp = DS18B20_IN.getTempCByIndex(0);
      delay(50);
    } while (temp == 85.0 || temp == (-127.0));
  }else
  {
    do {
      DS18B20_OUT.requestTemperatures();
      temp = DS18B20_OUT.getTempCByIndex(0);
      delay(50);
    } while (temp == 85.0 || temp == (-127.0));
  }

  return temp;
}


void setup(void){
  pinMode(ONE_WIRE_BUS_IN,OUTPUT);
  pinMode(ONE_WIRE_BUS_OUT,OUTPUT);
  //pinMode(BUILTIN_LED, OUTPUT);

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

  DS18B20_IN.begin();
  DS18B20_OUT.begin();

  long duration, distance;
  char temperatureString_IN[6];
  char temperatureString_OUT[6];

  float temperature_IN = getTemperature(ONE_WIRE_BUS_IN);

  dtostrf(temperature_IN, 2, 2, temperatureString_IN);

  // send temperature to the serial console
  Serial.println("Temp In:");
  Serial.println(temperatureString_IN);

  float temperature_OUT = getTemperature(ONE_WIRE_BUS_OUT);

  dtostrf(temperature_OUT, 2, 2, temperatureString_OUT);

  // send temperature to the serial console
  Serial.println("Temp Out:");
  Serial.println(temperatureString_OUT);


  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  client.print(String("GET ") + path + temperatureString_IN + "&field2=" + temperatureString_OUT + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: keep-alive\r\n\r\n");
  delay(500);

  ESP.deepSleep(60e6);
}


void loop() {
}
