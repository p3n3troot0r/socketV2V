#include <asm/errno.h>
#include <asm/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "wsmp.h"

enum DSRCmsgID {
	mapData								= 	18, // MAP, intersections 
	signalPhaseAndTimingMessage		  	= 	19, // SPAT 
   	basicSafetyMessage    			    =   20, // BSM, heartbeat msg
   	commonSafetyRequest              	=   21, //  CSR
   	emergencyVehicleAlert            	=   22, // EVA
   	intersectionCollision           	=   23, // ICA
   	nmeaCorrections                		=   24, // NMEA
   	probeDataManagement              	=   25, // PDM
   	probeVehicleData                	=   26, // PVD
   	roadSideAlert                    	=   27, // RSA
   	rtcmCorrections                		=   28, // RTCM
   	signalRequestMessage            	=   29, // SRM
   	signalStatusMessage             	=   30, // SSM
   	travelerInformation        	  		=   31, // TIM
   	personalSafetyMessage           	=   32, // PSM
};

/* BasicSafetyMessage */
struct BSM {
	enum DSRCmsgID msgID; 
	struct {
		uint8_t msgCnt;
  		uint32_t tid; 
  		uint16_t secMark; 
      	uint32_t lat; 
  		uint32_t longt;
  		uint16_t elev;
  		uint32_t accuracy;
  		uint32_t txstate; 
  		uint16_t speed;
  		uint16_t heading;
  		uint8_t angle;
  		uint64_t accelSet[4];
  		uint16_t brakes;
  		uint32_t size;
	} blob1; 
};


struct RoadSideAlert { 
	enum DSRCmsgID msgID; // A 1 byte instance 
   	uint8_t msgCnt;
   	uint16_t typeEvent;
   	struct {
		uint32_t lat;
		uint32_t longt;
	} position; // optional - opt like -pos XXXX for ts/elev/speed/etc
};

struct EmergencyVehicleAlert  {
	enum DSRCmsgID msgID; // A 1 byte instance 
   	uint16_t timeStamp; // optional
   	uint32_t tid; // optional
	struct RoadSideAlert *rsaMsg;
    uint8_t responseType; // optional            
    uint8_t mass; // 0..255 = optional
    uint8_t basicType; // 0..15 = optional = axle count 
};

int main() {

	struct BSM *bsm1;
	bsm1 = calloc(sizeof(struct BSM),1);
	bsm1->msgID = basicSafetyMessage; 
	bsm1->blob1.msgCnt = 0xAB;
	bsm1->blob1.tid = 0x12345678;
	bsm1->blob1.secMark = 0x6358;
	bsm1->blob1.lat = 0x67BA2100;
	bsm1->blob1.longt = 0x1A4B263D;
	bsm1->blob1.elev = 0xAAAA;
	bsm1->blob1.accuracy = 0x1221ABAB;
	bsm1->blob1.txstate = 0x3;
	bsm1->blob1.speed = 0xA2B;
	bsm1->blob1.heading = 0x1212;
	bsm1->blob1.angle = 0x42;
	bsm1->blob1.accelSet[0] = 0x0A0B;
	bsm1->blob1.accelSet[1] = 0x0C0D;
	bsm1->blob1.accelSet[2] = 0x0FA;
	bsm1->blob1.accelSet[3] = 0x1AFBC;
	bsm1->blob1.brakes = 0xA134;
	bsm1->blob1.size = 0x01323456;

	struct wsmp_wsm *wsm1;
	wsm1 = calloc(sizeof(struct wsmp_wsm),1);
	wsm1->subtype = 0x0;
	wsm1->version = 0x3;
	wsm1->tpid = 0x00;
	wsm1->use_n_iex = 0;
	wsm1->psid = 0xC00305;
	wsm1->use_t_iex = 0;
	wsm1->len = sizeof(*bsm1);
	wsm1->data = (uint8_t *) bsm1;


	struct RoadSideAlert *rsa1;
	rsa1 = calloc(sizeof(struct RoadSideAlert),1);
	rsa1->msgID = roadSideAlert; // A 1 byte instance 
	rsa1->msgCnt = 0xBA;
	rsa1->typeEvent = 0xDEFC;
	rsa1->position.lat = 0x67BA2100;
	rsa1->position.longt = 0x1A4B263D;

	struct EmergencyVehicleAlert *eva1;
	eva1 = calloc(sizeof(struct EmergencyVehicleAlert),1);
	eva1->msgID = emergencyVehicleAlert; // A 1 byte instance 
   	eva1->timeStamp = 0x515; // optional
	eva1->tid = 0x12345678;
	eva1->rsaMsg = rsa1;
    eva1->basicType = 0x04;

	struct wsmp_wsm *wsm2;
	wsm2 = calloc(sizeof(struct wsmp_wsm),1);
	wsm2->subtype = 0x0;
	wsm2->version = 0x3;
	wsm2->tpid = 0x00;
	wsm2->use_n_iex = 0;
	wsm2->psid = 0xC00305;
	wsm2->use_t_iex = 0;
	wsm2->len = sizeof(*rsa1);
	wsm2->data = (uint8_t *) rsa1;

	struct wsmp_wsm *wsm3;
	wsm3 = calloc(sizeof(struct wsmp_wsm),1);
	wsm3->subtype = 0x0;
	wsm3->version = 0x3;
	wsm3->tpid = 0x00;
	wsm3->use_n_iex = 0;
	wsm3->psid = 0xC00305;
	wsm3->use_t_iex = 0;
	wsm3->len = sizeof(*eva1);
	wsm3->data = (uint8_t *) eva1;
}









