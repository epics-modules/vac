---
layout: default
title: Release Notes
nav_order: 2
---

# vac Release Notes
{: .no_toc}

## Table of contents
{: .no_toc .text-delta }

- TOC
{:toc}

## Release 1-9-2 (June 5, 2023)

- **Documentation** -- Converted to GitHub Pages.

## Release 1-9-1 (Oct 5, 2020)

- **Phoebus displays** -- Added `.bob` display files; updated `.edl` and `.ui` files.
- **iocsh installation** -- iocsh scripts now installed to top-level folder from `vacApp/iocsh`.

## Release 1-9 (June 21, 2019)

- **ASYN flush** -- Device supports using ASYN have been updated to flush input queues at various places.
- **MM200 fixes** -- The Televac MM200 code has been reviewed and several issues fixed.
- **Build configuration** -- The `CONFIG_SITE` and `RELEASE` files now have the standard `-include`s to support `.local` files.
- **vsRecord TYPE field** -- Now correctly marked `SPC_NOMOD`.
- **Code cleanup** -- Code comments have been improved and both device support sources have gone through major reformatting.
- **Build compatibility** -- The module now builds against recent EPICS Base versions without warnings.
- **QPC proto installation** -- The streamDevice protocol files for the QPC are now installed into the top-level `db/` directory.
- **Documentation** -- Converted to GitHub-flavored Markdown.

## Release 1-8 (August 15, 2018)

- **Digitel 500/1500 fix** -- Corrected communication issue in `devDigitelPump.c`.
- **QPC streamDevice** -- Added streamDevice support files (proto and databases) for both serial and Ethernet communication to the QPC.
- **QPC Modbus** -- Added Modbus support database for QPC (currently only a single pump; status readback is not working).
- **Documentation** -- Updated documentation and added QPC Modbus register map v1.3.
- **MPCq/SPC compatibility** -- This support module should also work for the MPCq and the SPC, although neither has been tested (the command set is similar).

## Release 1-7 (June 21, 2018)

- **QPC support** -- Added support for the QPC device in `devDigitelPump.c` with additional `asynPrint` statements.
- **QPC record fields** -- Added new `DTYP` for the QPC device, model and firmware version fields (also available for MPC and MPC-II), and setpoint 4 fields in `digitelRecord.dbd`.
- **Debug variable** -- Exported `recDigitelDebug` variable.
- **QPC displays** -- Added two new MEDM displays: `QPCpumps.adl` (all QPC pumps) and `QPCsingle_pump.adl` (single pump).

## Release 1-6

- **Buffer overflow fix** -- Added null terminator at end of read buffer to prevent overflow when writing to response buffer. Addressed problem with not setting proper EOS in ASYN.
- **MX200 support** -- Added Televac MX200 support to `vsRecord` and `devVacSen`.
- **iocsh scripts** -- New `iocsh/` top-level directory with scripts for the `iocshLoad` command (EPICS Base 3.15+). See the [xxx wiki](https://github.com/epics-modules/xxx/wiki/IOC-Shell-Scripts) for further information.

## Release 1-5-1 (Nov 17, 2014)

- **EPICS 3.15 compatibility** -- Removed `asyn.dbd` from `vacSupport.dbd` to avoid duplicate record type definitions.

## Release 1-5 (Nov 11, 2014)

- **CC10 support** -- Added CC10 controller type (Televac).
- **Display files** -- Updated display files for CSS-BOY and caQtDM.
- **Build configuration** -- `CONFIG_SITE` now `-include`s `$(SUPPORT)/configure/CONFIG_SITE`.

## Release 1-4-1 (Apr 17, 2013)

- **Configurable station numbers** -- Modified `devVacSen.c` so users can specify station numbers which were previously hard-coded.
- **Display files** -- Added display files for CSS-BOY and caQtDM.

## Release 1-4 (Oct 24, 2011)

- **Build configuration** -- Modified `RELEASE`; deleted `RELEASE.arch`.
- **CSS-BOY displays** -- Added `.opi` display files for CSS-BOY.

## Release 1-3 (Mar 30, 2010)

- **Bitbus support** -- Modified `devDigitelPump.c` to work with bitbus device.
- **EPICS 3.14.11 fixes** -- Build fixes for EPICS 3.14.11.

## Release 1-2 (Dec 19, 2008)

- **Display files** -- New display files: `Pump.adl`, `Pump_sp.adl`, `VacSen.adl`.
- **SCAN field** -- Added `SCAN` field to `digitelPump.db` and `vs.db`.

---

Suggestions and comments to: [Keenan Lang](mailto:klang@anl.gov) (klang@anl.gov)
