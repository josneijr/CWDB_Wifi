#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

const char* ssid = "dlink";//type your ssid
const char* password = "2222222222";//type your password

SoftwareSerial STM32(D5, D6, false, 256);

String message = "";

void LerSerial()
{
    Serial.println("Mensagem da serial: ");
     while(STM32.available())
     {
          message = STM32.readStringUntil('\r');
     }

     
     Serial.println(message);
}

void setup() {
  STM32.begin(19200);
  STM32.setTimeout(10000);
  
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {

  LerSerial();
  
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    HTTPClient http;    //Declare object of class HTTPClient

    http.begin("http://cwdb.herokuapp.com/receiveData");      //Specify request destination
    http.addHeader("Content-Type", "application/json;charset=utf-8");  //Specify content-type header
  
    int httpCode = http.POST("[{\"timestamp\": \"15/01/2018 22:26:49\", \"lat\": \"-25.45\", \"long\": \"-49.29\", \"value\": \"67\"}]");   //Send the request
    String payload = http.getString();                  //Get the response payload

    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload

    http.end();  //Close connection

  }
  else {
     Serial.println("Error in WiFi connection");
  }

  delay(1000);  //Send a request every 30 seconds


}