// Este Projeto foi retirado do Livro IOT na prática do Curso Big Data e Inteligência Analítica da Faculdade Uniasselvi
// Este Código serve para Configurar o ESP32 na sua rede Wifi e Configurar o ThingSpeak para receber os dados do Sensor DHT11 para monitoramento da Temperatura Ambiente e Umidade do Ar
// Este Código pode ser reutilizado com outros sensores, bastando somente replicar a configuração de rede
// Para realizar a autenticação com o ThingSpeak, é necessário criar uma conta gratuita no site https://thingspeak.com/ e configurar seus canais.

// Chamando as bibliotecas

#include "ThingSpeak.h"
#include "DHT.h"
#include <WiFiClientSecure.h>

const char* ssid     = "ssid";     // Informar o ssid nome da rede Wifi
const char* password = "password";  // your network password
const char* server = "www.howsmyssl.com"; // Server URL // O certificado usado neste projeto é do exemplo WiFiClientSecure, caso a chave não funcione gere uma nova acessando o exemplo na biblioteca.
const char* test_root_ca= \
     "-----BEGIN CERTIFICATE-----\n" \
     "MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n" \
     "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
     "DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n" \
     "PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n" \
     "Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
     "AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n" \
     "rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n" \
     "OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n" \
     "xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n" \
     "7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n" \
     "aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n" \
     "HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n" \
     "SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n" \
     "ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n" \
     "AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n" \
     "R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n" \
     "JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL*T0yjWW06XyxV3bqxbYo\n" \
     "Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n" \
     "-----END CERTIFICATE-----\n";

//...BLOCO 1 - NOMEAR PINOS USADOS DO PROJETO...//

#define WriteAPIKey "API_KEY_NUMBER" // Informar a chave de escrita que foi cadastrada no ThingSpeak
#define Numero_Canal 0
#define DHTTYPE DHT11
#define DHTPIN 33

float umidade_ar = 0;
float temperatura = 0;

int LED1 = 25;
int LED2 = 26;
int LED3 = 27;
int LED4 = 14;
int LED5 = 12;
int LED6 = 13;

WiFiClient PCB_ESP32;
WiFiClientSecure client;

DHT SENSOR_DHT11(DHTPIN, DHTTYPE);

//...BLOCO 2 - CONFIGURAR PINOS USADOS NO PROJETO...//

void setup() 
{
// Configurando o ESP32 na rede WiFi, Este trecho de código foi reutilizado da biblioteca de exemplos WiFiClientSecure

   //Initialize serial and wait for port to open:
  Serial.begin(115200);
  delay(100);

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
  }

  Serial.print("Connected to:  ");
  Serial.println(ssid);

  client.setCACert(test_root_ca);
  //client.setCertificate(test_client_key); // for client verification
  //client.setPrivateKey(test_client_cert);  // for client verification

  Serial.println("\nStarting connection to server...");
  if (!client.connect(server, 443))
    Serial.println("Connection failed!");
  else {
    Serial.println("Connected to server!");
    // Make a HTTP request:
    client.println("GET https://www.howsmyssl.com/a/check HTTP/1.0");
    client.println("Host: www.howsmyssl.com");
    client.println("Connection: close");
    client.println();

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    // if there are incoming bytes available
    // from the server, read them and print them:
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }

    client.stop();
  }

// Configurando os Pinos do Projeto e o ThingSpeak  
 
  ThingSpeak.begin(PCB_ESP32);
  SENSOR_DHT11.begin();
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
}

//...BLOCO 3 - REPETIÇÃO DE TAREFAS...//

void loop()
{
  umidade_ar = SENSOR_DHT11.readHumidity();
  temperatura = SENSOR_DHT11.readTemperature();
 
  if (isnan(umidade_ar) || isnan(temperatura))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Umidade: ");
  Serial.print(umidade_ar);
  Serial.print("%  Temperatura: ");
  Serial.print(temperatura);
  Serial.println("°C ");
  
  if(temperatura <= 10)
  {
    digitalWrite(LED1,HIGH);
    digitalWrite(LED2,LOW);
    digitalWrite(LED3,LOW);
    digitalWrite(LED4,LOW);
    digitalWrite(LED5,LOW);
    digitalWrite(LED6,LOW);
  }
  else if(temperatura > 10 && temperatura <= 20)
  {
    digitalWrite(LED1,HIGH);
    digitalWrite(LED2,HIGH);
    digitalWrite(LED3,LOW);
    digitalWrite(LED4,LOW);
    digitalWrite(LED5,LOW);
    digitalWrite(LED6,LOW);
  }
  else if(temperatura > 20 && temperatura <= 30)
  {
    digitalWrite(LED1,HIGH);
    digitalWrite(LED2,HIGH);
    digitalWrite(LED3,HIGH);
    digitalWrite(LED4,LOW);
    digitalWrite(LED5,LOW);
    digitalWrite(LED6,LOW);
  }
  else if(temperatura > 30 && temperatura <= 40)
  { 
    digitalWrite(LED1,HIGH);
    digitalWrite(LED2,HIGH);
    digitalWrite(LED3,HIGH);
    digitalWrite(LED4,HIGH);
    digitalWrite(LED5,LOW);
    digitalWrite(LED6,LOW);
  }
  else if(temperatura > 40 && temperatura <= 50)
  { 
    digitalWrite(LED1,HIGH);
    digitalWrite(LED2,HIGH);
    digitalWrite(LED3,HIGH);
    digitalWrite(LED4,HIGH);
    digitalWrite(LED5,HIGH);
    digitalWrite(LED6,LOW);
  }
  else if(temperatura > 50)
  {
    digitalWrite(LED1,HIGH);
    digitalWrite(LED2,HIGH);
    digitalWrite(LED3,HIGH);
    digitalWrite(LED4,HIGH);
    digitalWrite(LED5,HIGH);
    digitalWrite(LED6,HIGH);
  }
  
// Configurando os parâmetros para que o ThingSpeak receba os dados.

  ThingSpeak.setField(1, temperatura);
  ThingSpeak.setField(2, umidade_ar);

  int x = ThingSpeak.writeFields(Numero_Canal, WriteAPIKey);

  if(x == 200)
  {
    Serial.println("Dados Enviados com Sucesso ao ThingSpeak");
  }
  else
  {
    Serial.println("Problemas no Envio de Mensagens. HTTP Error Code: " + String(x));
  }
  
  // Wait a few seconds between measurements.
  delay(15000);
  
}
