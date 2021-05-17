#include <TMC5160BOBLib.h>
TMC5160Controller::TMC5160Controller(uint8_t chipCS,uint8_t pinEN)
{
	_chipCS=chipCS;
	_pinEN=pinEN;
	TMC5160_INTERNAL_CLOCK=12180000;
	TMC5160_EXTERNAL_CLOCK=16000000;
	pinMode(_chipCS,OUTPUT);
	pinMode(_pinEN,OUTPUT);
	digitalWrite(_chipCS,HIGH);
	digitalWrite(_pinEN,HIGH);//Logique inversée, cf TMC56130 Datasheet.
	_trashDatagram=0;
	_currentCHOPCONFDatagram=0x000100C3;// TOFF=3, HSTRT=4, HEND=1, TBL=2, CHM=0 (spreadCycle™)
	_defaultCHOPCONFDatagram=0x000100C3;
	_currentIHOLD_IRUNDatagram=0x00070F09;//IHOLD=9, IRUN=15, IHOLDDELAY=7
	_currentPWMCONFDatagram=0xC40C001E;//TMC default reset (datasheet p. 47)
	_defaultPWMCONFDatagram=0xC40C001E;
	_zeroDatagram=0x00000000;
	_actualClockSpeed=TMC5160_INTERNAL_CLOCK;
	_actualStepRes=STEP256;
	pi=3.141592653589793;
	MOTORSTEPS=200;
}



int32_t TMC5160Controller::XUSITo5160(float XUSI){
	int32_t res=int32_t((float)(MOTORSTEPS*_actualStepRes)/(2.*pi)*XUSI);
	return res;
}
/*
int32_t TMC5160Controller::AUSITo5160(float AUSI){
	int32_t AUSTEPS2=

}*/
int32_t TMC5160Controller::VUSITo5160(float VUSI){
	float VUSTEPS=((float)(MOTORSTEPS*_actualStepRes)/(2.*pi)*VUSI);
	int32_t V5160=(int32_t)(VUSTEPS*16777216./((float) _actualClockSpeed));
	return V5160;

}



void TMC5160Controller::setMOTORSTEPS(uint32_t motorstepval){
	MOTORSTEPS=motorstepval;
}

void TMC5160Controller::setCLOCKSPEED(uint32_t clockspeedval){
	_actualClockSpeed=clockspeedval;
}

void TMC5160Controller::enable(){
	digitalWrite(_pinEN,LOW);
}

void TMC5160Controller::disable(){
	digitalWrite(_pinEN,HIGH);
}



void TMC5160Controller::setSPISettings(SPISettings mySettings){
	_mySettings=mySettings;
}

/*void TMC5160Controller::affiche()
{
	Serial.println(POSMODE);

}*/

/*
void TMC5160Controller::atry(uint32_t *titi)
{
	uint8_t *p_titi = (uint8_t *)titi;
	for(uint8_t i=0;i<4;i++){
		*(p_titi+i)=1;
	}

}*/

void TMC5160Controller::sendData(uint32_t address, uint32_t datagram, uint32_t *incommingDatagram){

	switch (address){
		case CHOPCONF_ADR:
			_currentCHOPCONFDatagram=datagram;

			break;
		case IHOLD_IRUN_ADR:
			_currentIHOLD_IRUNDatagram=datagram;
			break;
		case PWMCONF_ADR:
			_currentPWMCONFDatagram=datagram;
		default:
			break;
	}

	uint8_t *p_incommingDatagram = (uint8_t *) incommingDatagram;
	uint8_t trash=0;
	SPI.beginTransaction(_mySettings);
	digitalWrite(_chipCS,LOW);
	delayMicroseconds(10);//Peut être réductible??
	trash = SPI.transfer(address);
	for(uint8_t i=0; i<4;i++)
	{
		*(p_incommingDatagram+3-i)=SPI.transfer((datagram >> (24-8*i)) & 0xff);
	}
	digitalWrite(_chipCS,HIGH); 
	SPI.endTransaction();
	

}





void TMC5160Controller::beQuiet(){
	
	TMC5160Controller::sendData(GCONF_ADR,0x00000004,&_trashDatagram);
}

void TMC5160Controller::dontBeQuiet(){
	TMC5160Controller::sendData(GCONF_ADR,0x00000000,&_trashDatagram);	
}

