---
layout: default
title: Home
nav_order: 1
---

# The synApps vac Module

The vac module provides EPICS custom record types, device support drivers, and databases for vacuum system instrumentation. It supports ion pump controllers and vacuum gauge controllers from several manufacturers, communicating over serial (RS-232/RS-485) and Ethernet (TCP/IP) using the ASYN driver framework.

## Supported Devices

### Ion Pump Controllers

Supported through the [`digitel` record type](digitelRecord) and the `devDigitelPump` device support driver.

| Device | Manufacturer | Communication |
|--------|-------------|---------------|
| [Digitel 500 / 1500](ion-pumps#digitel-5001500) | Physical Electronics | RS-232 |
| [MPC / MPC-II](ion-pumps#mpcmpc-ii) | Gamma Vacuum | RS-232, RS-485 |
| [QPC / QPCe](ion-pumps#qpcqpce) | Gamma Vacuum | RS-232, RS-485, Ethernet |
| [SPCe](ion-pumps#spce) | Gamma Vacuum | RS-232, RS-485, Ethernet |

The QPC is also supported through [streamDevice and Modbus databases](qpc) using standard EPICS records.

### Vacuum Gauge Controllers

Supported through the [`vs` record type](vsRecord) and the `devVacSen` device support driver.

| Device | Manufacturer | Communication |
|--------|-------------|---------------|
| [GP307](vacuum-gauges#gp307) | Granville-Phillips | RS-232 |
| [GP350](vacuum-gauges#gp350) | Granville-Phillips | RS-232, RS-485 |
| [MM200](vacuum-gauges#mm200) | Televac | RS-232 |
| [MX200](vacuum-gauges#mx200) | Televac | RS-232, RS-485 |
| [CC10](vacuum-gauges#cc10) | Televac | RS-232, RS-485 |

## Build Requirements

| Module | Required | Notes |
|--------|----------|-------|
| ASYN | Yes | Serial and network communication |
| IPAC | vxWorks only | tyGSOctal serial port support |
| streamDevice | Optional | For QPC streamDevice databases |
| modbus | Optional | For QPC Modbus databases |

The vac module publishes `vacSupport.dbd` and `libvac` for use by IOC applications.

## Quick Start

The recommended way to configure devices is through the provided [iocsh scripts](iocsh-scripts), which handle serial port setup and database loading in a single call:

```
iocshLoad("$(VAC)/iocsh/digitelPump.iocsh", "PREFIX=SR:, INSTANCE=IP1, PORT=/dev/ttyUSB0, DEV=MPC, STN=1")
iocshLoad("$(VAC)/iocsh/vacSensor.iocsh", "PREFIX=SR:, INSTANCE=VS1, PORT=/dev/ttyUSB1, DEV=GP350")
```

For manual configuration details, see the [Ion Pumps](ion-pumps) and [Vacuum Gauges](vacuum-gauges) pages.

## Documentation

- [Ion Pump Controllers](ion-pumps) -- Digitel, MPC, QPC setup and configuration
- [Vacuum Gauge Controllers](vacuum-gauges) -- GP307, GP350, MM200, MX200, CC10 setup and configuration
- [QPC Alternate Databases](qpc) -- streamDevice and Modbus support for the QPC
- [digitel Record Reference](digitelRecord) -- Field reference for the digitel record type
- [vs Record Reference](vsRecord) -- Field reference for the vs record type
- [iocsh Scripts](iocsh-scripts) -- Pre-built configuration scripts
- [Release Notes](vacReleaseNotes) -- Version history

## Credits

- **Greg Nawrocki** -- Original Digitel 500 device support
- **Mohan Ramanathan** -- Record types, device support for MPC, GP307, GP350, MM200, MX200
- **Marty Smith** -- QPC support and documentation
- **Tim Mooney** -- Module management and releases
- **Keenan Lang** -- Current maintainer ([klang@anl.gov](mailto:klang@anl.gov))
