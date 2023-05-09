#include <PubSubClient.h>
#include <DHT.h>
#include <string.h>
#include "SPIFFS.h"
#include <WiFiClientSecure.h>
#include <Wire.h>

const char* ssid = "Redwood"; 
const char* password = "Passwords"; 
const char* mqtt_server = "http://apsm7xvjsvch0-ats.iot.ap-south-1.amazonaws.com"; 
const int mqtt_port = 8883;

const char* cert1 = {"-----BEGIN CERTIFICATE-----\n\
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n\
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n\
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n\
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n\
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n\
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n\
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n\
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n\
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n\
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n\
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n\
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n\
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n\
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n\
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n\
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n\
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n\
rqXRfboQnoZsG4q5WTP468SQvvG5\n\
-----END CERTIFICATE-----"};

const char* cert2 = {"-----BEGIN CERTIFICATE-----\n\
MIIDWTCCAkGgAwIBAgIUbt0a8Pn2aV+j9MeMbA1N0R2uvj8wDQYJKoZIhvcNAQEL\n\
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n\
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIzMDUwODEyMjc0\n\
M1oXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n\
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKT9hwGShdx6WKslD+G0\n\
9RxPNC2rpCV+GnZsM2R3BbzHLOq/4zUkpxTf8uZ1XeQ16rTO9hpDS8qYnGryWxTB\n\
XVl98NcA2gM1j4Q9xscpWvygUvrUIqUiMrFF+1eAWcsaPEpSaZQ7klMKYslgcpc/\n\
jfu4YuhVqtcveqY8iPeVK4KLX4VPsZLeqhKthQLfDgU7QrcDuT5Q78gKibs2zJG7\n\
fIycQuUL3FJ8R5W9mbiMIFR2Sg82vQsQQUb3EbhAyHGycdmQcvywKObhcoLWCkch\n\
xDhdvRCSFpFX5Tk1DNRkRnx8uewE62d0epKaXZaaRn+RI+Mtiqt0RSSe+7tnIhhO\n\
0B0CAwEAAaNgMF4wHwYDVR0jBBgwFoAUXhGEuWXO29d1Cb2MK80GSKWWOr0wHQYD\n\
VR0OBBYEFCA2ZhutB7ZDRcOij0ubTmAurBF+MAwGA1UdEwEB/wQCMAAwDgYDVR0P\n\
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBB6iDnTwRSjFTVN6sKc1ft8jqt\n\
/MnCZwdVeQlI5hUkQELkJwowFyTYxlpxBRCGaR6g0YHYcxdlYLshlvuwtuLkSKtf\n\
jwzBm4rIvtV8NU1at7z0D3ycrt74Mn+PDbAay6Gy0U/Vf6k/7DkTfdCpE0mdICXE\n\
2ZdWuPqEn90G34xFPb5f7UyZ+0ckBZ6pREbD/GgUyB0CTOg5JrOKN1BWAwHbWdHa\n\
KHA32qQBe9pJvwGNhXgNtQdwbTpwpOLXj/Ij+u8MoVlbX3sWD3tzaFK6tD3AaHkU\n\
boTYD7yT89tgDaAmiVJgekK86BIwCwTSX8EQJwL7g550wEymPUogaxHU/REy\n\
-----END CERTIFICATE-----"};

