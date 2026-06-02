---
layout: default
title: Release Notes
nav_order: 8
---

# vac Release Notes

## Release 1-9-2 (June 5, 2023)

- Documentation converted to GitHub Pages.

## Release 1-9-1 (Oct 5, 2020)

- Added Phoebus `.bob` display files; updated `.edl` and `.ui` files.
- iocsh scripts now installed to top-level folder from `vacApp/iocsh`.

## Release 1-9 (June 21, 2019)

- Device supports using ASYN have been updated to flush input queues at various places.
- The Televac MM200 code has been reviewed and several issues fixed.
- The `CONFIG_SITE` and `RELEASE` files now have the standard `-include`s to support `.local` files.
- The vsRecord's `TYPE` field is now correctly marked `SPC_NOMOD`.
- Code comments have been improved and both device support sources have gone through major reformatting.
- The module now builds against recent EPICS Base versions without warnings.
- The streamDevice protocol files for the QPC are now installed into the top-level `db/` directory.
- Documentation has been converted to GitHub-flavored Markdown.

## Release 1-8 (August 15, 2018)

- Corrected communication issue found with Digitel 500/1500 in `devDigitelPump.c`.
- Added streamDevice support files (proto and databases) for both serial and Ethernet communication to the QPC.
- Added Modbus support database for QPC (currently only a single pump; status readback is not working).
- Updated documentation and added QPC Modbus register map v1.3.
- This support module should also work for the MPCq and the SPC, although neither has been tested (the command set is similar).

## Release 1-7 (June 21, 2018)

- Exported `recDigitelDebug` variable and added setpoint 4 for the QPC device.
- Added new `DTYP` for the QPC device as well as model and firmware version fields (also available for MPC and MPC-II).
- Added fields for setpoint 4 in `digitelRecord.dbd` for the QPC device.
- Added support for the QPC device in `devDigitelPump.c` with additional `asynPrint` statements.
- Added two new MEDM displays: `QPCpumps.adl` (all QPC pumps) and `QPCsingle_pump.adl` (single pump).

## Release 1-6

- Added null terminator at end of read buffer to prevent overflow when writing to response buffer. Addressed problem with not setting proper EOS in ASYN.
- Added Televac MX200 support to `vsRecord` and `devVacSen`.
- New `iocsh/` top-level directory with scripts for the `iocshLoad` command (EPICS Base 3.15+). See the [xxx wiki](https://github.com/epics-modules/xxx/wiki/IOC-Shell-Scripts) for further information.

## Release 1-5-1 (Nov 17, 2014)

- Modifications to build with EPICS 3.15 (removed `asyn.dbd` from `vacSupport.dbd` to avoid duplicate record type definitions).

## Release 1-5 (Nov 11, 2014)

- Added CC10 controller type (Televac).
- Updated display files for CSS-BOY and caQtDM.
- `CONFIG_SITE` now `-include`s `$(SUPPORT)/configure/CONFIG_SITE`.

## Release 1-4-1 (Apr 17, 2013)

- Modified `devVacSen.c` so users can specify station numbers which were previously hard-coded.
- Added display files for CSS-BOY and caQtDM.

## Release 1-4 (Oct 24, 2011)

- Modified `RELEASE`; deleted `RELEASE.arch`.
- Added `.opi` display files for CSS-BOY.

## Release 1-3 (Mar 30, 2010)

- Modified `devDigitelPump.c` to work with bitbus device.
- Fixes for EPICS 3.14.11.

## Release 1-2 (Dec 19, 2008)

- New display files: `Pump.adl`, `Pump_sp.adl`, `VacSen.adl`.
- Added `SCAN` field to `digitelPump.db` and `vs.db`.

---

Suggestions and comments to: [Keenan Lang](mailto:klang@anl.gov) (klang@anl.gov)
