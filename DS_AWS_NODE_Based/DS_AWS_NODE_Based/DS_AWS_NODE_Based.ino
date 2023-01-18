#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "FS.h"
// Update these with values suitable for your network.
#include <pgmspace.h>
 
#define SECRET

#define THINGNAME "trying_3"

 
int8_t TIME_ZONE = -5; //NYC(USA): -5 UTC

const char WIFI_SSID[] = "mine";               //my wifi
const char WIFI_PASSWORD[] = "9560033900";           //

WiFiClient espClient;

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];

//int SIGNAL_DURATION = 10000;  //provide in millisec
int SIGNAL_DURATION = 1000;  //provide in millisec


//AWS NODE POINT
const char MQTT_HOST[] = "a3ilsm9jwax8be-ats.iot.ap-south-1.amazonaws.com";  

// Amazon Root CA 1
//static const char AWS_CERT_CA[] PROGMEM = R"EOF(
static const char cacert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

// Device Certificate                                              
//static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
static const char client_cert[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAOrXWLbfLK6oDTLSJ4vootE5tgH+MA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMjEyMTcwNTEy
NTZaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCqC+oXRjYz5H0dYkpa
v/E3XOyVkUGUQlbYnaifP8kld43UXrMACM47WGbyujAhjUqufFumlZ0zD4tLe2al
cEutcNK80oObX+dMSka+U5XQpjb1/KoPxGj+GY86RiQoA+HVq+UOtZ9r2hgbMKEx
cdZmYVa6LXA5upPhcNgOXsjZ0ibWROPuQKJAlSUQfFFvMZcLXJjUBNKDwha+EsBZ
EBYuMKfE9ghtzQvzEAJ/etmlVkq48EIHMcLJyZXzigvpwb54+gKuhBo2daO//yPz
ClXT4S/c0kWTm+M1Cpsys/ULK71sTGhrZ/YuCsYCPDbbZrybGC6W8Oq8Aw4oDkXc
X3P5AgMBAAGjYDBeMB8GA1UdIwQYMBaAFAv61neG2on/8Twe8rwH7e/BFeO6MB0G
A1UdDgQWBBR9ltqRW1Ohvt46bMhOfqRXA5Nn7DAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAozXtMgFQ6ztL77DH2pqERGCy
gqqdRbK4PK0me4XVer/EEy0yMTlQP4Da/ZeV+5F5ZDAeaKKmuVLgQnnkCWHJq9vI
SU5t1ti4k9ScWob3sd6ydQPRRErtEtsv2vzi8exAhSTbWyegL2iS4Ai/0bCesWc4
NvgqWsrDXEnp3N3R8tq7xwA+QmqZO46h/QheDJ7jS4lRxn3gdC4DMlFqmvKAadP1
MCtWIUvIw6PN+Vr1zPUaFFQGzniARxn+DNeF+YNkEr9OOfVHgzb9grV4Pm75SIl2
JJWg8kwfNw53vEYkb4QxKVo1vlm1ysuV+rkc3uau3LUO7m4RJiqxYcgPfq9dqw==
-----END CERTIFICATE-----
)KEY";
 
 
// Device Private Key                                              
//static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
static const char privkey[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEAqgvqF0Y2M+R9HWJKWr/xN1zslZFBlEJW2J2onz/JJXeN1F6z
AAjOO1hm8rowIY1KrnxbppWdMw+LS3tmpXBLrXDSvNKDm1/nTEpGvlOV0KY29fyq
D8Ro/hmPOkYkKAPh1avlDrWfa9oYGzChMXHWZmFWui1wObqT4XDYDl7I2dIm1kTj
7kCiQJUlEHxRbzGXC1yY1ATSg8IWvhLAWRAWLjCnxPYIbc0L8xACf3rZpVZKuPBC
BzHCycmV84oL6cG+ePoCroQaNnWjv/8j8wpV0+Ev3NJFk5vjNQqbMrP1Cyu9bExo
a2f2LgrGAjw222a8mxgulvDqvAMOKA5F3F9z+QIDAQABAoIBAFHl4xzKiBh+F3Qr
frwubbLIYnm2t+r7+G/jnu7z/aC57+k0oumFVC5o6UxGOCAwuKNI6atVq5yIBAKa
sFlFuG0URasDiLcwfWnU7DWGdEVFiHr48b8eTaL0FHNwjMFo/T+3xl5LQzjilXGi
/j+eIDC2+No6QC7NmaOegbCKHhopv2D5ZsIvvBe5CFO8HGLDlLNirPGJYbksFfPc
TMWpSgePBGIKZU3H5fac76w5i2XydPWi8qbjW/BTNIZlZfxH5vtkSaJmQuUmYwKf
m0FIyER7GWhwtgKIYACDv9tBa5BJNLwLWozIQOmhdmLt7iz1bbroU4EcDFhrWhCy
uL8JlbECgYEA4Fs4B7Opbr1OQK09md0vuPuD4vloH0+pmVWxWGVolH8p7nrhLOTO
4GHLCloS1BHDZ1DKyZn7YCTDMOGhLT0cFmjkz8SLFcs8l4qIPjSOQolVaKO4sj3P
aF3PIh4C1m8cs8rr/zMq32gVAESvQXGJj9iH+owTKSfcFjOzx3sk0ksCgYEAwge+
taidHp/htH89KKwflrKlSX7xzSs5bPXG6XoTzPE7YK5/2059eYmnisZPE7FfegB+
rwL7H46dTuYphg/GTJUA8l1vSlYPbFvaxWyysfRzNEv/FT04hg74QAX8O4Bpcfx5
HF0Jn0Ym0xp2RdBZ/Wyj4EP3w0ta0r4720/aiEsCgYEAvuuhQJyZAI3D+7HxC/B3
3fjbANmwy4RlFQEhS9HwLv4P1stmt2d2zOcre4GvKBx9pnvQNcL9cZGXMQ+wsuif
8EoO/OeKXF95DpRq4FSlAuYPcl5wm+2RhITf8W6Mh6JCUJXMQn1ubHML/BB2+z/9
ChX/kSyGWa170sDoqkaWzQUCgYEAs7yC8DpqmaTXauF3IQcZF/sS29tJWJpSztl5
0mvwjadlLrFW5St3Vi+q0QXU2NQ1Monk0HA9ekUc+aqCCprwQZ7BysG8xyqjjOgl
5z5C5lAgq/A+MkmM47pYQs6IkgHpK9sJwbl5oV5VtrZLm25e/kA2KhgsGm/iiVgJ
kKYPNLMCgYEAq1o2MpPvjCkIvfB9rrWTR/ifhtHTjiF42cjsy7obKHqKHpJa09bb
4Svzi2V9ON4yMOZKb0i+toeBHxYotvdeV7sQPFwHptGsEOSJ5Eqw3CiNt+E+08PQ
eUz6zaITM+ebXX668tT2FtJShEihKPY+H7o6AqH/w19cBCvvN1DHnhw=
-----END RSA PRIVATE KEY-----
)KEY";

