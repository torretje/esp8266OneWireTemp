#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 12
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;
const char* ssid = "fruitfly";
const char* password = "fruitfly";
MDNSResponder mdns;

ESP8266WebServer server(80);

const int led = 0;

void handleRoot() {
  sensors.begin();
  digitalWrite(led, 1);
  String message="{\"info\":{";
  // locate devices on the bus
  message+=countDevices();
  message+=",";
  message+=parasitePower();
  message+=",";
  message+=macAddr();
  message+="\",\"sensors\":";
  message+=readTemps();
  message+="}}";
  server.send(200, "text/plain", message);
  digitalWrite(led, 0);
}

String parasitePower(){
  String message="\"parasite_power\":\"";
  // locate devices on the bus
  message+=sensors.isParasitePowerMode();
  message+="\"";
  return message; 
}

String readTemps(){
  String message;
  int numberOfDevices = sensors.getDeviceCount();
  sensors.requestTemperatures();
  delay(850);
  message="[";
    for(int i=0;i<numberOfDevices; i++)
  {
     // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){
      message+="{";
     message+="\"address\":\"";
        for (uint8_t i = 0; i < 8; i++){
             message+=intToHex(tempDeviceAddress[i]);
         };
      message+="\",\"temp\":\"";
     // DeviceAddress deviceAddress=tempDeviceAddress[i];
     

      message+= sensors.getTempCByIndex(i);
      message+=(numberOfDevices-1==i)?"\"}":"\"},";
    }
  
  }
  message+="]";
  return message;  
}
  
String countDevices(){
  String message="\"devices\":";
  message+=sensors.getDeviceCount();
  return message;  
}

String macAddr(){
  String message="\"macaddress\":\"";
  message+=WiFi.macAddress();
  return message;
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}
 
void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    digitalWrite(led, 0);
    delay(250);
    digitalWrite(led, 1);
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  if (mdns.begin("tuin", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", handleRoot);
  
  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("HTTP server started");

  sensors.begin();

  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  int numberOfDevices = sensors.getDeviceCount();
  digitalWrite(led, 0);
  for(int i=0;i<numberOfDevices; i++)
  {
     // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){
      sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
      digitalWrite(led, 1);
      delay(750);
      digitalWrite(led, 0);
      delay(250);
      
    }
  }
}


void loop(void){
  server.handleClient();
} 

char TO_HEX(int i){
  return (i <= 9) ? '0' + i : 'A' - 10 + i;
}

String intToHex(int16_t x){
  char res[3];
  if (x <= 0xFFFF)
  {
    res[0] = TO_HEX(((x & 0xF0) >> 4));
    res[1] = TO_HEX(((x & 0x0F)));
    res[2] = '\0';
  }  
  return res; 
}
