---
layout: default
title: iocsh Scripts
nav_order: 5
---

# iocsh Scripts

The vac module provides pre-built iocsh scripts that handle serial port configuration and database loading in a single call. These scripts are the recommended way to add vacuum devices to an IOC.

The scripts are installed to the top-level `iocsh/` directory and are loaded using the `iocshLoad` command (available in EPICS Base 3.15+).

## Ion Pump Controllers

### digitelPump.iocsh

Top-level dispatcher script for ion pump controllers. It selects the correct device-specific script based on the `DEV` macro.

```
iocshLoad("$(VAC)/iocsh/digitelPump.iocsh", "PREFIX=SR:, INSTANCE=IP1, PORT=/dev/ttyUSB0, DEV=MPC, STN=1")
```

#### Macros

| Macro | Required | Default | Description |
|-------|----------|---------|-------------|
| `PREFIX` | Yes | | PV prefix (maps to `P` in the database) |
| `INSTANCE` | Yes | | PV suffix / pump identifier (maps to `PUMP` in the database) |
| `PORT` | Yes | | Serial port device path (e.g. `/dev/ttyUSB0`) |
| `DEV` | Yes | | Device type: `D500` or `MPC` |
| `STN` | Yes | | Station number (pump number for MPC, setpoint number for D500/D1500) |
| `ADDR` | No | `0` | Device address (for MPC on RS-485) |
| `VAC` | No | (auto) | Path to the vac module top directory |

### Available Device Scripts

| Script | Devices |
|--------|---------|
| `digitelPump_D500.iocsh` | Digitel 500, Digitel 1500 |
| `digitelPump_MPC.iocsh` | MPC, MPC-II, LPC |

### Examples

```
# Digitel 500
iocshLoad("$(VAC)/iocsh/digitelPump.iocsh", "PREFIX=SR:, INSTANCE=IP1, PORT=/dev/ttyUSB0, DEV=D500, STN=2")

# Two pumps on the same MPC controller
iocshLoad("$(VAC)/iocsh/digitelPump.iocsh", "PREFIX=SR:, INSTANCE=IP1, PORT=/dev/ttyUSB0, DEV=MPC, STN=1, ADDR=5")
iocshLoad("$(VAC)/iocsh/digitelPump.iocsh", "PREFIX=SR:, INSTANCE=IP2, PORT=/dev/ttyUSB0, DEV=MPC, STN=2, ADDR=5")
```

