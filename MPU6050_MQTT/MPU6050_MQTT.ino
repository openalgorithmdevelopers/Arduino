/*
 *  Mandatory includes
 */
#include <Arduino.h>
#include <TinyMPU6050.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "mine";
const char* password = "9560033900";
const char* mqtt_server = "broker.emqx.io";


WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
/*
 *  Constructing MPU-6050
 */
MPU6050 mpu (Wire);

/*
 *  Setup
 */

 
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


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
      if (client.connect(clientId.c_str()), "tornado", "") {
      Serial.println("connected");
      // Once connected, publish an announcement...
//      client.publish("/MPU6050/Arduino", "MPU6050 sending.....");
      client.subscribe("/DS/flask");        
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
  Serial.begin(9600);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
    
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Initialization
  mpu.Initialize();

  // Calibration
  Serial.println("=====================================");
  Serial.println("Starting calibration...");
  mpu.Calibrate();
  Serial.println("Calibration complete!");
  Serial.println("Offsets:");
  Serial.print("GyroX Offset = ");
  Serial.println(mpu.GetGyroXOffset());
  Serial.print("GyroY Offset = ");
  Serial.println(mpu.GetGyroYOffset());
  Serial.print("GyroZ Offset = ");
  Serial.println(mpu.GetGyroZOffset());
}

/*
 *  Loop
 */

int oldX = 0, oldY = 0, oldZ = 0, newX = 0, newY = 0, newZ = 0;

void loop() {

  mpu.Execute();
//  Serial.print("AngX = ");
//  Serial.print(oldX);
//  Serial.print("  /  AngY = ");
//  Serial.print(oldY);
//  Serial.print("  /  AngZ = ");
//  Serial.println(oldZ);
  newX = mpu.GetAngX();
  if(abs(oldX - newX) > 1)
    oldX = int(newX);
  newY = mpu.GetAngY();
  if(abs(oldY - newY) > 1)
    oldY = int(newY);
  newZ = mpu.GetAngZ();
  if(abs(oldZ - newZ) > 1)
    oldZ = int(newZ);  

  String a = "[" + String(newX) + "," + String(newY) + "," + String(newZ) + "]";
//  Serial.pr/intln(a);
  Serial.println(a.length());
  for(int i = 0; i < a.length(); i++)
    *msg = a[i];
  Serial.println(msg);
  client.publish("/MPU6050/Arduino", msg, 1);
}
