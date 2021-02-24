#pragma once

/* LightIOT library by Juan David Velasquez Rosero
 */

// This will load the definition for common Particle variable types
#include "Particle.h"

/*=======================================================================
    INA SENSORS ADDRESS
    -----------------------------------------------------------------------*/
    #define INA219_BATTERY                         (0x44)
    #define INA219_PANEL                         (0x41)
    #define INA219_LED                         (0x40)

// This is your main class that users will import into their application
class LightIOT
{
public:
  /**
   * Constructor
   */
  LightIOT(int argonNumber);

    //=============================================================
    //================ BASIC FUNCTIONS LIGHT ======================
    //=============================================================
    void begin();
    void getCredentials(String credentials[],char inputBuffer[]);
    void measuring(float *vAverage, float *cAverage);
    void getAverage(float *vAverage, float *cAverage);
    void resetAverage(float *vAverage, float *cAverage);
    String sensorsToJSON(float *vAverage, float *cAverage);

    void resetModeForError(float vPanel, float cPanel);

    void setModeDriver(String modeDriver);
    void changeModeIR(String currentMode);
    
    //=============================================================
    //================ IR COMMAND LED LAMP ========================
    //=============================================================
    void sendCommand(uint8_t command);
    void sendRaw(unsigned int buf[], int len, int khz);
    void mark(int time);
    void space(int time);
    void enableIROut(int khz);
    
    unsigned int  rawData_demo[67] = {9400,4500, 600,1700, 600,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,1700, 550,1700, 600,1700, 600,1650, 600,1700, 600,1700, 550,1700, 600,550, 600,550, 600,1700, 550,1700, 600,550, 600,550, 600,550, 600,550, 600,1700, 550,1700, 600,550, 600,550, 600,1700, 550,1700, 600,1700, 600,1650, 600};  // DEMO // NEC 807F30CF
    unsigned int  rawData_off[67] = {9450,4550, 550,1700, 600,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,600, 550,1700, 600,1650, 600,1700, 600,1650, 600,1700, 600,1700, 550,1700, 600,550, 600,550, 600,550, 600,1650, 650,550, 550,600, 550,550, 650,550, 550,1700, 600,1700, 550,1700, 600,550, 600,1700, 550,1700, 600,1700, 600,1650, 600};  // OFF  // NEC 807F10EF
    unsigned int  rawData_l[67] = {9450,4500, 600,1700, 600,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,1700, 550,1700, 600,1700, 600,1650, 600,1700, 600,1700, 550,1700, 600,1650, 650,550, 550,600, 550,1700, 600,550, 600,550, 600,550, 600,550, 600,550, 600,1700, 600,1650, 600,550, 600,1700, 550,1700, 600,1700, 600,1650, 600};  // L    // NEC 807F906F
    unsigned int  rawData_t[67] = {9600,4500, 600,1700, 600,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,600, 550,1700, 600,1700, 600,1650, 600,1700, 550,1700, 600,1700, 600,1650, 600,600, 550,550, 650,550, 550,600, 550,1700, 600,550, 600,550, 600,550, 600,1700, 600,1650, 600,1700, 550,1700, 600,550, 600,1700, 600,1700, 550,1700, 600};  // T    // NEC 807F08F7
    unsigned int  rawData_u[67] = {9450,4500, 600,1650, 650,550, 550,600, 550,600, 600,550, 550,600, 600,550, 600,550, 600,550, 600,1650, 600,1700, 600,1700, 550,1700, 600,1700, 600,1650, 600,1700, 550,600, 600,550, 600,1650, 600,550, 600,1700, 600,550, 600,550, 600,550, 600,1650, 600,1700, 600,550, 600,1650, 600,550, 600,1700, 600,1700, 550,1700, 600};  // U    // NEC 807F28D7
    unsigned int  rawData_m[67] = {9550,4500, 600,1700, 600,550, 600,550, 600,550, 600,550, 600,550, 600,600, 550,550, 600,550, 600,1700, 600,1700, 550,1700, 600,1700, 600,1650, 600,1700, 600,1650, 650,1650, 600,550, 600,550, 600,600, 550,550, 600,550, 600,600, 550,550, 650,550, 550,1700, 600,1700, 600,1700, 550,1700, 600,1700, 600,1700, 550,1700, 600};  // M    // NEC 807F807F
    int irout_khz;

private:
  int _argonNumber = 0;
  int _p1 = 1;
  int _p2 = 0;
  String _modeDriver = "MODE L";
  unsigned int _last_tError = 0;
  const int _intervalOutError = 5*60*1000;
  bool _error = false;
  int _countSamples = 0;
};