void TMC5160Controller::configCHOPCONF(uint32_t datagram){
	_currentCHOPCONFDatagram=datagram;
	TMC5160Controller::sendData(CHOPCONF_ADR,_currentCHOPCONFDatagram,&_trashDatagram);
}

void TMC5160Controller::setDefaultCHOPCONF(){
	TMC5160Controller::sendData(CHOPCONF_ADR,_defaultCHOPCONFDatagram,&_trashDatagram);
}

void TMC5160Controller::setStepRes(uint32_t stepRes){
	if ((stepRes>=0) && (stepRes<=8)){
	_currentCHOPCONFDatagram &= 0xf0ffffff;//Mise à 0 de l'octet Mres en conservant les autres
	_currentCHOPCONFDatagram |= (stepRes<<24);
	switch (stepRes){
		case FULLSTEP:
			_actualStepRes=1;
			break;
		case STEP128:
			_actualStepRes=128;
			break;
		case STEP64:
			_actualStepRes=64;
			break;
		case STEP32:
			_actualStepRes=32;
			break;
		case STEP16:
			_actualStepRes=16;
			break;
		case STEP8:
			_actualStepRes=8;
			break;
		case STEP4:
			_actualStepRes=4;
			break;
		case STEP2:
			_actualStepRes=2;
			break;
		case STEP256://si STEP256 ou autre qui n'est pas dans la liste
		default:
			_actualStepRes=256;
			break;
	}
	TMC5160Controller::sendData(CHOPCONF_ADR,_currentCHOPCONFDatagram,&_trashDatagram);
	}
}

void TMC5160Controller::setIHOLD(uint32_t iholdval){
	if((iholdval>=0) && (iholdval<=31)){
		_currentIHOLD_IRUNDatagram &=0x000fff00;
		_currentIHOLD_IRUNDatagram |= iholdval;
		TMC5160Controller::sendData(IHOLD_IRUN_ADR,_currentIHOLD_IRUNDatagram,&_trashDatagram);
	}
}

void TMC5160Controller::setIRUN(uint32_t irunval){
	if((irunval>=0) && (irunval<=31)){
		_currentIHOLD_IRUNDatagram &=0x000f00ff;
		_currentIHOLD_IRUNDatagram |= (irunval<<8);
		TMC5160Controller::sendData(IHOLD_IRUN_ADR,_currentIHOLD_IRUNDatagram,&_trashDatagram);
	}
}

void TMC5160Controller::setIHOLDDELAY(uint32_t iholddelayval){
	if((iholddelayval>=0) && (iholddelayval<=15)){
		_currentIHOLD_IRUNDatagram &=0x0000ffff;
		_currentIHOLD_IRUNDatagram |= (iholddelayval<<16);
		TMC5160Controller::sendData(IHOLD_IRUN_ADR,_currentIHOLD_IRUNDatagram,&_trashDatagram);
	}
}

void TMC5160Controller::setTPOWERDOWN(uint32_t tpowerdownval){
	if((tpowerdownval>=0) && (tpowerdownval<=8)){
		TMC5160Controller::sendData(TPOWERDOWN_ADR,tpowerdownval,&_trashDatagram);

	}
}

void TMC5160Controller::configPWMCONF(uint32_t datagram){
	_currentPWMCONFDatagram=datagram;
	TMC5160Controller::sendData(PWMCONF_ADR,_currentPWMCONFDatagram,&_trashDatagram);

}

void TMC5160Controller::setDefaultPWMCONF(){
	TMC5160Controller::sendData(PWMCONF_ADR,_defaultPWMCONFDatagram,&_trashDatagram);
}

int32_t TMC5160Controller::_conv24(uint32_t valToConv){
	valToConv &= 0x00FFFFFF; //Par sécurité, mais normalement non nécessaire
  	int32_t convedVal=0;
  if (valToConv >> 23 == 0x1) {//Le 24ème bit vaut 1 si valToConv est négative car vactual est codé sur 24bit, c'est un "int24_"t
    valToConv = ((~valToConv) & 0x00ffffff) + 1;//Complémentation sur 24 bits uniquement puis ajout de 1
    convedVal = - (int32_t) valToConv;
  }
  else {
    convedVal = (int32_t) valToConv;
  }
  return convedVal;

}

