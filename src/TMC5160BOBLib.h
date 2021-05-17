#ifndef TMC5160BOBLib_H_INCLUDED
#define TMC5160BOBLib_H_INCLUDED
#include <Arduino.h>
#include <TMC5160_registers.h>
#include <SPI.h>
#define writeAccess 0x80
const uint32_t RAMPMODE_ADR=writeAccess+TMCRhino_RAMPMODE;
const uint32_t GCONF_ADR=writeAccess+TMCRhino_GCONF;
const uint32_t CHOPCONF_ADR=writeAccess+TMCRhino_CHOPCONF;
const uint32_t IHOLD_IRUN_ADR=writeAccess+TMCRhino_IHOLD_IRUN;
const uint32_t TPOWERDOWN_ADR=writeAccess+TMCRhino_TPOWERDOWN;
const uint32_t PWMCONF_ADR=writeAccess+TMCRhino_PWMCONF;
const uint32_t VSTART_ADR=writeAccess+TMCRhino_VSTART;
const uint32_t A1_ADR=writeAccess+TMCRhino_A1 ;
const uint32_t V1_ADR=writeAccess+TMCRhino_V1;
const uint32_t AMAX_ADR=writeAccess+TMCRhino_AMAX;
const uint32_t VMAX_ADR=writeAccess+TMCRhino_VMAX;
const uint32_t DMAX_ADR=writeAccess+TMCRhino_DMAX;
const uint32_t D1_ADR=writeAccess+TMCRhino_D1;
const uint32_t VSTOP_ADR=writeAccess+TMCRhino_VSTOP;
const uint32_t TZEROWAIT_ADR=writeAccess+TMCRhino_TZEROWAIT;
const uint32_t XTARGET_ADR=writeAccess+TMCRhino_XTARGET;
const uint32_t XACTUAL_ADR=writeAccess+TMCRhino_XACTUAL;
const uint32_t TPWMTHRS_ADR=writeAccess+TMCRhino_TPWMTHRS;
#define STEP256 0x00000000
#define STEP128 0x00000001
#define STEP64 0x00000002
#define STEP32 0x00000003
#define STEP16 0x00000004
#define STEP8 0x00000005
#define STEP4 0x00000006
#define STEP2 0x00000007
#define FULLSTEP 0x00000008
#define POSMODE 0x00000000
#define VMODE_PLUS 0x00000001
#define VMODE_MINUS 0x00000002
#define HOLDMODE 0x00000003

class TMC5160Controller
{
	public:
	 TMC5160Controller(uint8_t chipCS,uint8_t pinEN);
	 void enable();
	 void disable();
	 //void affiche();
	 //void atry(uint32_t *titi);
	 void sendData(uint32_t address, uint32_t datagram, uint32_t *incommingDatagram);
	 void setSPISettings(SPISettings mySettings);
	 void beQuiet();
	 void dontBeQuiet();
	 void configCHOPCONF(uint32_t datagram);
	 void setDefaultCHOPCONF();
	 void setDefaultPWMCONF();
	 void setStepRes(uint32_t stepRes);
	 void setIHOLD(uint32_t iholdval);
	 void setIRUN(uint32_t irunval);
	 void setIHOLDDELAY(uint32_t iholddelayval);
	 void setTPOWERDOWN(uint32_t tpowerdownval);
	 void configPWMCONF(uint32_t datagram);
	 int32_t getVACTUAL();
	 int32_t getXACTUAL();
	 void configRAMPMODE(uint32_t datagram);
	 void setVSTART(uint32_t vstartval);
	 void setA1(uint32_t a1val);
	 void setV1(uint32_t v1val);
	 void setAMAX(uint32_t amaxval);
	 void setVMAX(uint32_t vmaxval);
	 void setDMAX(uint32_t dmaxval);
	 void setD1(uint32_t d1val);
	 void setVSTOP(uint32_t vstopval);
	 void setTZEROWAIT(uint32_t tzerowaitval);
	 void setXTARGET(int32_t xtargetval);
	 void setXACTUAL(int32_t xactualval);
	 void setCLOCKSPEED(uint32_t clockspeedval);
	 int32_t XUSITo5160(float XRad);
	 int32_t VUSITo5160(float VUSI);
	 void setMOTORSTEPS(uint32_t motorstepval);
	 void setTPWMTHRS(uint32_t tpwmthrsval);


	 uint32_t TMC5160_INTERNAL_CLOCK;
	 uint32_t TMC5160_EXTERNAL_CLOCK;
	 uint32_t MOTORSTEPS;
	 float pi;


	 



	private:
		uint8_t _chipCS;
		uint8_t _pinEN;
		SPISettings _mySettings;
		uint32_t _trashDatagram;
		uint32_t _currentCHOPCONFDatagram;
		uint32_t _defaultCHOPCONFDatagram;
		uint32_t _currentIHOLD_IRUNDatagram;
		uint32_t _currentPWMCONFDatagram;
		uint32_t _defaultPWMCONFDatagram;
		uint32_t _zeroDatagram;
		int32_t _conv24(uint32_t valToConv);
		uint32_t _actualClockSpeed;
		uint32_t _actualStepRes;
		


};

/*void write_i(int var);
void write_f(float var);
void write_s(short var);
void write_b(byte var);
void write_l(long var);
void write_ul(unsigned long var);

void read_i(int *var);
void read_f(float *var);
void read_s(short *var);
void read_b(byte *var);
void read_l(long *var);
void read_ul(unsigned long *var);*/




#endif // WRITE_FUNC_H_INCLUDED