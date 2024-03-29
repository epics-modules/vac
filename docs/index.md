---
layout: default
title: Home
nav_order: 1
---


# VacApp Modules

## External use

	vacSupport.dbd
	libvac.a(so)

## Internals

	vsRecord.c
	vsRecord.dbd
	devVacSen.h             
	devVacSen.c               
	devVacSen.dbd             

	choiceDigitel.h           
	digitelRecord.dbd         
	digitelRecord.c           
	devDigitelPump.h          
	devDigitelPump.c          
	devDigitelPump.dbd        


	vacAppCommonInclude.dbd  
	vacAppInclude.dbd
	vacAppVXInclude.dbd

## Notes

August 2007.   Mohan Ramanathan

At this time the module has support for vacuum sensors GP307, GP350, Televac MM200

October 2014:
The new Televac controller CC10 has been aded to the Vac Sensor.

For the Ion pumps we have support for PI Digitel 500/1500 and the MPC (Gamma One)

Support for newer devices will be added at a later date.

The generic record "vs" is for the vac sensors. this record and 
the associated devVanSen supports the following gauges:
	GP307,  GP350,  MM200 (Televac) and CC10 (Televac)
	
The startup file  has the following substitution:
The database has a field called "TYPE" which has to be set correctly. 
vs.db has various substitions for startup file.  
  
	  If address is 0 then it is RS232 
	  For RS485 Address has to be a positive number
		for GP350 has to be between 1 and 31 of the form "AA"
	 	for MM200 has to be between 0 and 59 of the form [0..9..A..Z..a..z]
		for CC10 has to be between 0 and F  in HEX
	      Also for GP350 the prefix is "#" so we will force for MM200 the same!!
	 	  for CC10  the prefix is <STX>  = hex 02

### excerpts for an example st.cmd file

	#  For GP307 the STN are irrelavent
	#   GP307 expects "\n\r" for EOS for both inputs and outputs.
	dbLoadRecords("db/vs.db", "P=MR:,GAUGE=VS1,PORT=serial1,ADDR=0,DEV=GP307,STN=0")
	tyGSAsynInit("serial1",  "UART0", 0, 9600,'E',1,7,'N',"\r\n","\r\n")  

	#  For GP350 STN is irrelavant
	#   GP350 expects "\r" for EOS for both inputs and outputs.
	dbLoadRecords("db/vs.db", "P=MR:,GAUGE=VS2,PORT=serial2,ADDR=0,DEV=GP350,STN=0")
	tyGSAsynInit("serial2",  "UART0", 1, 9600,'N',1,8,'N',"\r","\r")  

	#  For MM200 the Televac has two cold cathodes and a minimum of 2 convectrons
	#	if STN is either 5/6 then the corresponding CV1 are 1/2 and CV2 are 3/4
	#	if STN is either 3/4 then the corresponding CV is 1/2 and no CV2
	#   MM200 expects "\r" for EOS for both inputs and outputs.
	dbLoadRecords("db/vs.db", "P=MR:,GAUGE=VS3,PORT=serial3,ADDR=0,DEV=MM200,STN=3")
	dbLoadRecords("db/vs.db", "P=MR:,GAUGE=VS4,PORT=serial3,ADDR=0,DEV=MM200,STN=4")
	tyGSAsynInit("serial3",  "UART0", 2, 9600,'N',1,8,'N',"\r","\r")  

	#  Alternative for MM200: specify CV1, CV2, and SPT directly.  If you specify more than
	#  one number for STN, the code expects you to specify all four.
	dbLoadRecords("db/vs.db","P=MR:,GAUGE=VS3,PORT=serial3,ADDR=0,DEV=MM200,STN=3 1 0 1")
	dbLoadRecords("db/vs.db","P=MR:,GAUGE=VS4,PORT=serial3,ADDR=0,DEV=MM200,STN=5 1 0 2")
	tyGSAsynInit("serial3",  "UART0", 2, 9600,'N',1,8,'N',"\r","\r")  

	#  For CC10 the Televac has a diode and the cold cathode integrated into one unit.
	#  It has three setpoints which are being read
	dbLoadRecords("db/vs.db", "P=MR:,GAUGE=VS1,PORT=serial3,ADDR=1,DEV=CC10,STN=0")
	tyGSAsynInit("serial3",  "UART0", 2, 9600,'N',1,8,'N',"\r","\r")  



	#  For Digitel 500/1500 the DEV= D500 or D1500.  The device talks only at 9600 7 E 1
	#  Also the  input EOS to device is "\r" while out from device is "\n\r"
	#  THE ADDR is irrelvant and STN stands for the no of setpoints from 0-3
	dbLoadRecords("db/digitelPump.db", "P=MR:,PUMP=IP1,PORT=serial5,ADDR=0,DEV=D500,STN=2")
	tyGSAsynInit("serial5",  "UART0", 4, 9600,'E',1,7,'N',"\n\r","\r")  

	#  For Gamma One MPC MPCe also LPC and SPC .  Device supports both RS232 and RS485
	#  THE ADDR is addreess of device (both RS232 and RS485
	#  STN stands for the no of the pump (MPC can do 2 devices) so 1 for pump1 and 2 for pump 2
	#  Then the corrresponding setpoints are odd and even for the respective pumps.
	dbLoadRecords("db/digitelPump.db", "P=MR:,PUMP=IP2,PORT=serial6,ADDR=5,DEV=MPC,STN=1")
	dbLoadRecords("db/digitelPump.db", "P=MR:,PUMP=IP3,PORT=serial6,ADDR=5,DEV=MPC,STN=2")
	tyGSAsynInit("serial6", "UART0", 5, 9600,'N',1,8,'N',"\r","\r") 

## Documentation

* [Documentation](vacDoc.md)
* [Release Notes](vacReleaseNotes.md)

