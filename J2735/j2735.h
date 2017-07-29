#ifndef _J2735_H
#define _J2735_H

#include <asm/errno.h>
#include <asm/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define J2735_MSG_BSM	0 /* basic safety message */
#define J2735_MSG_CSR	3 /* common safety request */
#define J2735_MSG_EVA	4 /* emergency vehicle alert */
#define J2735_MSG_IXC	5 /* intersection collision */
#define J2735_MSG_RSA	9 /* roadside alert */

#define BSM_SIZE 
#define EVA_SIZE
#define CSR_SIZE
#define RSA_SIZE

enum DSRCmsgID { // 1 byte = 8b = 0..255
	mapData					=   18, // MAP, intersections 
	signalPhaseAndTimingMessage		=   19, // SPAT 
   	basicSafetyMessage    			=   20, // BSM, heartbeat msg
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
   	travelerInformation        	  	=   31, // TIM
   	personalSafetyMessage           	=   32, // PSM
};
/*
enum VehicleEventFlags {
    eventHazardLights					= 0,
    eventStopLineViolation				= 1,
    eventABSactivated					= 2,
    eventTractionControlLoss			= 3,
    eventStabilityControlactivated		= 4,
    eventHazardousMaterials				= 5,
    eventReserved1						= 6,
    eventHardBraking					= 7,
    eventLightsChanged					= 8,
    eventWipersChanged					= 9,   
    eventFlatTire						= 10,
    eventDisabledVehicle				= 11,
    eventAirBagDeployment				= 12,
};*/

/* These are redundant, integrated below
struct BSMData {
	enum DSRCmsgID msgID; // A 1 byte instance 
  	uint8_t msgCnt;
  	uint8_t tid; // temp ID
  	uint8_t secMark;
  	uint8_t lat;
  	uint8_t longt;
  	uint8_t elev;
  	uint8_t accuracy;
  	uint8_t txstate;
  	uint8_t speed;
  	uint8_t heading;
  	uint8_t angle;
  	uint8_t accelSet[4];
  	uint8_t brakes;
  	uint8_t size;
};

struct FullPositionVector {
	uint8_t lat;
	uint8_t longt;
	uint8_t timestamp; // optional
	uint8_t elevation; // optional
	uint8_t heading; // optional
	uint8_t speed; // optional
	// confidences optional
};

struct EmergencyDetails {
  	uint8_t sspRights; // 0..31
  	uint8_t sirenUse; // 0..3
  	uint8_t lightsUse; // 0..7
  	uint8_t multi; // 0..3
  	//uint8_t events[2]; // sspRights event
  	uint8_t responsetype; // 0..6 - opt
};*/

/* General message frame */
struct j2735MSG {
  enum DSRCmsgID msgID; // A 1 byte instance 
  uint8_t *data;

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



#endif
