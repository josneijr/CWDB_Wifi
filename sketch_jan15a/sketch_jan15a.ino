#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

//WIFI INFO
const char* ssid = "dlink";
const char* password = "2222222222";

//Serial entre a Bluepill e a Node
SoftwareSerial STM32(D6, D5, false, 256);

int contadorMsgs = 0;

String bufferMsgs[100];

void LerSerial()
{
    //Mensagem recebida
    String message = "";

    //Recebe os dados da serial
    while(STM32.available())
    {
        message = STM32.readStringUntil('\r');
    }

    Serial.println("Mensagem original da serial: ");    
    Serial.println(message);

    message = message.substring(message.lastIndexOf('{'), message.lastIndexOf('}')+1);

    //Salva a mensagem no buffer
    bufferMsgs[contadorMsgs] = message;
    contadorMsgs++;

    //Imprime no debug do PC
    Serial.println("Mensagem tratada da serial: ");    
    Serial.println(message);
}

void EnviarWifi(){

    Serial.println("Enviando dados via wi-fi");
  
    //Declare object of class HTTPClient
    HTTPClient http;    

    //Specify request destination
    http.begin("http://cwdb.herokuapp.com/receiveData");      
    //Specify content-type header
    http.addHeader("Content-Type", "application/json;charset=utf-8");  

    String msgToSend = "[";

    for(int i=0; i<contadorMsgs; i++)
    {
        if(i>0)
        {
            msgToSend += ",";
        }

        msgToSend += bufferMsgs[i];
    }

    contadorMsgs = 0;
    msgToSend += "]";

    //Send the request
    int httpCode = http.POST(msgToSend);
    //Get the response payload
    String payload = http.getString();

    //Print HTTP return code
    Serial.print("HTTP response code: ");   
    Serial.println(httpCode);

    //Print request response payload
    Serial.print("HTTP response payload: ");
    Serial.println(payload);    

    //Close connection  
    http.end();  
}

void setup() {
    
    //Configura a conexão com a STM32 (Bluepill)
    STM32.begin(19200);
    //Timeout para evitar travar na aquisição de dados da Bluepill
    STM32.setTimeout(10000);
    
    //Abre o debug com o PC
    Serial.begin(115200);
    delay(10);
    
    // Connect to WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    
    //Tenta conexão com o wi-fi
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("");
    Serial.println("WiFi connected");
}

void loop() {
    //Lê os dados disponíveis da serial
    LerSerial();
  
    //Checa se o wi-fi está UP
    if (WiFi.status() == WL_CONNECTED) 
    {
        //Envia wifi
        if(contadorMsgs>0){
            EnviarWifi();  
        }
        else{
            Serial.println("Sem pontos para enviar agora");
        }
    }
    else 
    {
        Serial.println("Error in WiFi connection");
    }
  
    //Wait 1 second
    //delay(1000);  
}