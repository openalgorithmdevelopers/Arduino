/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "FS.h"
// Update these with values suitable for your network.

const char* ssid = "mine";
const char* password = "9560033900";
const char* mqtt_server = "broker.emqx.io";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];

//int SIGNAL_DURATION = 10000;  //provide in millisec
int SIGNAL_DURATION = 1000;  //provide in millisec

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
//    if (client.connect(clientId.c_str())) {
      if (client.connect(clientId.c_str()), "tornado", "") {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "Digital Stethoscope sending.....");
      // ... and resubscribe
//      client.subscribe("inTopic");
        client.subscribe("/leds/esp8266");
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
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  
//  if (!client.connected()) {
//    reconnect();
//  }
//  client.loop();
  
  read_and_write_signal();
}

void loop() {
  
}

void read_and_write_signal(){
  String DS_signal = "";
  File file;
   
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
 
  file = SPIFFS.open("/stethoscope_signal.txt", "w");
 
  if (!file) {
    Serial.println("There was an error opening the file for writing");
    return;
  }
  
  DS_signal = ":-3:";  // start of signal
  if (file.print(DS_signal)) {
    Serial.println("File was written with Starting character");
  } else {
    Serial.println("File write failed");
  }  

  int loopCounter = 0;
  while(loopCounter < SIGNAL_DURATION){
    loopCounter += 1;  
    DS_signal = String(loopCounter++) + ":";

    int sample = analogRead(A0);
    Serial.print(sample);
    DS_signal += String(sample) + ",";
    
    if (file.print(DS_signal)) {
      Serial.println("File was written with ");
      Serial.println(DS_signal);
    } else {
      Serial.println("File write failed");
    }
    delay(1); 
  }
  
  if (file.print(":-5:")) { //end of signal
    Serial.println("File was written with End Character");
  } else {
    Serial.println("File write failed");
  }

  file.close();

  Serial.println("Signal recording on a file completed..");
}

void Send_FIle_MQTT(){
  File file;
  
  file = SPIFFS.open("/stethoscope_signal.txt", "r"); 

  if (!file) {
    Serial.println("There was an error reading the content of the file");
    return;
  } 
  
  Serial.println("reading");
  while(file.available()){
      String signal_part, tmp;
      int counter = 0;
      char *tmp2;

      tmp2 = msg; 
      while(counter < MSG_BUFFER_SIZE - 1){
        if(file.available()){
          int ch = file.read();
            msg[counter] = ch;
        }
        counter += 1;
      }
      client.publish("outSignal", msg, 1);
      Serial.println("Published: ");
      Serial.println(msg);
      delay(1000);
  }
  Serial.println("reading and publishing ends");
  Serial.println(file.size());
  file.close();
}
