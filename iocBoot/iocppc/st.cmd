## Example vxWorks startup file

## The following is needed if your board support package doesn't at boot time
## automatically cd to the directory containing its startup script
#cd "/home/oxygen6/MOHAN/R3.14.8/iocBoot/iocppc"

< cdCommands
#< ../nfsCommands

cd topbin
ld < vacApp.munch

## Register all support components
cd top
dbLoadDatabase("dbd/vacAppVX.dbd")
vacAppVX_registerRecordDeviceDriver(pdbbase)

cd startup

iocsh("vac.iocsh")

iocInit()

## Start any sequence programs
#seq &sncExample,"user=mohan"
