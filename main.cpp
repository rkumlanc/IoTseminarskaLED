#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

// Podatki za povezavo z WiFi
const char* ssid = "Telefonist";
const char* password = "Kucroo123#";

// Nastavitve za WebSocket
WebSocketsClient webSocket;
WebSocketsClient webSocketSend;
const char* websocket_server = "192.168.151.144";
int websocket_port = 8811;  // Port na katerem teče WebSocket strežnik za ESP32
int portNumber = 8888;

String dataString;

// Določitev pinov
const int PIN22 = 22;
const int PIN23 = 23;
const int PIN12 = 12;
const int PIN13 = 13;
const int buttonPin = 5;  // Pin, na katerem je priključen gumb

bool lastButtonState = LOW;  // Spremenljivka za shranjevanje zadnjega stanja gumba
void onWebSocketEvent(WStype_t type, uint8_t * payload, size_t lenght){
  Serial.println("On web socket event");
  Serial.println(type);
  switch(type){
    case WStype_TEXT:
      {
        //<256> določa kapaciteeto objekta
        StaticJsonDocument<256> doc;
        DeserializationError error=deserializeJson(doc, payload);
        if(error){
          Serial.print(F("deserializeJSON napaka"));
          Serial.println(error.c_str());
          return;
        }

        char tip[40];
        memset(tip, 0, 40); //pobrišemo vrednosti iz znakovnega niza tip
        strcpy(tip, doc["tipSporočila"]); //kopiramo tipSporočila v tip {LED}

        if(strcmp(tip, "LED")==0){
          Serial.println("Ukaz je led");
          int vrednost= doc["vrednost"];
          String pin=doc["pin"];
          if(pin=="PIN12"){
            Serial.println("PIN12");
            if(vrednost==0){digitalWrite(PIN12, 0);Serial.println("Prižgi PIN12");}
            else if (vrednost==1){digitalWrite(PIN12, 1);}}

          else if(pin=="PIN22"){
            Serial.println("PIN22");
            if(vrednost==0){digitalWrite(PIN22, 0);Serial.println("Prižgi PIN22");}
            else if (vrednost==1){digitalWrite(PIN22, 1);}
          }
          else if(pin=="PIN13"){
            Serial.println("PIN13");
            if(vrednost==0){digitalWrite(PIN13, 0);Serial.println("Prižgi PIN13");}
            else if (vrednost==1){digitalWrite(PIN13, 1);}
          }
          else if(pin=="PIN23"){
            Serial.println("PIN23");
            if(vrednost==0){digitalWrite(PIN23, 0);Serial.println("Prižgi PIN23");}
            else if (vrednost==1){digitalWrite(PIN23, 1);}
          }
             
          else if(pin=="PIN22_PIN12"){
            Serial.println("PIN22_PIN12");
            if(vrednost==0){
              digitalWrite(PIN12, 0);
              digitalWrite(PIN22, 0);              
            }
            else if (vrednost==1){
              digitalWrite(PIN12, 1);
              digitalWrite(PIN22, 1);
              
            }
          }
          else if(pin=="PIN23_PIN13"){
            Serial.println("PIN23_PIN13");
            if(vrednost==0){
              digitalWrite(PIN23, 0);
              digitalWrite(PIN13, 0);              
            }
            else if (vrednost==1){
              digitalWrite(PIN23, 1);
              digitalWrite(PIN13, 1);
              
            }
          }

          else if(pin=="LEDvse"){
            Serial.println("Vse");
            if(vrednost==0){
              digitalWrite(PIN12, 0);
              digitalWrite(PIN22, 0);
              digitalWrite(PIN13, 0);
              digitalWrite(PIN23, 0);
              Serial.println("Vklopi vse");
            }
            else if (vrednost==1){
              digitalWrite(PIN12, 1);
              digitalWrite(PIN22, 1);
              digitalWrite(PIN13, 1);
              digitalWrite(PIN23, 1);
            }
          }
          else{
            Serial.println("Ukaz ni prepoznan");
          }
          break;
        }
      }
     
    default:
      break;

  }
}

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  pinMode(PIN12, OUTPUT);
  pinMode(PIN22, OUTPUT);
  pinMode(PIN13, OUTPUT);
  pinMode(PIN23, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi");

  webSocket.begin(websocket_server, portNumber);
  webSocket.onEvent(onWebSocketEvent);
  webSocketSend.begin(websocket_server, websocket_port);
}

void loop() {
  bool currentButtonState = digitalRead(buttonPin);
  if (currentButtonState != lastButtonState) {
    if (currentButtonState == HIGH) {
      Serial.println("Gumb 5 je pritisnjen");
      StaticJsonDocument<256> doc;
      doc["tipSporočila"] = "LED";
      doc["pin"] = "LEDvse";
      doc["vrednost"] = 1;
      String output;
      serializeJson(doc, output);
      webSocketSend.sendTXT(output);
    }
    lastButtonState = currentButtonState;
    
  }
  webSocket.loop();
  webSocketSend.loop();
}
