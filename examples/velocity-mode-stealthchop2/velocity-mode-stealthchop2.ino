// This example shows how to you TMC5160 BOB with teensy 3.6 in a simple case : usage of velocity mode in stealthchop mode
// The wiring uses internal clock
// It is stongly recommended to refer the tmc5160 datasheet available here to understand what is done :
// https://www.trinamic.com/fileadmin/assets/Products/ICs_Documents/TMC5160_Datasheet_V1.01.pdf
// While testing programms and running them : never disconnect any wire of the motor while running.
// Each register is written till you rewrite it or power down the driver!
//Setting high currents will heat the board + the motor. Make your own tests
//for parameters limits, refer to datasheet

#include <TMC5160BOBLib.h>
#define CSpin  10
#define ENpin  25

TMC5160Controller m = TMC5160Controller(CSpin, ENpin);

SPISettings settingsA(3000000, MSBFIRST, SPI_MODE3);//Speed can be changed, theorically, with int. clock, up to 4MHz 


uint32_t amax;
char c;
float VUSI = -2 * m.pi;//VUSI is the velocity target, in units of the international system; m.pi just stores the values of pi. 
int32_t consvsigned;// Velocity target in units of tmc5160

void setup() {
  Serial.begin(250000);
  while (!Serial);//needed for the teensy
  m.enable();//enable driver
  m.TMC5160_INTERNAL_CLOCK=12180000;// it is advised to you an external clock, internal clock (theorically 12MHz) is not precise, this value has been tested with one TMC5160BOB, you
  // may determinate it with your own. 12000000 remains a good value if high precision is not required.
  m.setCLOCKSPEED(m.TMC5160_INTERNAL_CLOCK);//Set the clock, is only used for conversion USI to TMC5160 units.
  SPI.begin();
  m.setSPISettings(settingsA);//Call this first
  m.beQuiet();//activate stealthchop
  m.setDefaultCHOPCONF();//see datasheet. The values are // TOFF=3, HSTRT=4, HEND=1, TBL=2, CHM=0 (spreadCycle™). Good values for most applications
  m.setStepRes(STEP256);//Step resolution. Other available are STEP128, STEP64, STEP32, STEP16, STEP8, STEP4, STEP2 and FULLSTEP
  m.setDefaultPWMCONF();//see datasheet. The values are TMC default reset (datasheet p. 47). Good values for most applications
  m.setIHOLD(2);//Hold current from 0 t0 31 (more than an amp in this case).
  m.setIRUN(15);//Running current. from 0 to 31.
  m.setIHOLDDELAY(5);//See datasheet
  m.setTPOWERDOWN(1);//See datasheet
  m.setTPWMTHRS(0); //time elapsed between 2 microsteps above witch stealthchop2 is activated. under, spreadcycle is activated. Set zero for using stealthchop2 at all velocity
  //warning : some current peaks can appear if sc2 is badly configured. Use sc2 for precise positionning at low speed, results are better.
  m.configRAMPMODE(VMODE_MINUS);// configuring the TMC5160 in velocity mode, negative value 
  amax = 60000;//amax
  m.setAMAX(amax);//between 0 and 2^16-1 . A value out of limits is not written to the register
  m.setVMAX(0);//Speed target

}

void loop() {
  if (Serial.available()) {//open serial monitor, type anything
    m.enable();//enable the driver
    c = (char) Serial.read();//clean
    consvsigned = m.VUSITo5160(VUSI);//convert
    if (consvsigned >= 0) {
      m.configRAMPMODE(VMODE_PLUS);//in case of positive value
    }
    else {
      m.configRAMPMODE(VMODE_MINUS);//or negative
      consvsigned = -consvsigned;//then, always send positive
    }
    m.setVMAX(consvsigned);//must be between 0 and 2^23-512.A value out of limits is not written to the register

    delay(1000);//wait
    Serial.println("STOP");//we want to stop the rotation now!
    m.setVMAX(0);//Stop it
    delay(1000);
    m.disable();//disable!
  }

}
