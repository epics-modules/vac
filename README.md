[![VAC](https://github.com/epics-modules/vac/actions/workflows/ci-scripts-build.yml/badge.svg)](https://github.com/epics-modules/vac/actions/workflows/ci-scripts-build.yml)

# vac

APS BCDA synApps module for vacuum system instrumentation.

The vac module provides EPICS custom record types, device support drivers, and databases for ion pump controllers and vacuum gauge controllers. It communicates with hardware over serial (RS-232/RS-485) and Ethernet (TCP/IP) using the ASYN driver framework.

## Supported Devices

**Ion Pump Controllers** -- via the `digitel` record type

| Device | Manufacturer |
|--------|-------------|
| Digitel 500 / 1500 | Physical Electronics |
| MPC / MPC-II | Gamma Vacuum |
| QPC / QPCe | Gamma Vacuum |
| SPCe | Gamma Vacuum |

**Vacuum Gauge Controllers** -- via the `vs` record type

| Device | Manufacturer |
|--------|-------------|
| GP307 / GP350 | Granville-Phillips |
| MM200 / MX200 | Televac |
| CC10 | Televac |

## Build Requirements

- **EPICS Base** (3.15+)
- **ASYN** (required)
- **IPAC** (vxWorks only, for tyGSOctal serial ports)
- **streamDevice** (optional, for QPC streamDevice databases)
- **modbus** (optional, for QPC Modbus databases)

## Documentation

Full documentation is available on [GitHub Pages](https://epics-modules.github.io/vac).

## Links

- [Report an issue](https://github.com/epics-modules/vac/issues/new?title=%20ISSUE%20NAME%20HERE&body=**Describe%20the%20issue**%0A%0A**Steps%20to%20reproduce**%0A1.%20Step%20one%0A2.%20Step%20two%0A3.%20Step%20three%0A%0A**Expected%20behaviour**%0A%0A**Actual%20behaviour**%0A%0A**Build%20Environment**%0AArchitecture:%0AEpics%20Base%20Version:%0ADependent%20Module%20Versions:&labels=bug)
- [Request a feature](https://github.com/epics-modules/vac/issues/new?title=%20FEATURE%20SHORT%20DESCRIPTION&body=**Feature%20Long%20Description**%0A%0A**Why%20should%20this%20be%20added?**%0A&labels=enhancement)
- [synApps](https://www.aps.anl.gov/BCDA/synApps)
- [License](https://www.aps.anl.gov/BCDA/synApps-license)
