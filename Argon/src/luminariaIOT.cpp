/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "d:/Descargas/Luminarias/CodigoArgon/luminariaIOT/src/luminariaIOT.ino"
#include <LightIOT.h>
#include <MQTT.h>

void rutineConnectionMQTT();
void setup();
void loop();
bool connectMqttServer();
bool connectMqttConfig();
#line 4 "d:/Descargas/Luminarias/CodigoArgon/luminariaIOT/src/luminariaIOT.ino"
#define LIGHT_NUMBER 1

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

LightIOT lightController(LIGHT_NUMBER);
//=============================================================
//================== MQTT VARIABLES ===========================
//=============================================================
void callback(char* topic, byte* payload, unsigned int length);
MQTT client("209.182.218.175", 1883, callback);
//MQTT client("192.68.185.27", 1883, callback);

void callbackConfig(char* topic, byte* payload, unsigned int length);
MQTT clientConfig("192.168.1.6", 1883, callbackConfig);

//=============================================================
//============ VARIABLES - TIME LAPSE INTERVAL ================
//=============================================================
const int interval = 5*1000;
unsigned int last_t = 0;

//=============================================================
//================== MEASURING VARIABLES ======================
//=============================================================
float vAverage[3];
float cAverage[3];

//=============================================================
//================== EEPROM MEMORY ======================
//=============================================================
String ipServer="192.68.185.27";
#define DIR_IP_SERVER 0

//=============================================================
//=============== RECEIVE MESSAGES MAIN MQTT===================
//=============================================================
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;

    if (!strcmp(p, "DEMO"))
        lightController.sendCommand(0); //  Send DEMO command
    else if (!strcmp(p, "OFF"))
        lightController.sendCommand(1);  //  Send OFF command
    else if (!strcmp(p, "MODE L"))
        lightController.sendCommand(2);  // Send L command
    else if (!strcmp(p, "MODE T"))
        lightController.sendCommand(3);  // Send T command
    else if (!strcmp(p, "MODE U"))
        lightController.sendCommand(4);  // Send U command
    else
        lightController.sendCommand(5);  // Send M command
    
        
    Serial.print(topic);Serial.print("  ");
    Serial.println(p);
    delay(100);
}

//=============================================================
//=============== RECEIVE MESSAGES CONFIG MQTT=================
//=============================================================
void callbackConfig(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;

    
    if(strcmp(topic, "changeMQTT")==0){
        String myIpString(p);
        EEPROM.put(DIR_IP_SERVER, myIpString);
        client.setBroker(p, 1883);
    }

    if(strcmp(topic, "changeNetwork")==0){
        String credentials[2];
        lightController.getCredentials(credentials,p);

        WiFi.disconnect();

        WiFi.clearCredentials();
        WiFi.setCredentials("Luminarias");
        if(credentials[1].length()==0)  WiFi.setCredentials(credentials[0]);
        else WiFi.setCredentials(credentials[0],credentials[1]);
        //Activamos wifi e intentamos conexion
        WiFi.on();
        WiFi.connect();
    }

    Serial.print(topic);Serial.print("  ");
    Serial.println(p);
    delay(100);
}

//=============================================================
//==================== CONECCTION RUTINE ======================
//=============================================================
void rutineConnectionMQTT(){
    newConnectionAttempt :
    if(!connectMqttServer()) {
        if(!connectMqttConfig()){
            goto newConnectionAttempt;
        }else Serial.println("Connexion MQTT CONFIG exitosa");
    }else Serial.println("Connexion MQTT exitosa");
    delay(1000);
}

//=============================================================
//======================= VOID SETUP ==========================
//=============================================================
void setup() {    
    pinMode(D7,OUTPUT);
    Serial.begin(115200);

    Serial.println("Conectando con sensores INA219...!");
    lightController.begin();
    Serial.println("Midiendo VOLTAJE y CORRIENTE con INA219 ...");

    //WiFi.clearCredentials();
    WiFi.setCredentials("Luminarias");

    //Activamos wifi e intentamos conexion
        WiFi.on();
        WiFi.connect();
        waitUntil(WiFi.ready); 
    
    Serial.println(WiFi.SSID());
    digitalWrite(D7,HIGH);

    if(WiFi.ready()) rutineConnectionMQTT();
}



//=============================================================
//======================== VOID LOOP ==========================
//=============================================================
void loop() {

    // MQTT SERVER CONNECTION
    if(client.isConnected())   client.loop();
    else if(clientConfig.isConnected())   clientConfig.loop();
    else if(WiFi.ready()) rutineConnectionMQTT();
    
    lightController.measuring(vAverage,cAverage);

    // MQTT PUBLISH DATA
    unsigned int t = millis();
    if(t > last_t + interval) {
    	last_t = t;

        lightController.getAverage(vAverage,cAverage);
        lightController.resetModeForError(vAverage[0],cAverage[0]);
        Serial.println(vAverage[0]);
        String dataJson = lightController.sensorsToJSON(vAverage,cAverage);
        lightController.resetAverage(vAverage,cAverage);

    	if (client.isConnected()) {
            client.publish("newData",dataJson);
            digitalWrite(D7,!digitalRead(D7)); 

            //Power Signal 
            int rssi = WiFi.RSSI();
            Serial.print("RSSI: ");Serial.println(rssi);
        }
    }
}

//=============================================================
//=================== MAIN MQTT CONNECTION ====================
//=============================================================

bool connectMqttServer(){
    bool state = false;
    Serial.println("Conectando con server MQTT");
    
    
    String myIpString;
    EEPROM.get(DIR_IP_SERVER, myIpString);
    if(myIpString.length()==0) myIpString = "192.68.185.27";
    char p[myIpString.length()+1];
    myIpString.toCharArray(p, myIpString.length());
    client.setBroker(p, 1883);

    // connect to the server
    Serial.println(System.deviceID());
    client.connect("argonClient",System.deviceID(), "anything");
    
    String baseTopic = "changeMode";
    String topicDeviceID = baseTopic+ "/" + System.deviceID();
    String topicDeviceName = baseTopic + "/" + String("Luminaria") + String(LIGHT_NUMBER);

    // publish/subscribe
    if (client.isConnected()) {
        client.subscribe(baseTopic, MQTT::QOS2);
        client.subscribe(topicDeviceID, MQTT::QOS2);
        client.subscribe(topicDeviceName, MQTT::QOS2);
        state = true;
    }

    return state;
}

//=============================================================
//=================== CONFIG MQTT CONNECTION ==================
//=============================================================
bool connectMqttConfig(){
    bool state = false;
    Serial.println("Conectando con server MQTT Config");
    // connect to the server
    clientConfig.connect("argonClient",System.deviceID(), "anything");

    // publish/subscribe
    if (clientConfig.isConnected()) {
        clientConfig.subscribe("changeNetwork", MQTT::QOS2);
        clientConfig.subscribe("changeMQTT", MQTT::QOS2);
        state = true;
    }

    return state;
}