/* LightIOT library by Juan David Velasquez Rosero
 */

#include "LightIOT.h"
#include "Particle.h"
#include "adafruit-ina219.h"

/**
 * IR led initial configuration
 */
#define TIMER_PWM_PIN        A5
#define TIMER_ENABLE_PWM     analogWrite(TIMER_PWM_PIN, 128, irout_khz*1000)
#define TIMER_DISABLE_PWM    analogWrite(TIMER_PWM_PIN, 0, irout_khz*1000)
#define TIMER_CONFIG_KHZ(val) ({  \
  pinMode(TIMER_PWM_PIN, OUTPUT); \
})

/**
 * INA219 Sensors INIT.
 */
Adafruit_INA219 batterySensor = Adafruit_INA219(INA219_BATTERY);
Adafruit_INA219 panelSensor = Adafruit_INA219(INA219_PANEL);
Adafruit_INA219 ledSensor = Adafruit_INA219(INA219_LED);

/**
 * Constructor.
 */
LightIOT::LightIOT(int argonNumber){
  _argonNumber = argonNumber;

  switch (_argonNumber){
    case 1:
      _p1 = 0.01069;
      _p2 = -0.02273;
      break;
    case 2:
      _p1 = 0.01062;
      _p2 = 0.05619;
      break;
    case 3:
      _p1 = 0.01067;
      _p2 = 0.003243;
      break;
    case 4:
      _p1 = 0.0108;
      _p2 = -0.04086;
      break;
    case 5:
      _p1 = 0.01073;
      _p2 = -0.06544;
      break;
    case 6:
      _p1 = 0.01068;
      _p2 = -0.03161;
      break;
    case 7:
      _p1 = 0.01065;
      _p2 = 0.01583;
      break;
    case 8:
      _p1 = 0.01078;
      _p2 = -0.0292;
      break;
    case 9:
      _p1 = 0.01078;
      _p2 = -0.03595;
      break;
    case 10:
      _p1 = 0.01076;
      _p2 = -0.02433;
      break;
    
    default:
        _p1 = 1;
        _p2 = 0;
      break;
  }
}

void LightIOT::begin(){
    batterySensor.begin();
    panelSensor.begin();
    ledSensor.begin();
}

// -------------  FUNCION PARA RECEPCION DE DATOS -----------------
void LightIOT::getCredentials(String credentials[],char inputBuffer[]){
    byte _sentencePos = 0;
    byte _comma_count = 0;
    credentials[_comma_count] = "";

    while (1){
        if (inputBuffer[_sentencePos] == NULL) break;
        if (inputBuffer[_sentencePos] == '&'){
            _comma_count++;
            credentials[_comma_count] = "";
            _sentencePos++;
        }
        else{
            credentials[_comma_count] += inputBuffer[_sentencePos];
            _sentencePos++;
        }
    }
}

//=============================================================
//================== IRSEND FUNCTION ==========================
//=============================================================

void LightIOT::sendCommand(uint8_t command){
    if (command==0){
        sendRaw(rawData_demo, 67, 38);  // Send DEMO command
        _modeDriver = "DEMO";
    }else if (command==1){
        sendRaw(rawData_off, 67, 38);  //  Send OFF command
        _modeDriver = "OFF";
    }else if (command==2){
        sendRaw(rawData_l, 67, 38);  // Send L command
        _modeDriver = "MODE L";
    }else if (command==3){
        sendRaw(rawData_t, 67, 38);  // Send T command
        _modeDriver = "MODE T";
    }else if (command==4){
        sendRaw(rawData_m, 67, 38);  // Send U command
        _modeDriver = "MODE U";
    }else{
        sendRaw(rawData_u, 67, 38);  // Send M command
        _modeDriver = "MODE M";
    }
        
}

void LightIOT::sendRaw(unsigned int buf[], int len, int khz)
{
  for(int i=0;i<5;i++){
      enableIROut(khz);
      for (int i = 0; i < len; i++) {
        if (i & 1) {
          space(buf[i]);
        }
        else {
          mark(buf[i]);
        }
      }
      space(0); // Just to be sure
      delay(100);
  }
}

void LightIOT::mark(int time) {
  // Sends an IR mark for the specified number of microseconds.
  // The mark output is modulated at the PWM frequency.
  TIMER_ENABLE_PWM; // Enable pin 3 PWM output
  if (time > 0) delayMicroseconds(time);
}

/* Leave pin off for time (given in microseconds) */

void LightIOT::space(int time) {
  // Sends an IR space for the specified number of microseconds.
  // A space is no output, so the PWM output is disabled.
  TIMER_DISABLE_PWM; // Disable pin 3 PWM output
  if (time > 0) delayMicroseconds(time);
}