unsigned long lastMillis = 0;
unsigned long previousMillis = 0;
const long interval = 5000;
 
#define AWS_IOT_PUBLISH_TOPIC   "DoorDhwani/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "DoorDhwani/sub"
 
WiFiClientSecure net;
PubSubClient client(net);

BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);

time_t now;
time_t nowish = 1510592825;
 
 
void NTPConnect(void)
{
  Serial.print("Setting time using SNTP");
  configTime(TIME_ZONE * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while (now < nowish)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("done!");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
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


void messageReceived(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
 
 
void connectAWS()
{
  delay(3000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println(String("Attempting to connect to SSID: ") + String(WIFI_SSID));
 
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
 
  NTPConnect();
 
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);
 
  client.setServer(MQTT_HOST, 8883);
  client.setCallback(messageReceived);
 
 
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(1000);
  }
 
  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
 
  Serial.println("AWS IoT Connected!");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
 
int i;
void publishMessage()
{
  i += 1;
  if(i > 10000)
    i = 0;
  StaticJsonDocument<200> doc;
  doc["ID"] = "Bhupi_NodeMCU_1001";
  doc["time"] = millis();
  doc["variable"] = i;
  //doc["humidity"] = h;
  //doc["temperature"] = t;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  connectAWS();
  
//  if (!client.connected()) {
//    reconnect();
//  }
//  client.loop();
  
  read_and_write_signal();
 Send_FIle_MQTT();
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
  
  DS_signal = "";  // start of signal
  if (file.print(DS_signal)) {
    Serial.println("File was written with Starting character");
  } else {
    Serial.println("File write failed");
  }  

  int loopCounter = 0;
  while(loopCounter < SIGNAL_DURATION){
    loopCounter += 1;  
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
  
  file.close();

  Serial.println("Signal recording on a file completed..");
}

void read_and_write_signal_OLD(){
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
          if(ch != ',')
            msg[counter] = ch;
        }
        counter += 1;
      }
      StaticJsonDocument<200> doc;
      //doc["ID"] = "Bhupi_NodeMCU_1001";
      doc["time"] = millis();
      doc["variable"] = i;
      //doc["humidity"] = h;
      //doc["temperature"] = t;
      char jsonBuffer[512];
      serializeJson(doc, jsonBuffer); // print to client
     
      client.publish(n, jsonBuffer);
      //client.publish(AWS_IOT_PUBLISH_TOPIC, msg, 1);
      Serial.println("Published: ");
      Serial.println(msg);
      delay(1000);
  }
  Serial.println("reading and publishing ends");
  Serial.println(file.size());
  file.close();
}

void Send_FIle_MQTT_OLD(){
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
      client.publish(AWS_IOT_PUBLISH_TOPIC, msg, 1);
      Serial.println("Published: ");
      Serial.println(msg);
      delay(1000);
  }
  Serial.println("reading and publishing ends");
  Serial.println(file.size());
  file.close();
}
