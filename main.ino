#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <RCSwitch.h>
#include <Ticker.h>

// wifi settings
const char* ssid = "";
const char* password = "";

// mqtt settings
const char* mqttServer = "";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";

// amount of 433mhz devices for the mqtt emulation
int deviceCount = 2;
// definition of devices (mapping mqtt to 433mhz code)
char* devices[][3] = {
    // {"mqtt channel", "hauscode", "buchstabe"}
    // mqtt channel for the smart home device
    // hauscode: that 5 digit combination in your 433mhz remote
    // buchstabe: A-E on your 433mhz remote encoded as one hot:
    //    A : 10000
    //    B : 01000
    //    C : 00100
    //    D : 00010
    //    E : 00001
    // !!!!! Don't forget to update deviceCount !!!!!
    {"intern/gateway/tv",       "00111", "01000"},
    {"intern/gateway/coffee",   "00111", "00100"}
    };

// gloval vars
WiFiClient espClient;
PubSubClient client(espClient);
RCSwitch mySwitch = RCSwitch();
Ticker secondTick;
volatile int watchdogCount = 0;

// set up device after (re)boot
void setup() {
  // set up 433mhz transmitter
  mySwitch.enableTransmit(9);

  // set up wifi connection
  WiFi.begin(ssid, password);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(500);
  }
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
      Serial.println("OK");  
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
 
  // subscribe to required mqtt channels

  for(int i = 0; i < deviceCount; i++){
    client.subscribe(devices[i][0]);
  }

  // start the watchdog
  // restarts esp in case main loop does not finish after 10 seconds
  secondTick.attach(1, ISRWatchdog);
}

// watchdog to reset after crashes
void ISRWatchdog(){
  watchdogCount++;
  if(watchdogCount == 10){
    ESP.reset();
  }
}

// callback to handle incoming mqtt events
void callback(char* topic, byte* payload, unsigned int length) {
  String state = byteToString(payload, length);  

  // translate mqtt commands for 433mhz devices
  for(int i = 0; i < deviceCount; i++){
    if(String(topic).equals(devices[i][0]))switchDevice(state, devices[i][1], devices[i][2]);
  }
}

// switch a 433mhz device
void switchDevice(String state, const char* codeA, const char* codeB){
    if(state == "true"){
      mySwitch.switchOn(codeA, codeB);
    }
    else if(state == "false"){
      mySwitch.switchOff(codeA, codeB);
    }
}

// convert a byte array with a given length to a string
String byteToString(byte* payload, unsigned int length){
  String state = "";
  for (int i = 0; i < length; i++) {
    state += (char)payload[i];
  }
  return state;
}

// main loop
void loop() {
  // handle mqtt
  client.loop();

  // reset watchdog to prevent reset
  watchdogCount = 0;

  // handle wifi connection
  int wifi_retry = 0;
  while(WiFi.status() != WL_CONNECTED && wifi_retry < 5 ) {
      // reconnect wifi if not connected
      wifi_retry++;
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      delay(1000);
  }
  if(wifi_retry >= 5) {
      // if reconnect failed 5 times -> restart esp
      ESP.restart();
  }
}
