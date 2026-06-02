---
layout: default
title: QPC Alternate Databases
nav_order: 4
---

# QPC Alternate Databases

In addition to the [digitel record support](ion-pumps#qpcqpce), the QPC can be controlled using standard EPICS records through **streamDevice** or **Modbus** databases. These provide an alternative approach that does not require the custom `digitel` record type.

Each database creates records for a single pump supply on the QPC. Load one instance per pump.

## streamDevice Support

The `QPCstreams.db` database uses streamDevice with protocol files for either serial or Ethernet communication. It provides records for current, pressure, voltage, status, HV enable/disable, model, firmware version, pressure units, pump size, setpoint on/off pressures, setpoint status, and pump name.

### Macro Reference

| Macro | Description | Example |
|-------|-------------|---------|
| `P` | PV prefix | `FE:28:ID:` |
| `PMP` | Pump identifier | `IP1` |
| `SPLY` | Supply number (1--4) | `1` |
| `SPT` | Setpoint number (should match SPLY) | `1` |
| `PROTO` | Protocol file name (without `.proto`) | `QPC-eth` or `QPC-serial` |
| `PORT` | ASYN port name | `QPC1` |

### Protocol Files

Two protocol files are provided:

- **`QPC-eth.proto`** -- For Ethernet (TCP port 23). Uses the `cmd XX` command format without framing or checksum.
- **`QPC-serial.proto`** -- For RS-232/RS-485. Uses the `~ AA XX data CC` framed format with address and checksum bytes.

### Records Created

Each `QPCstreams.db` instance creates the following records (PV names are prefixed with `$(P)$(PMP):`):

| Record | Type | Description |
|--------|------|-------------|
| `Current` | ai | Pump current (5s scan) |
| `Pressure` | ai | Pump pressure (5s scan) |
| `Voltage` | ai | Pump voltage (5s scan) |
| `Status` | stringin | Supply status string (5s scan) |
| `isEnabled` | stringin | HV enabled status (5s scan) |
| `enable` | bo | Enable HV on this supply |
| `disable` | bo | Disable HV on this supply |
| `Model` | stringin | Controller model (at init) |
| `FirmwareVers` | stringin | Firmware version (at init) |
| `setPressUnits` | mbbo | Set pressure units (Torr/mbar/Pascal) |
| `getPressUnits` | mbbi | Pressure units readback (10s scan) |
| `PumpSize` | ai | Pump size in L/s (at init) |
| `setPumpSize` | ao | Set pump size (30--1200 L/s) |
| `Spt$(SPT)OnPress` | ai | Setpoint on pressure readback (5s scan) |
| `Spt$(SPT)OffPress` | ai | Setpoint off pressure readback (5s scan) |
| `Spt$(SPT)Status` | bi | Setpoint relay status (5s scan) |
| `setSpt$(SPT)OnPressure` | ao | Set the on pressure for a setpoint |
| `SptMessage` | stringin | Setpoint error message display |
| `setSpt$(SPT)OffPressure` | calcout | Set the off pressure (with validation) |
| `checkOffPressure` | calcout | Validates off pressure (must be 20% > on pressure) |
| `sendOffPressure` | ao | Sends validated off pressure to device |
| `OffSptMsg` | scalcout | Generates off setpoint range messages |
| `OffPressMsg` | scalcout | Generates off setpoint status messages |
| `OffSptMessage` | stringin | Off setpoint user message display |
| `Pump$(SPLY)Name` | stringin | Pump name (15 chars max, 5s scan) |

### Setpoint Notes

- Setting the on pressure to a value greater than the off pressure causes the QPC firmware to automatically adjust the off pressure to be 20% greater than the on pressure.
- Setting the off pressure must be at least 20% greater than the on pressure, or the controller returns an error. The database includes validation logic to check this before sending the command.
- Valid setpoint pressure range: 1.0E-11 to 1.0E-4.

### Example: Ethernet Configuration

```
# Configure ASYN IP port to QPC Ethernet port (TCP port 23)
drvAsynIPPortConfigure("QPC1", "192.168.1.100:23", 0, 0, 0)

# Load one instance per pump
dbLoadRecords("db/QPCstreams.db", "P=SR:,PMP=IP1,SPLY=1,SPT=1,PROTO=QPC-eth,PORT=QPC1")
dbLoadRecords("db/QPCstreams.db", "P=SR:,PMP=IP2,SPLY=2,SPT=2,PROTO=QPC-eth,PORT=QPC1")
dbLoadRecords("db/QPCstreams.db", "P=SR:,PMP=IP3,SPLY=3,SPT=3,PROTO=QPC-eth,PORT=QPC1")
dbLoadRecords("db/QPCstreams.db", "P=SR:,PMP=IP4,SPLY=4,SPT=4,PROTO=QPC-eth,PORT=QPC1")
```

### Example: Serial Configuration (via Moxa Terminal Server)

```
# Configure ASYN IP port to Moxa serial port
drvAsynIPPortConfigure("QPC1_serial", "10.6.33.133:4002", 0, 0, 0)
asynOctetSetInputEos("QPC1_serial", -1, "\r")
asynOctetSetOutputEos("QPC1_serial", -1, "\r")

# Load one instance per pump
dbLoadRecords("db/QPCstreams.db", "P=SR:,PMP=IP1,SPLY=1,SPT=1,PROTO=QPC-serial,PORT=QPC1_serial")
dbLoadRecords("db/QPCstreams.db", "P=SR:,PMP=IP2,SPLY=2,SPT=2,PROTO=QPC-serial,PORT=QPC1_serial")
dbLoadRecords("db/QPCstreams.db", "P=SR:,PMP=IP3,SPLY=3,SPT=3,PROTO=QPC-serial,PORT=QPC1_serial")
dbLoadRecords("db/QPCstreams.db", "P=SR:,PMP=IP4,SPLY=4,SPT=4,PROTO=QPC-serial,PORT=QPC1_serial")
```

## Modbus Support

The `QPCmodbus.db` database uses the EPICS Modbus module to communicate with the QPC over Modbus TCP (port 502). This database currently supports a single pump and requires multiple ASYN port configurations for the different Modbus function codes.

**Note:** This database is a work in progress. Status readback is not currently functional, and only a single pump is supported per database instance.

A QPC Modbus register map spreadsheet (`QPC_ModbusRegisterMap_v1_3-1.xlsx`) is included in the `docs/` directory.

### Macro Reference

| Macro | Description |
|-------|-------------|
| `P` | PV prefix |
| `PMP` | Pump identifier |
| `SPLY` | Supply number |
| `SPT` | Setpoint number |
| `PORT` | ASYN port for Modbus function 4 reads (voltage, current, pressure, model, firmware, MAC) |
| `PORT1` | ASYN port for Modbus function 3 reads (pump size, pump name) |
| `PORT2` | ASYN port for Modbus function 3 reads (setpoint on/off pressures, pressure units) |
| `PORT3` | ASYN port for Modbus function 6 writes (pressure units) |
| `PORT4` | ASYN port for Modbus function 5 writes (HV enable/disable) |
| `PORT5` | ASYN port for Modbus function 2 reads (setpoint status) |

### Example: Modbus Configuration

```
# Configure Modbus TCP connection
drvAsynIPPortConfigure("QPC_ETH", "192.168.1.100:502", 0, 0, 0)

# Configure Modbus ports for different function codes and register ranges
# Function 4 (Read Input Registers) - voltage, current, pressure, model, firmware, MAC
drvModbusAsynConfigure("QPC_FC4", "QPC_ETH", 0, 4, 0, 64, 0, 100, "Gamma")

# Function 3 (Read Holding Registers) - pump size, pump name
drvModbusAsynConfigure("QPC_FC3a", "QPC_ETH", 0, 3, 0, 48, 0, 100, "Gamma")

# Function 3 (Read Holding Registers) - setpoints, pressure units
drvModbusAsynConfigure("QPC_FC3b", "QPC_ETH", 0, 3, 100, 82, 0, 100, "Gamma")

# Function 6 (Write Single Register) - pressure units
drvModbusAsynConfigure("QPC_FC6", "QPC_ETH", 0, 6, 100, 1, 0, 100, "Gamma")

# Function 5 (Write Single Coil) - HV enable/disable
drvModbusAsynConfigure("QPC_FC5", "QPC_ETH", 0, 5, 0, 1, 0, 100, "Gamma")

# Function 2 (Read Discrete Inputs) - setpoint status
drvModbusAsynConfigure("QPC_FC2", "QPC_ETH", 0, 2, 0, 1, 0, 100, "Gamma")

# Load database
dbLoadRecords("db/QPCmodbus.db", "P=SR:,PMP=IP1,SPLY=1,SPT=1,PORT=QPC_FC4,PORT1=QPC_FC3a,PORT2=QPC_FC3b,PORT3=QPC_FC6,PORT4=QPC_FC5,PORT5=QPC_FC2")
```

## GUI Displays

### QPCsingle_pump_streams.adl

Display for a single QPC pump using the streamDevice database. Also available in `.ui`, `.opi`, `.edl`, and `.bob` formats.

### QPCpumps_streams.adl

Display showing all four QPC pumps using the streamDevice database.

For QPC displays using the `digitel` record, see the [ion pump controller displays](ion-pumps#gui-displays).
