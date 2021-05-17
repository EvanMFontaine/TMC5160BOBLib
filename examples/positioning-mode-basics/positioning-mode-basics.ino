// This example shows how to you TMC5160 BOB with teensy 3.6 in a simple case : usage of positioning mode
// The wiring uses internal clock
// It is stongly recommended to refer the tmc5160 datasheet available here to understand what is done :
// https://www.trinamic.com/fileadmin/assets/Products/ICs_Documents/TMC5160_Datasheet_V1.01.pdf
// While testing programms and running them : never disconnect any wire of the motor while running.
// Each register is written till you rewrite it or power down the driver!
//Setting high currents will heat the board + the motor. Make your own tests
//for parameters limits, refer to datasheet
//note : parameters sets here are for very fast movement, nema 23 used.

#include <TMC5160BOBLib.h>
uint8_t CSpin = 10;
uint8_t ENpin = 25;
TMC5160Controller m = TMC5160Controller(CSpin, ENpin);
SPISettings settingsA(3000000, MSBFIRST, SPI_MODE3);//Speed can be changed, theorically, with int. clock, up to 4MHz
uint32_t amax, a1, d1, dmax, vmax, vstop, v1; //Ramp parameter, strongly recommended to refer p39 of the datasheet
int32_t pos;//actual position (not mesured with sensor, but return by algorithm of the tmc5160
char c;
int32_t xtarget;//position goal in tmc5160 units
float xrad = 2 * m.pi/4.; // position goal in units of the international system, here, 6.28 rad (one round)

void setup() {
  Serial.begin(250000);
  while (!Serial); //needed for the teensy
  m.enable();//enable driver
  m.TMC5160_INTERNAL_CLOCK = 12180000; // it is advised to you an external clock, internal clock (theorically 12MHz) is not precise, this value has been tested with one TMC5160BOB, you
  // may determinate it with your own. 12000000 remains a good value if high precision is not required.
  m.setCLOCKSPEED(m.TMC5160_INTERNAL_CLOCK);//Set the clock, is only used for conversion USI to TMC5160 units.
  SPI.begin();
  m.setSPISettings(settingsA);//Call this first
  m.beQuiet();//silence mode (sc2)
  m.setDefaultCHOPCONF();//see datasheet. The values are // TOFF=3, HSTRT=4, HEND=1, TBL=2, CHM=0 (spreadCycle™). Good values for most applications
  m.setStepRes(STEP64);//Step resolution. Other available are STEP128, STEP64, STEP32, STEP16, STEP8, STEP4, STEP2 and FULLSTEP
  m.setDefaultPWMCONF();//see datasheet. The values are TMC default reset (datasheet p. 47). Good values for most applications
  m.setIHOLD(2);//Hold current from 0 t0 31 (more than an amp in this case).
  m.setIRUN(15);//Running current. from 0 to 31.
  m.setIHOLDDELAY(5);//See datasheet
  m.setTPOWERDOWN(1);//See datasheet
  m.setTPWMTHRS(500);//see datasheet : image of time elapsed between 2 microsteps above witch stealthchop2 is activated. under, spreadcycle is activated.


  amax = 30000;//Parameters for ramp, see datasheet and their limits. All value outside of the limits is not send to tmc5160.
  a1 = 2 * amax;
  d1 = a1;
  dmax = amax;
  vmax = 800000;
  v1 = (uint32_t)vmax / 2;
  vstop = 10; //never set to 0!!
  m.setTZEROWAIT(1);
  m.setAMAX(amax);//Settings parameters
  m.setA1(a1);
  m.setD1(d1);
  m.setAMAX(amax);
  m.setDMAX(dmax);
  m.setVMAX(vmax);
  m.setV1(v1);
  m.setVSTOP(vstop);
  m.setVSTART(0);
  m.configRAMPMODE(POSMODE);//Configuring to positionning mode
  m.setXACTUAL(0);//Let's say we are at the 0 position
  m.setXTARGET(0);//target 0 : dont move
  //m.setVMAX(0);

}

void loop() {
  if (Serial.available()) {//type anything
    c = (char) Serial.read();//clean
    //m.setVMAX(consv);
    m.setXACTUAL(0);
    xtarget = m.XUSITo5160(xrad); //convert
    m.setXTARGET(xtarget);//move
    for (int i = 0; i < 200; i++) { //print the actual position!
      //vitesse=m.getVACTUAL();
      pos = m.getXACTUAL();
      Serial.println(pos);
      delay(5);
    }
  }

}
