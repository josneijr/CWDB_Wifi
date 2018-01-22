#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

const char* ssid = "dlink";//type your ssid
const char* password = "2222222222";//type your password

//Contador usado para andar no array
int cont = 0;
//Array contendo as últimas N mensagens completas
String msgArray[100];
//Buffer para mensagens incompletas
String msgBuffer;

SoftwareSerial STM32(D5,D6,false,256);

void setup() 
{  
    STM32.begin(19200);
    Serial.begin(115200);
    
    STM32.setTimeout(1000);
    delay(10);

    ConectaWifi();
}

void ConectaWifi()
{
    // Connect to WiFi network
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
  
    // Print the IP address
    Serial.print("IP: ");
    Serial.print(WiFi.localIP());  
}

void LerSerial()
{
    while(STM32.available())
    {
        msgBuffer = STM32.readStringUntil('}');
        STM32.readString();

        Serial.println(msgBuffer);
        msgArray[cont] = msgBuffer;

        cont++;
    }   
}

void EnviarWifi()
{
    //Criamos um objeto de conexão para esse cliente
    HTTPClient http;

    //URL para a qual vamos enviar
    http.begin("http://cwdb.herokuapp.com/receiveData");
    //Header define que vamos enviar um JSON
    http.addHeader("Content-Type", "application/json;charset=utf-8");

    String message = "[";

    for(int i=0; i<cont; i++)
    {
        if(msgArray[i]==0)
            return;
      
        if(i!=0)
            message += ",";
      
        message += msgArray[i];

        message += "}";
    }
  
    message += "]";

    Serial.println(message);

    //Retorno do HTTP (200 = OK; qualquer outro número = erro)
    int httpCode = http.POST(message);

    //Se conseguiu enviar
    if(httpCode == 200)
    {
        //Apagar o array
        memset(msgArray, 0, sizeof(msgArray));    
        cont = 0;      
    }

    //Pegar a resposta do payload
    String payload = http.getString();

    //Imprime o código HTTP de retorno
    Serial.println(httpCode);
    //Imprime a resposta de payload
    Serial.println(payload);

    //Fecha a comunicação do cliente
    http.end();
}

void loop() 
{
    //Vamos ler a serial toda vez
    LerSerial();

    Serial.println("Loop");
    
    //Wifi conectado?
    if (WiFi.status() == WL_CONNECTED) 
    {
        //Vamos escrever na serial quando temos dados no array
        if(cont>0)
        {
            EnviarWifi();
        }
    }
    else 
    {
        Serial.println("Error in WiFi connection");

        WiFi.disconnect();
        ConectaWifi();
    }

    //Aguardar 1 segundo
    delay(1000);  
}

