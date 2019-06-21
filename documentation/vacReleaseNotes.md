# vac Release Notes

## Release 1-9 (June 21, 2019)

  * Device supports using Asyn have been updated to flush input queues at various places.
  * The Televac MM200 code has been gone over and some issues fixed.
  * The CONFIG_SITE and RELEASE files now have the standard `-include`s to support `.local` files.
  * The vsRecord's TYPE field is now correctly marked `SPC_NOMOD`.
  * Code comments have been improved and the code layout of both device support sources have gone through major reformatting.
  * The module now builds against recent EPICS Base versions without warnings.
  * The streamDevice protocol files for the QPC are now installed into the top-level `db/` directory.
  * Documentation has been converted to GitHub-flavored Markdown.

## Release 1-8 (August 15, 2018)

  * devDigitelPump.c: Corrected communication issue found with Digitel
500/1500
  * Added streamDevice support files (proto and databases) for both
serial and Ethernet communication to the QPC
  * Added Modbus support database for QPC (Currently only a single pump
and status is missing)
  * Updated some of the documentation and added a things to note section
to vacDoc and added QPC Modbus register map v1.3
  * This support module should also now work for the MPCq and the SPC as
well although has not been tested on either the command set is similar

## Release 1-7 (June 21, 2018)

  * devDigitelRecord.c: Exported recDigitelDebug variable and added a
set point 4 for the QPC device from Gamma Vacuum
  * digitelRecord.dbd: Added new DTYP for the QPC device as well as a
model and firmware version fields, which are also available for the MPC and
MPC-II devices from Gamma. Also added fields for set point 4 for the QPC
device.
  * devDigitelPump.c: Added support for the QPC device as well as some
more asynPrints
  * Added 2 new MEDM displays one for all QPC pumps (QPCpumps.adl) and
one for a single pump (QPCsingle_pump.adl)

## Release 1-6

  * devDigitelPump.c: Added null at end of readbuffer to make sure no overflow when writing to responsebuf. Problem with not setting proper EOS in asyn.
  * vsRecord, devVacSen: Added Televac MX200
  * New iocsh top-level directory, contains scripts with the correct shell commands to add an individual device to an IOC. For use with the iocshLoad command from EPICS base 3.15, further information can be found at <https://github.com/epics-modules/xxx/wiki/IOC-Shell-Scripts>

## Release 1-5-1 (Nov. 17, 2014)

  * Modifications to build support for EPICS 3.15. (Don't include asyn.dbd in vacSupport.dbd, because asyn.dbd includes asynRecord.dbd. EPICS 3.15 doesn't permit multiply defined record types.)

## Release 1-5 (Nov. 11, 2014)

  * Added "CC10" controller type, which is similar to Televac.
  * Updated display files for CSS-BOY and caQtDM.
  * CONFIG_SITE now -includes $(SUPPORT)/configure/CONFIG_SITE

## Release 1-4-1 (Apr. 17, 2013)

  * Modified devVacSen.c so user can specify station numbers which previously were hard coded.
  * Added display files for CSS-BOY and caQtDM.

## Release 1-4 (Oct. 24, 2011)

  * Modified RELEASE; deleted RELEASE.arch
  * Added .opi display files for CSS-BOY

## Release 1-3 (Mar. 30, 2010)

  * src/devDigitelPump.c - modifed to work with bitbus device
  * fixes for EPICS 3.14.11

## Release 1-2 (Dec. 19, 2008)

  * new files: vacApp/op/adl/Pump.adl, .../Pump_sp.adl, .../VacSen.adl
  * Db/vs.db, Db/digitelPump.db - added SCAN field

Suggestions and Comments to:
[Tim Mooney](mailto:mooney@aps.anl.gov) (mooney@aps.anl.gov)