void LightIOT::enableIROut(int khz){
    irout_khz = khz;
    pinMode(TIMER_PWM_PIN, OUTPUT);
    digitalWrite(TIMER_PWM_PIN, LOW);
    TIMER_CONFIG_KHZ(khz);
}

//=============================================================
//==================== SENSORS FUNCTION =======================
//=============================================================

// update sensor variables, add to average and countSamples
void LightIOT::measuring(float *vAverage, float *cAverage){
  
    float ledVoltage = _p1*((float)analogRead(A0))+_p2;
    float ledCurrent = ledSensor.getCurrent_mA();
    float panelVoltage = panelSensor.getBusVoltage_V();
    float panelCurrent = panelSensor.getCurrent_mA();
    float batteryVoltage = batterySensor.getBusVoltage_V();
    float batteryCurrent = batterySensor.getCurrent_mA();

    vAverage[0] += ledVoltage;
    cAverage[0] += ledCurrent;
    vAverage[1] += panelVoltage;
    cAverage[1] += panelCurrent;
    vAverage[2] += batteryVoltage;
    cAverage[2] += batteryCurrent;

    _countSamples++;
}

// set average variables (average=average/samples)
void LightIOT::getAverage(float *vAverage, float *cAverage){
    vAverage[0] = vAverage[0]/ (float) _countSamples;
    cAverage[0] = cAverage[0]/ (float) _countSamples;
    vAverage[1] = vAverage[1]/ (float) _countSamples;
    cAverage[1] = cAverage[1]/ (float) _countSamples;
    vAverage[2] = vAverage[2]/ (float) _countSamples;
    cAverage[2] = cAverage[2]/ (float) _countSamples;
}

// reset average variables
void LightIOT::resetAverage(float *vAverage, float *cAverage){
    vAverage[0] = 0;
    cAverage[0] = 0;
    vAverage[1] = 0;
    cAverage[1] = 0;
    vAverage[2] = 0;
    cAverage[2] = 0;
    _countSamples= 0;
}

void LightIOT::setModeDriver(String modeDriver){
  _modeDriver = modeDriver;
}

// Create json object with average variables
String LightIOT::sensorsToJSON(float *vAverage, float *cAverage){
    String dataJson = "{";

        	dataJson = dataJson + "\"id\":" + "\"e00fce68da5fd5cd09d82078\""+",";
            dataJson = dataJson + "\"name\":" + "\"Luminaria1\""+",";   //luminaria i
        	dataJson = dataJson + "\"mode\":" + "\""+ _modeDriver +"\"" +",";
        	dataJson = dataJson + "\"data\":" + "{";
        	
            	dataJson = dataJson+ "\"led\":" + "{";
            	dataJson = dataJson+ "\"voltage\":" + String(vAverage[0]) +",";
            	dataJson = dataJson+ "\"current\":" + String(cAverage[0]) +"}";
            	
            	dataJson = dataJson+ ",";
            	
            	dataJson = dataJson+ "\"pv\":" + "{";
            	dataJson = dataJson+ "\"voltage\":" + String(vAverage[1]) +",";
            	dataJson = dataJson+ "\"current\":" + String(cAverage[1]) +"}";
            	
            	dataJson = dataJson+ ",";
            	
            	dataJson = dataJson+ "\"battery\":" + "{";
            	dataJson = dataJson+ "\"voltage\":" + String(vAverage[2]) +",";
            	dataJson = dataJson+ "\"current\":" + String(cAverage[2]) +"}";
        	
        	dataJson = dataJson+ "}";
    	
    	dataJson = dataJson+ "}";

    return dataJson;
}

//=============================================================
//========================= FIX ERROR =========================
//=============================================================



void LightIOT::resetModeForError(float vPanel, float cPanel){
    if(vPanel>=16.0 && cPanel<150.0){
        _error = true;
    }

    if(_error) {
        unsigned int tActual = millis();
        if(tActual > _last_tError + _intervalOutError) {
            _last_tError = tActual;
            changeModeIR(_modeDriver);
            _error = false;
        }
    }
    
}

//=============================================================
//========================= CHANGE MODE =========================
//=============================================================
void LightIOT::changeModeIR(String currentMode){
    if(currentMode.equals("OFF")){
        sendCommand(1);  //  Send OFF command
    }else if(currentMode.equals("MODE L")){
        sendCommand(2);  // Send L command
    }else if(currentMode.equals("MODE T")){
        sendCommand(3);  // Send T command
    }else if(currentMode.equals("MODE U")){
        sendCommand(4);  // Send U command
    }else{
        sendCommand(5);  // Send M command
    }
}