const char* cert3 = {"-----BEGIN RSA PRIVATE KEY-----\n\
MIIEpQIBAAKCAQEApP2HAZKF3HpYqyUP4bT1HE80LaukJX4admwzZHcFvMcs6r/j\n\
NSSnFN/y5nVd5DXqtM72GkNLypicavJbFMFdWX3w1wDaAzWPhD3Gxyla/KBS+tQi\n\
pSIysUX7V4BZyxo8SlJplDuSUwpiyWBylz+N+7hi6FWq1y96pjyI95UrgotfhU+x\n\
kt6qEq2FAt8OBTtCtwO5PlDvyAqJuzbMkbt8jJxC5QvcUnxHlb2ZuIwgVHZKDza9\n\
CxBBRvcRuEDIcbJx2ZBy/LAo5uFygtYKRyHEOF29EJIWkVflOTUM1GRGfHy57ATr\n\
Z3R6kppdlppGf5Ej4y2Kq3RFJJ77u2ciGE7QHQIDAQABAoIBAQCFOCSrk1i8tpkj\n\
z7AsKgYEDrvt6HqIv3K0jhdNFoGudRCCl9/oiCjUxSMHDFt8qkBSMA4tFIt9EY6z\n\
F++NKbirSvKNWCWwk63HNG5tX6oU84ss9t7mgV93Hi+c7okSYqOtyy9uvk8e9/Ld\n\
8R91mp6VUsOiH5HrYG3jDlv1tR/FrmGDIHsQHBEI168koi2HqvUn3aHpoXl4i3/+\n\
a9QO5NJs2Mo+YiVU7MfKLPPrh3BIC78wPBMgancOst6yDFmBfJ1UGHO2vtOssn8x\n\
SDk7y0Knt+i6uTL4tn2oT4Xu7icRuxij4vfN9ugWBer5b/60VSRhjvYo6tFO3q59\n\
wi6PEqrhAoGBANbFJHd93ca8xz+DDeR+DkAhrYXo4KZNy/XdNPtA6Tc5WioBLi9i\n\
792gHih30aFLL5Sq8CVZrp8uX5LQ/CLQrYkw7cPPO3zm1S/6BKu0yChEO8EfWt+B\n\
qS1CbOAWEcmHph1WHkrhqsJuu+uvmevQFRa2Jf3ZFgBacXM1UlMJYY+lAoGBAMSp\n\
9qsiBq7XZez3dozS6xito0lanjTjTS4iXZZ0ulPAx4DECBM4YqVhxvJ2Xi7UHmO2\n\
Ai4Si4akFZDiwRnD4SicTNz+vVt1YxVzz21Os4CbGcM7HOYsnC7vbXriuxcbhTHz\n\
nXjLkvmTeKdOAdkyQAafRsfx1UqIoE65j7/Y3lUZAoGBAMroNJzECKyDnAcwLQbr\n\
5VDHwKYcL2pPY9wkorKiT/QBtmKq/NR+N7p+VR/Mm/w0CX7+xKW1BhqT+G3ADgZi\n\
eyF7XZc6iaxmdZG/N3x17wjD9mRt2DAD0y7UbAKGN0XcZ5+dxj7e17KcZeS04cn9\n\
rH7plhkvaO7qg/bhsKaM3WSVAoGAIJX07DgwGUpRdikuuHi+QZGLy7y7GBJNmIZT\n\
MXnX4cbAh8JzgBPIrJR97XDVfDbjbiwbQl0KR13qwN4Blu5Q4XS8e6Wt5HiX2Vce\n\
GIOA1J9ebm3eBPmBS3maj1EAJP0HXdZ60igRC093H2mafs2HtFEx3jFg4P+waDrw\n\
6Z3agQkCgYEApKDuhhkY3kuVXbdws3KHOixG8yTYmkhiyYP2gx8myu+vg/QCihkF\n\
b/tCA3pNLWW69s1dVZmlIbSj8p6EHmFy9kaAvYP4KW4edXfE3m0IdEo1AhPnbi4D\n\
Jdn1VGN7Yi95Gdzg7PN+7gD98D0OAIl4JvD0cbFBpUBJufNKj7iXBNc=\n\
-----END RSA PRIVATE KEY-----"};

String Read_rootca;
String Read_cert;
String Read_privatekey;
//=============================================================================================================================
#define BUFFER_LEN  256
long lastMsg = 0;
char msg[BUFFER_LEN];
int value = 0;
byte mac[6];
char mac_Id[18];
int count = 1;
//=============================================================================================================================

WiFiClientSecure espClient;
PubSubClient client(espClient);


