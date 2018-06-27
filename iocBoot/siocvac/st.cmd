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


## Load record instances
# STN must be set to the pump number
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


asynOctetSetInputEos("IP1_4", -1, "\r")
asynOctetSetOutputEos("IP1_4", -1, "\r")

asynOctetSetInputEos("IP5_8", -1, "\r")
asynOctetSetOutputEos("IP5_8", -1, "\r")

#!asynOctetSetInputEos("TV1", -1, "\r")
#!asynOctetSetOutputEos("TV1", -1, "\r")

#!asynOctetSetInputEos("Televac1", -1, "\r")
#!asynOctetSetOutputEos("Televac1", -1, "\r")


cd ${TOP}/iocBoot/${IOC}

iocInit