Note: There is no `digitelPump_QPC.iocsh` script. For QPC controllers using the `digitel` record, use manual st.cmd configuration as described on the [ion pump controllers](ion-pumps#example-manual-stcmd-configuration) page. For QPC controllers using streamDevice, see the `QPCpump.iocsh` script below.

## QPC Ion Pump Controllers (streamDevice)

### QPCpump.iocsh

Script for QPC ion pump controllers using the [streamDevice database](qpc). This loads the `QPCstreams.db` database with the correct protocol file based on the communication type.

Unlike the other iocsh scripts, the ASYN port must be created by the user before calling this script, since the port type depends on the connection (direct serial, Moxa terminal server, direct TCP to QPCe, etc.).

```
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP1, PORT=QPC1, SPLY=1, COMM=serial")
```

#### Macros

| Macro | Required | Default | Description |
|-------|----------|---------|-------------|
| `PREFIX` | Yes | | PV prefix (maps to `P` in the database) |
| `INSTANCE` | Yes | | PV suffix / pump identifier (maps to `PMP` in the database) |
| `PORT` | Yes | | ASYN port name (must already be created) |
| `SPLY` | Yes | | Pump supply number (1--4) |
| `SPT` | No | same as `SPLY` | Setpoint number |
| `COMM` | No | `serial` | Communication type: `serial` or `tcp` |
| `VAC` | No | (auto) | Path to the vac module top directory |

Use `COMM=serial` for RS-232, RS-485, or serial-over-Ethernet (e.g. Moxa terminal server). Use `COMM=tcp` for direct TCP connections to the QPCe Ethernet port (port 23).

### Examples

```
# QPC via direct TCP to QPCe (port 23)
drvAsynIPPortConfigure("QPC1", "192.168.1.100:23", 0, 0, 0)
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP1, PORT=QPC1, SPLY=1, COMM=tcp")
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP2, PORT=QPC1, SPLY=2, COMM=tcp")
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP3, PORT=QPC1, SPLY=3, COMM=tcp")
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP4, PORT=QPC1, SPLY=4, COMM=tcp")

# QPC via serial over Moxa terminal server
drvAsynIPPortConfigure("QPC1_ser", "10.6.33.133:4002", 0, 0, 0)
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP1, PORT=QPC1_ser, SPLY=1")
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP2, PORT=QPC1_ser, SPLY=2")
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP3, PORT=QPC1_ser, SPLY=3")
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP4, PORT=QPC1_ser, SPLY=4")

# QPC via local serial port
drvAsynSerialPortConfigure("QPC1_local", "/dev/ttyUSB0", 0, 0, 0)
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP1, PORT=QPC1_local, SPLY=1")
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP2, PORT=QPC1_local, SPLY=2")
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP3, PORT=QPC1_local, SPLY=3")
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP4, PORT=QPC1_local, SPLY=4")
```

## Vacuum Gauge Controllers

### vacSensor.iocsh

Top-level dispatcher script for vacuum gauge controllers. It selects the correct device-specific script based on the `DEV` macro.

```
iocshLoad("$(VAC)/iocsh/vacSensor.iocsh", "PREFIX=SR:, INSTANCE=VS1, PORT=/dev/ttyUSB1, DEV=GP350")
```

#### Macros

| Macro | Required | Default | Description |
|-------|----------|---------|-------------|
| `PREFIX` | Yes | | PV prefix (maps to `P` in the database) |
| `INSTANCE` | Yes | | PV suffix / gauge identifier (maps to `GAUGE` in the database) |
| `PORT` | Yes | | Serial port device path (e.g. `/dev/ttyUSB1`) |
| `DEV` | Yes | | Device type: `GP307`, `GP350`, `MM200`, `MX200`, or `CC10` |
| `STN` | No | `0` | Station number (cold cathode number for MM200/MX200; unused for others) |
| `ADDR` | No | `0` | Device address (for RS-485) |
| `BAUD` | No | `9600` | Baud rate (MX200 only; factory default is 115200) |
| `VAC` | No | (auto) | Path to the vac module top directory |

### Available Device Scripts

| Script | Devices |
|--------|---------|
| `vacSensor_GP307.iocsh` | Granville-Phillips 307 |
| `vacSensor_GP350.iocsh` | Granville-Phillips 350 |
| `vacSensor_MM200.iocsh` | Televac MM200 |
| `vacSensor_MX200.iocsh` | Televac MX200 |
| `vacSensor_CC10.iocsh` | Televac CC10 |

### Examples

```
# GP307
iocshLoad("$(VAC)/iocsh/vacSensor.iocsh", "PREFIX=SR:, INSTANCE=VS1, PORT=/dev/ttyUSB0, DEV=GP307")

# GP350
iocshLoad("$(VAC)/iocsh/vacSensor.iocsh", "PREFIX=SR:, INSTANCE=VS2, PORT=/dev/ttyUSB1, DEV=GP350")

# MM200 with cold cathode station 5
iocshLoad("$(VAC)/iocsh/vacSensor.iocsh", "PREFIX=SR:, INSTANCE=VS3, PORT=/dev/ttyUSB2, DEV=MM200, STN=5")

# CC10 on RS-485
iocshLoad("$(VAC)/iocsh/vacSensor.iocsh", "PREFIX=SR:, INSTANCE=VS4, PORT=/dev/ttyUSB3, DEV=CC10, STN=0, ADDR=1")

# MX200 at default script baud rate (9600)
iocshLoad("$(VAC)/iocsh/vacSensor.iocsh", "PREFIX=SR:, INSTANCE=VS5, PORT=/dev/ttyUSB4, DEV=MX200, STN=3")

# MX200 at factory default baud rate (115200)
iocshLoad("$(VAC)/iocsh/vacSensor.iocsh", "PREFIX=SR:, INSTANCE=VS5, PORT=/dev/ttyUSB4, DEV=MX200, STN=3, BAUD=115200")
```

## How the Dispatcher Pattern Works

The dispatcher scripts (`digitelPump.iocsh` and `vacSensor.iocsh`) use macro substitution to select the correct device-specific script:

```
iocshLoad("$(VAC)/iocsh/digitelPump_$(DEV).iocsh", ...)
```

This means the `DEV` macro value must exactly match a device-specific script filename suffix. For example, `DEV=MPC` loads `digitelPump_MPC.iocsh`.

Each device-specific script handles the serial port setup (baud rate, data bits, parity, end-of-string characters) and loads the appropriate database file with the correct macros. The user does not need to know the serial settings for each device -- the script takes care of it.