#define DHTPIN 15         
DHT dht(DHTPIN, DHT11);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

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
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("ei_out", "hello world");
      // ... and resubscribe
      client.subscribe("ei_in");
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
  Serial.begin(115200);
  dht.begin();
  Serial.setDebugOutput(true);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(2, OUTPUT);
  setup_wifi();
  delay(1000);
  // //=============================================================
  // if (!SPIFFS.begin(true)) {
  //   Serial.println("An Error has occurred while mounting SPIFFS");
  //   return;
  // }
  // //=======================================
  // //Root CA File Reading.

  // File file2 = SPIFFS.open(".AmazonRootCA1.pem", "r");
  // if (!file2) {
  //   Serial.println("Failed to open file for reading");
  //   return;
  // }
  // Serial.println("Root CA File Content:");
  
  // while (file2.available()) {
  //   Read_rootca = file2.readString();
  //   Serial.println(Read_rootca);
  // }
  // //=============================================
  // // Cert file reading
  // File file4 = SPIFFS.open("/56c611bbbd94e396beb99510158645cbae025502df8ab8c7e3fc18b2bcc05f83-certificate.pem.crt", "r");
  // if (!file4) {
  //   Serial.println("Failed to open file for reading");
  //   return;
  // }
  // Serial.println("Cert File Content:");
  // while (file4.available()) {
  //   Read_cert = file4.readString();
  //   Serial.println(Read_cert);
  // }
  // //=================================================
  // //Privatekey file reading
  // File file6 = SPIFFS.open(".345634565634563456-private.pem.key", "r");
  // if (!file6) {
  //   Serial.println("Failed to open file for reading");
  //   return;
  // }
  // Serial.println("privateKey File Content:");
  // Serial.println(file6.readString());
  // while (file6.available()) {
  //   Read_privatekey = file6.readString();
  //   Serial.println(Read_privatekey);
  // }
  //=====================================================

  int str1 = 0;
  while(cert1[str1] != '\0') str1++;
  char* pRead_rootca = (char*) malloc(sizeof(char) * (str1 + 1));
  strcpy(pRead_rootca, cert1);

  str1 = 0;
  while(cert2[str1] != '\0') str1++;
  char* pRead_cert = (char *)malloc(sizeof(char) * (str1 + 1));
  strcpy(pRead_cert, cert2);

  str1 = 0;
  while(cert3[str1] != '\0') str1++;
  char* pRead_privatekey = (char *)malloc(sizeof(char) * (str1 + 1));
  strcpy(pRead_privatekey, cert3);

  Serial.println("================================================================================================");
  Serial.println("Certificates that passing to espClient Method");
  Serial.println();
  Serial.println("Root CA:");
  Serial.write(pRead_rootca);
  Serial.println("================================================================================================");
  Serial.println();
  Serial.println("Cert:");
  Serial.write(pRead_cert);
  Serial.println("================================================================================================");
  Serial.println();
  Serial.println("privateKey:");
  Serial.write(pRead_privatekey);
  Serial.println("================================================================================================");

  espClient.setCACert(pRead_rootca);
  espClient.setCertificate(pRead_cert);
  espClient.setPrivateKey(pRead_privatekey);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  //====================================================================================================================
  WiFi.macAddress(mac);
  snprintf(mac_Id, sizeof(mac_Id), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println(mac_Id);
  //=====================================================================================================================
}


void loop() {
  float h = dht.readHumidity();   // Reading Temperature form DHT sensor
  float t = dht.readTemperature();      // Reading Humidity form DHT sensor
  float tF = (t * 1.8) + 32;
  Serial.println(tF);
  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    delay(1000);
    return;
  }
  //===============================================
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    //=============================================================================================
    String macIdStr = mac_Id;
    String Temprature = String(t);
    String Humidity = String(h);
    snprintf (msg, BUFFER_LEN, "{\"macid\" : \"%s\", \"temprature\" : %s, \"humidity\" : \"%s\"}", macIdStr.c_str(), Temprature.c_str(), Humidity.c_str());
    Serial.print("Publish message: ");
    Serial.print(count);
    Serial.println(msg);
    client.publish("ei_out", msg);
    count = count + 1;
    //================================================================================================
  }
  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}