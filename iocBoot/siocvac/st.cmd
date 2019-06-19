#!../../bin/linux-x86_64/fe

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase ("dbd/fe.dbd",0,0)
fe_registerRecordDeviceDriver(pdbbase)

# QPC ion pump controllers via Moxa Ports 4002 & 4003
drvAsynIPPortConfigure("IP1_4", "10.6.33.133:4002", 0, 0, 0)
drvAsynIPPortConfigure("IP5_8", "10.6.33.133:4003", 0, 0, 0)

drvAsynIPPortConfigure("TV1", "10.6.33.133:4005", 0, 0, 0)
drvAsynIPPortConfigure("Televac1", "10.6.33.133:4006", 0, 0, 0)

########################################################################
# MODBUS setup for QPC
# Configure Asyn TCP port for metasys
# drvAsynIPPortConfigure(portName, hostInfo, priority, noAutoConnect, noProcessEos)
#!drvAsynIPPortConfigure("QPC1Port","10.6.33.133:502",0,0,1)
# modbusInterposeConfig(portName,linkType,timeoutMsec)
#!modbusInterposeConfig("QPC1Port",0,3000)
# drvModbusAsynConfigure(portName,tcpPortName,slaveAddress,modbusFunction,
#                       modbusStartAddress,modbusLength,dataType,pollMsec,plcType)
# Modbus function 4 read address range
#!drvModbusAsynConfigure("QPC1","QPC1Port",0,4,64,64,0,100,"QPC1Port")
# Modbus function 3 read address range
#!drvModbusAsynConfigure("QPC2","QPC1Port",0,3,0,119,0,100,"QPC1Port")
#!drvModbusAsynConfigure("QPC3","QPC1Port",0,3,200,81,0,100,"QPC1Port")
# Used for write access modbus function 6
#!drvModbusAsynConfigure("QPC4","QPC1Port",0,6,200,81,0,100,"QPC1Port")
# Used for write access modbus function 5
#!drvModbusAsynConfigure("QPC5","QPC1Port",0,5,0,7,6,100,"QPC1Port")
# Modbus function 2 read address range
#!drvModbusAsynConfigure("QPC6","QPC1Port",0,2,8,4,0,100,"QPC1Port")
# This part does not make any sense to me yet .....
# This is the status of the first pump/supply only the length is 64 bits
# as such Modbus function 2 is bit access
#!drvModbusAsynConfigure("QPC7","QPC1Port",0,2,24,64,0,100,"QPC1Port")
# Pump 2 starts at address 89
#!drvModbusAsynConfigure("QPC8","QPC1Port",0,2,89,64,0,100,"QPC1Port")
########################################################################

###################################################################
# streamDevice
epicsEnvSet ("STREAM_PROTOCOL_PATH", "${TOP}/db")

# Use this for QPC over Ethernet (QPC uses Ethernet port 23)
drvAsynIPPortConfigure ("QPC-1", "10.6.33.134:23",0,0,0)
asynOctetSetInputEos("QPC-1", -1, "\r\r")
asynOctetSetOutputEos("QPC-1", -1, "\r")
asynSetTraceMask("QPC-1", -1, 1)
asynSetTraceIOMask("QPC-1", -1, 1)
###################################################################
# Serial Port via Moxa box
###################################################################

## Load record instances
# STN must be set to the pump number for the QPC (currently)
#     for the Digitel 500/1500 it is the number of set points per controller
#     for the MPC it is the first set point number and odd pump numbers are
#         assumed to be set points 1 & 3 while even pump numbers are assumed
#	  to be even set points 2 & 4
# The set point for each pump is the same as the pump number
dbLoadRecords("db/digitelPump.db", "P=FE:28:ID:,PUMP=IP1,PORT=IP1_4,ADDR=5,DEV=QPC,STN=1")
dbLoadRecords("db/digitelPump.db", "P=FE:28:ID:,PUMP=IP2,PORT=IP1_4,ADDR=5,DEV=QPC,STN=2")
dbLoadRecords("db/digitelPump.db", "P=FE:28:ID:,PUMP=IP3,PORT=IP1_4,ADDR=5,DEV=QPC,STN=3")
dbLoadRecords("db/digitelPump.db", "P=FE:28:ID:,PUMP=IP4,PORT=IP1_4,ADDR=5,DEV=QPC,STN=4")

dbLoadRecords("db/digitelPump.db", "P=FE:28:ID:,PUMP=IP5,PORT=IP5_8,ADDR=5,DEV=QPC,STN=1")
dbLoadRecords("db/digitelPump.db", "P=FE:28:ID:,PUMP=IP6,PORT=IP5_8,ADDR=5,DEV=QPC,STN=2")
dbLoadRecords("db/digitelPump.db", "P=FE:28:ID:,PUMP=IP7,PORT=IP5_8,ADDR=5,DEV=QPC,STN=3")
dbLoadRecords("db/digitelPump.db", "P=FE:28:ID:,PUMP=IP8,PORT=IP5_8,ADDR=5,DEV=QPC,STN=4")

#  For MM200 the Televac has two cold cathodes and a minimum of 2 convectrons
#       if STN is either 5/6 then the corresponding CV1 are 1/2 and CV2 are 3/4
#       if STN is either 3/4 then the corresponding CV is 1/2 and no CV2
#   MM200 expects "\r" for EOS for both inputs and outputs.
#!dbLoadRecords("db/vs.db","P=FE:28:ID:,GAUGE=VS1,PORT=TV1,ADDR=0,DEV=MM200,STN=5")
#!dbLoadRecords("db/vs.db","P=FE:28:ID:,GAUGE=VS2,PORT=TV1,ADDR=0,DEV=MM200,STN=6")

#!dbLoadRecords("db/vs.db", "P=TV:,GAUGE=VS1,PORT=Televac1,ADDR=0,DEV=MX200,STN=3 1 0 1")
#!dbLoadRecords("db/vs.db", "P=TV:,GAUGE=VS2,PORT=Televac1,ADDR=0,DEV=MX200,STN=4 0 2 2")

# QPC
asynOctetSetInputEos("IP1_4", -1, "\r")
asynOctetSetOutputEos("IP1_4", -1, "\r")
asynOctetSetInputEos("IP5_8", -1, "\r")
asynOctetSetOutputEos("IP5_8", -1, "\r")

# Televac
#!asynOctetSetInputEos("TV1", -1, "\r")
#!asynOctetSetOutputEos("TV1", -1, "\r")
#!asynOctetSetInputEos("Televac1", -1, "\r")
#!asynOctetSetOutputEos("Televac1", -1, "\r")


cd ${TOP}/iocBoot/${IOC}

iocInit

