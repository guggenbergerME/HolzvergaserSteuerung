#include <Arduino.h>
#include <SPI.h>
#include <ESP8266WiFi.h> 
#include <PubSubClient.h>   
#include <PCF8574.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/////////////////////////////////////////////////////////////////////////// Funktionsprototypen
void loop                       ();
void wifi_setup                 ();
void callback                   (char* topic, byte* payload, unsigned int length);
void reconnect                  ();
void temp_messen                ();
void mqtt_callback_aufrufen     ();

//////////////////////////////////////////////////////////////////////////// PCF8574 Adresse
PCF8574 pcf8574(0x20);
// Wemso SDA und SDC Pins

/////////////////////////////////////////////////////////////////////////// Schleifen verwalten
unsigned long previousMillis_temp_messen = 0; // Spannung Messen
unsigned long interval_temp_messen = 10000; 

unsigned long previousMillis_mqtt_callback = 0; // Spannung Messen
unsigned long interval_mqtt_callback = 1000; 

/////////////////////////////////////////////////////////////////////////// mqtt variable
char msgToPublish[60];
char stgFromFloat[10];
char textTOtopic[60];
const char* kartenID = "HolzvergaserSteuerung";

/////////////////////////////////////////////////////////////////////////// WIRE Bus
#define ONE_WIRE_BUS D5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

/////////////////////////////////////////////////////////////////////////// Temp. Sensor HEX zuweisen
DeviceAddress temp_abgas         = { 0x28, 0xAA, 0x6C, 0x90, 0x16, 0x13, 0x02, 0xC2 }; 
const char* topic_temp_abgas     = "Heizung/Holzvergaser/temp/abgas";

DeviceAddress temp_kessel         = { 0x28, 0xAA, 0x6C, 0x90, 0x16, 0x13, 0x02, 0xC2 }; 
const char* topic_temp_kessel     = "Heizung/Holzvergaser/temp/kessel";

DeviceAddress temp_rl            = { 0x28, 0xAA, 0x6C, 0x90, 0x16, 0x13, 0x02, 0xC2 }; 
const char* topic_temp_rl        = "Heizung/Holzvergaser/temp/ruecklauf";

DeviceAddress temp_vl            = { 0x28, 0xAA, 0x6C, 0x90, 0x16, 0x13, 0x02, 0xC2 }; 
const char* topic_temp_vl        = "Heizung/Holzvergaser/temp/vorlauf";


WiFiClient espClient;
PubSubClient client(espClient);

/////////////////////////////////////////////////////////////////////////// wifi daten
const char* ssid = "GuggenbergerLinux";
const char* password = "Isabelle2014samira";
const char* mqtt_server = "192.168.150.1";

/////////////////////////////////////////////////////////////////////////// Callback
void callback(char* topic, byte* payload, unsigned int length) {

    if (strcmp(topic,"Heizung/Holzvergaser/geblaese/IN")==0) {

        // Kanal A
        if ((char)payload[0] == 'o' && (char)payload[1] == 'n') {  
                 Serial.println("Abgas Gebläse AN");
                 //digitalWrite(relais_A, !HIGH);
                 //client.publish("Relaiskarte_0003/OUT/A","on");
                delay(100);
              }

        if ((char)payload[0] == 'o' && (char)payload[1] == 'f' && (char)payload[2] == 'f') {  
                 Serial.println("Abgas Gebläse AUS");
                 //digitalWrite(relais_A, !LOW);
                 //client.publish("Relaiskarte_0003/OUT/A","off");
                delay(100);
              }
      } 
 
    if (strcmp(topic,"Heizung/Holzvergaser/pumpe/heizkreis/IN")==0) {

        // Kanal A
        if ((char)payload[0] == 'o' && (char)payload[1] == 'n') {  
                 Serial.println("Pumpe Heizkreis AN");
                 //digitalWrite(relais_A, !HIGH);
                 //client.publish("Relaiskarte_0003/OUT/A","on");
                delay(100);
              }

        if ((char)payload[0] == 'o' && (char)payload[1] == 'f' && (char)payload[2] == 'f') {  
                 Serial.println("Pumpe Heizkreis AUS");
                 //digitalWrite(relais_A, !LOW);
                 //client.publish("Relaiskarte_0003/OUT/A","off");
                delay(100);
              }
      } 




}

