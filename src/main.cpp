#include <Arduino.h>
#include <SPI.h>
#include <ESP8266WiFi.h> 
#include <PubSubClient.h>   

const byte relais_A = D0;       
const byte relais_B = D5;      
const byte relais_C = D6;       
const byte relais_D = D7; 

// Kartendaten

const char* kartenID = "Relaiskarte_0003";

WiFiClient espClient;
PubSubClient client(espClient);

// Connect to the WiFi
const char* ssid = "GuggenbergerLinux";
const char* password = "Isabelle2014samira";
const char* mqtt_server = "192.168.150.1";
//const char* mqtt_topic_a = "192.168.178.222";


void callback(char* topic, byte* payload, unsigned int length) {
/*
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  Serial.println();
*/


    if (strcmp(topic,"Relaiskarte_0003/IN/A")==0) {

        // Kanal A
        if ((char)payload[0] == 'o' && (char)payload[1] == 'n') {  
                 Serial.println("relais_A -> AN");
                 digitalWrite(relais_A, !HIGH);
                 client.publish("Relaiskarte_0003/OUT/A","on");
                delay(100);
              }

        if ((char)payload[0] == 'o' && (char)payload[1] == 'f' && (char)payload[2] == 'f') {  
                 Serial.println("relais_A -> AUS");
                 digitalWrite(relais_A, !LOW);
                 client.publish("Relaiskarte_0003/OUT/A","off");
                delay(100);
              }
      } 



    if (strcmp(topic,"Relaiskarte_0003/IN/B")==0) {

        // Kanal B
        if ((char)payload[0] == 'o' && (char)payload[1] == 'n') {  
                 Serial.println("relais_B -> AN");
                 digitalWrite(relais_B, !HIGH);
                 client.publish("Relaiskarte_0003/OUT/B","on");                 
                delay(100);
              }

        if ((char)payload[0] == 'o' && (char)payload[1] == 'f' && (char)payload[2] == 'f') {  
                 Serial.println("relais_B -> AUS");
                 digitalWrite(relais_B, !LOW);
                 client.publish("Relaiskarte_0003/OUT/B","off");
                delay(100);
              }
      }      


    if (strcmp(topic,"Relaiskarte_0003/IN/C")==0) {

        // Kanal C
        if ((char)payload[0] == 'o' && (char)payload[1] == 'n') {  
                 Serial.println("relais_C -> AN");
                 digitalWrite(relais_C, !HIGH);
                 client.publish("Relaiskarte_0003/OUT/C","on");                 
                delay(100);
              }

        if ((char)payload[0] == 'o' && (char)payload[1] == 'f' && (char)payload[2] == 'f') {  
                 //digitalWrite(relais_A, !LOW);
                 Serial.println("relais_C -> AUS");
                 digitalWrite(relais_C, !LOW);
                 client.publish("Relaiskarte_0003/OUT/C","off");
                delay(100);
              }
      }


    if (strcmp(topic,"Relaiskarte_0003/IN/D")==0) {

        // Kanal D
        if ((char)payload[0] == 'o' && (char)payload[1] == 'n') {  
                 Serial.println("relais_D -> AN");
                 digitalWrite(relais_D, !HIGH);
                 client.publish("Relaiskarte_0003/OUT/D","on");                 
                delay(100);
              }

        if ((char)payload[0] == 'o' && (char)payload[1] == 'f' && (char)payload[2] == 'f') {  
                 Serial.println("relais_D -> AUS");
                 digitalWrite(relais_D, !LOW);
                 client.publish("Relaiskarte_0003/OUT/D","off");
                delay(100);
              }
      }


}



void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Baue Verbindung zum mqtt Server au. IP: ");
    Serial.print(mqtt_server);
    Serial.println("");
    // Create a random client ID
    String clientId = "Haussteuerung-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("Verbunden ....");
      // ... and resubscribe
      client.subscribe("Relaiskarte_0003/IN/A");
      client.subscribe("Relaiskarte_0003/IN/B");
      client.subscribe("Relaiskarte_0003/IN/C");
      client.subscribe("Relaiskarte_0003/IN/D");           
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {

//Pins deklarieren
  pinMode(relais_A, OUTPUT);
  pinMode(relais_B, OUTPUT);
  pinMode(relais_C, OUTPUT);
  pinMode(relais_D, OUTPUT);

// Alle Relais auf LOW setzen

  digitalWrite(relais_A, !LOW);
  digitalWrite(relais_B, !LOW);
  digitalWrite(relais_C, !LOW);
  digitalWrite(relais_D, !LOW);

// MQTT Broker
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

// Serielle Kommunikation starten
  Serial.begin(115200);

  // Verbindung zum WiFI aufbauen

  Serial.print("Verbindung zu SSID -> ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Erfolgreich mit dem WiFi verbunden!");
  Serial.print("DHCP Adresse der Relaikarte : ");
  Serial.println(WiFi.localIP());
  Serial.print("ID der Relaiskarte: ");
  Serial.println(kartenID);

// Start mit Relais klack bestaetigen

digitalWrite(relais_A, !LOW);
delay(300);
digitalWrite(relais_A, !HIGH);
delay(300);
digitalWrite(relais_A, !LOW);
delay(300);
digitalWrite(relais_A, !HIGH);
delay(300);
digitalWrite(relais_A, !LOW);
}


void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  
}