int32_t TMC5160Controller::getVACTUAL(){
	uint32_t tempIDatagram=0;
	TMC5160Controller::sendData(TMCRhino_VACTUAL,_zeroDatagram,&_trashDatagram);
	TMC5160Controller::sendData(TMCRhino_VACTUAL,_zeroDatagram,&tempIDatagram);//La donnée voulue ne vient qu'au deuxième appel... cf page 22 datasheet TMC5160
	int32_t vactualval=TMC5160Controller::_conv24(tempIDatagram);
	return vactualval;

}


void TMC5160Controller::configRAMPMODE(uint32_t datagram){
	TMC5160Controller::sendData(RAMPMODE_ADR,datagram,&_trashDatagram);

}

int32_t TMC5160Controller::getXACTUAL(){
	uint32_t tempIDatagram=0;
	TMC5160Controller::sendData(TMCRhino_XACTUAL, _zeroDatagram ,&tempIDatagram);
	TMC5160Controller::sendData(TMCRhino_XACTUAL, _zeroDatagram ,&tempIDatagram);
	int32_t xactualval=(int32_t) tempIDatagram;
	return xactualval;
}

void TMC5160Controller::setVSTART(uint32_t vstartval){
	if((vstartval>=0)&&(vstartval<=262143))//2^18-1
	{
		TMC5160Controller::sendData(VSTART_ADR,vstartval,&_trashDatagram);
	}
}

void TMC5160Controller::setA1(uint32_t a1val){
	if((a1val>=0)&&(a1val<=65535))//2^16-1
	{
		TMC5160Controller::sendData(A1_ADR,a1val,&_trashDatagram);
	}
}

void TMC5160Controller::setV1(uint32_t v1val){
	if((v1val>=0)&&(v1val<=1048575))//2^20-1
	{
		TMC5160Controller::sendData(V1_ADR,v1val,&_trashDatagram);
	}
}

void TMC5160Controller::setAMAX(uint32_t amaxval){
	if((amaxval>=0)&&(amaxval<=65535))//2^16-1
	{
		TMC5160Controller::sendData(AMAX_ADR,amaxval,&_trashDatagram);
	}
}

void TMC5160Controller::setVMAX(uint32_t vmaxval){
	if((vmaxval>=0)&&(vmaxval<=8388096))//2^23-512
	{
		TMC5160Controller::sendData(VMAX_ADR,vmaxval,&_trashDatagram);
	}
}

void TMC5160Controller::setDMAX(uint32_t dmaxval){
	if((dmaxval>=0)&&(dmaxval<=65535))//2^16-1
	{
		TMC5160Controller::sendData(DMAX_ADR,dmaxval,&_trashDatagram);
	}
}

void TMC5160Controller::setD1(uint32_t d1val){
	if((d1val>0)&&(d1val<=65535))//2^16-1
	{
		TMC5160Controller::sendData(D1_ADR,d1val,&_trashDatagram);
	}
}

void TMC5160Controller::setVSTOP(uint32_t vstopval){
	if((vstopval>0)&&(vstopval<=262143))//2^18-1
	{
		TMC5160Controller::sendData(VSTOP_ADR,vstopval,&_trashDatagram);
	}
}

void TMC5160Controller::setTZEROWAIT(uint32_t tzerowaitval){
	if((tzerowaitval>=0)&&(tzerowaitval<=65535))//2^16-1
	{
		TMC5160Controller::sendData(TZEROWAIT_ADR,tzerowaitval,&_trashDatagram);
	}
}

void TMC5160Controller::setXTARGET(int32_t xtargetval){
	if((xtargetval>-2147483648)&&(xtargetval<=2147483647))//2^31-1
	{
		TMC5160Controller::sendData(XTARGET_ADR,(uint32_t)xtargetval,&_trashDatagram);
	}
}

void TMC5160Controller::setXACTUAL(int32_t xactualval){
	if((xactualval>-2147483648)&&(xactualval<=2147483647))//2^31-1
	{
		TMC5160Controller::sendData(XACTUAL_ADR,(uint32_t)xactualval,&_trashDatagram);
	}
}

void TMC5160Controller::setTPWMTHRS(uint32_t tpwmthrsval){
	if((tpwmthrsval>=0)&&(tpwmthrsval<=1048575))//2^20-1
	{
		TMC5160Controller::sendData(TPWMTHRS_ADR,(uint32_t)tpwmthrsval,&_trashDatagram);
	}
}