/////////////////////////////////////////////////////////////////////////// Reconnect
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Baue Verbindung zum mqtt Server au. IP: ");
    Serial.print(mqtt_server);
    Serial.println("");
    // Create a random client ID
    String clientId = "HolzvergaserSteuerung-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT Verbunden ....");
      // ... and resubscribe
      client.subscribe("Heizung/Holzvergaser/geblaese/IN");
      client.subscribe("Heizung/Holzvergaser/pumpe/heizkreis/IN");

         
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/////////////////////////////////////////////////////////////////////////// SETUP - Wifi
void wifi_setup() {
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
}

/////////////////////////////////////////////////////////////////////////// SETUP
void setup() {

//*********************************************** MQTT Broker
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

//*********************************************** Serielle Kommunikation starten
  Serial.begin(115200);

//*********************************************** Wifi Setup
wifi_setup();

//*********************************************** pcf8574
  pcf8574.pinMode(P0, OUTPUT);
  pcf8574.pinMode(P1, OUTPUT);
  pcf8574.pinMode(P2, OUTPUT);
  pcf8574.pinMode(P3, OUTPUT); 
  pcf8574.pinMode(P4, OUTPUT); 
  pcf8574.pinMode(P5, OUTPUT); 
  pcf8574.pinMode(P6, OUTPUT); 
  pcf8574.pinMode(P7, OUTPUT);     
  pcf8574.begin();

  //*********************************************** Relais OFF
pcf8574.digitalWrite(P0, !LOW);
pcf8574.digitalWrite(P1, !LOW);
pcf8574.digitalWrite(P2, !LOW);
pcf8574.digitalWrite(P3, !LOW);
pcf8574.digitalWrite(P4, !LOW);
pcf8574.digitalWrite(P5, !LOW);
pcf8574.digitalWrite(P6, !LOW);
pcf8574.digitalWrite(P7, !LOW);

}

/////////////////////////////////////////////////////////////////////////// Temperatur auslesen
void temp_messen() {

  // Kessel Temp 
/*
  int currentTemp2 = sensors.getTempC(temp_kollektor);
  dtostrf(currentTemp2, 4, 2, stgFromFloat);

   if ((currentTemp2 == -127)||(currentTemp2 == 85))  { 
     } 
    else 
        {     
        
        sprintf(msgToPublish, "%s", stgFromFloat);
        sprintf(textTOtopic, "%s", topic_kollektor);
        client.publish(textTOtopic, msgToPublish);
        }
*/
  sprintf(textTOtopic, "%s", topic_temp_abgas);
  client.publish(textTOtopic, "11");

  // Vorlauf Temp
  sprintf(textTOtopic, "%s", topic_temp_kessel);
  client.publish(textTOtopic, "22");
  // Rücklauf Temp
  sprintf(textTOtopic, "%s", topic_temp_rl);
  client.publish(textTOtopic, "33");
  // Abgas Temp
  sprintf(textTOtopic, "%s", topic_temp_vl);
  client.publish(textTOtopic, "44");
}

/////////////////////////////////////////////////////////////////////////// mqtt Callback aufrufen
void mqtt_callback_aufrufen() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

/////////////////////////////////////////////////////////////////////////// LOOP
void loop() {



   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ mqtt Callback aufrufen
  if (millis() - previousMillis_mqtt_callback> interval_mqtt_callback) {
      previousMillis_mqtt_callback= millis(); 
      // Prüfen der Panelenspannung
      Serial.println("mqtt Callback aufrufen");
      mqtt_callback_aufrufen();
    } 


  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ Temperatur messen
  if (millis() - previousMillis_temp_messen > interval_temp_messen) {
      previousMillis_temp_messen= millis(); 
      // Prüfen der Panelenspannung
      Serial.println("Temperatur messen");
      temp_messen();
    }

 
}
