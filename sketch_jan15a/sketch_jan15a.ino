#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>


const char* ssid = "dlink";//type your ssid
const char* password = "2222222222";//type your password

String mensagem = "";
const int timeout = 1000;

WiFiServer server(80);//Service Port

SoftwareSerial STM32(D5,D6);


void setup() {
  STM32.begin(19200);
  Serial.begin(115200);
  STM32.setTimeout(timeout);
  delay(10);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {

  //a cada 3 minutos

  
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    while(STM32.available())
    {
    mensagem = STM32.readString();
    //STM32.setTimeout(1000);
    STM32.flush();
     } 
     
    HTTPClient http;    //Declare object of class HTTPClient

    
    //mensagem = STM32.readString();
    http.begin("http://cwdb.herokuapp.com/receiveData");      //Specify request destination
    http.addHeader("Content-Type", "application/json;charset=utf-8");  //Specify content-type header

    String message = "{";

    message += "}";
    //int httpCode = http.POST("{\"timestamp\": \"15/01/2018 22:26:49\", \"lat\": \"-25.45\", \"long\": \"-49.29\", \"value\": \"67\"}");   //Send the request
    int httpCode = http.POST(mensagem);

   
    String payload = http.getString();                  //Get the response payload

    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload
    
    http.end();  //Close connection
    Serial.println(mensagem);
   // STM32.flush();
  }
  else {
     Serial.println("Error in WiFi connection");
  }

  delay(10000);  //Send a request every 30 seconds
}

