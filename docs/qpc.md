---
layout: default
title: QPC Alternate Databases
nav_order: 4
---

# QPC Alternate Databases

In addition to the [digitel record support](ion-pumps#qpcqpce), the QPC can be controlled using standard EPICS records through **streamDevice** or **Modbus** databases. These provide an alternative approach that does not require the custom `digitel` record type.

Each database creates records for a single pump supply on the QPC. Load one instance per pump.

## streamDevice Support

The `QPCstreams.db` database uses streamDevice with protocol files for either serial or TCP communication. It provides records for current, pressure, voltage, status, HV enable/disable, model, firmware version, pressure units, pump size, setpoint on/off pressures, setpoint status, and pump name.

### Using the iocsh Script (Recommended)

The [`QPCpump.iocsh`](iocsh-scripts#qpc-ion-pump-controllers-streamdevice) script handles protocol file selection and database loading. Create the ASYN port first, then call the script once per pump:

```
# QPC via direct TCP to QPCe (port 23)
drvAsynIPPortConfigure("QPC1", "192.168.1.100:23", 0, 0, 0)
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP1, PORT=QPC1, SPLY=1, COMM=tcp")
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP2, PORT=QPC1, SPLY=2, COMM=tcp")
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP3, PORT=QPC1, SPLY=3, COMM=tcp")
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP4, PORT=QPC1, SPLY=4, COMM=tcp")

# QPC via serial over Moxa terminal server (COMM defaults to serial)
drvAsynIPPortConfigure("QPC1_ser", "10.6.33.133:4002", 0, 0, 0)
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP1, PORT=QPC1_ser, SPLY=1")
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP2, PORT=QPC1_ser, SPLY=2")
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP3, PORT=QPC1_ser, SPLY=3")
iocshLoad("$(VAC)/iocsh/QPCpump.iocsh", "PREFIX=SR:, INSTANCE=IP4, PORT=QPC1_ser, SPLY=4")
```

See the [iocsh scripts](iocsh-scripts#qpc-ion-pump-controllers-streamdevice) page for full macro reference and additional examples.

### Database Macro Reference

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

- **`QPC-eth.proto`** -- For direct TCP connections (port 23). Uses the `cmd XX` command format without framing or checksum.
- **`QPC-serial.proto`** -- For RS-232/RS-485 (including serial-over-Ethernet via Moxa). Uses the `~ AA XX data CC` framed format with address and checksum bytes.

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

### Manual st.cmd: TCP Configuration

```
# Configure ASYN IP port to QPC Ethernet port (TCP port 23)
drvAsynIPPortConfigure("QPC1", "192.168.1.100:23", 0, 0, 0)

# Load one instance per pump
dbLoadRecords("db/QPCstreams.db", "P=SR:,PMP=IP1,SPLY=1,SPT=1,PROTO=QPC-eth,PORT=QPC1")
dbLoadRecords("db/QPCstreams.db", "P=SR:,PMP=IP2,SPLY=2,SPT=2,PROTO=QPC-eth,PORT=QPC1")
dbLoadRecords("db/QPCstreams.db", "P=SR:,PMP=IP3,SPLY=3,SPT=3,PROTO=QPC-eth,PORT=QPC1")
dbLoadRecords("db/QPCstreams.db", "P=SR:,PMP=IP4,SPLY=4,SPT=4,PROTO=QPC-eth,PORT=QPC1")
```

### Manual st.cmd: Serial Configuration (via Moxa Terminal Server)

```
# Configure ASYN IP port to Moxa serial port
drvAsynIPPortConfigure("QPC1_serial", "10.6.33.133:4002", 0, 0, 0)

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

## Comparing the digitel Record and streamDevice Implementations

The QPC can be controlled through either the custom [`digitel` record](ion-pumps#qpcqpce) or the streamDevice databases described above. Both talk to the same hardware using the same underlying serial/TCP protocol, but they differ in several ways.

### Protocol Differences

| Aspect | digitel record | streamDevice |
|--------|---------------|-------------|
| Device address | Configurable (0--255, from ASYN link) | Hardcoded to `05` in the proto file |
| Checksum | Computed per message (sum of bytes after `~`, masked to 0xFF) | Hardcoded `00` (QPC firmware accepts this) |
| Setpoint read command | `3C` (legacy) | `3B` (newer, replaces `3C`) |
| Setpoint write command | `3D` (legacy) | `3B` (newer, replaces `3D`) |
| Voltage parsing | Integer (`%d`) | Float (`%f`) |
| I/O timeout | 1.0 second | 2.0 seconds |

### Setpoint Commands

The two implementations use different setpoint commands:

- The **digitel record** uses the legacy `3C` (read) and `3D` (write) commands. These are deprecated but still supported by the QPC firmware for backward compatibility.
- The **streamDevice database** uses the newer `3B` command for both reads and writes. This is the replacement command introduced in newer firmware revisions.

The `3B` command has a known issue as of firmware 1.35: readback works correctly, but setting setpoints does not work properly. The digitel record avoids this by using the older `3C`/`3D` commands. The streamDevice database includes a `@mismatch` error handler that captures the error message from the controller when a write fails.

Additionally, the digitel record only reads **one setpoint per pump** for the QPC (the setpoint whose number matches the pump number). The streamDevice approach reads each setpoint independently through separate records.

### Feature Comparison

| Feature | digitel record | streamDevice |
|---------|---------------|-------------|
| Pressure readback | Yes | Yes |
| Current readback | Yes | Yes |
| Voltage readback | Yes | Yes |
| Supply status | Yes | Yes |
| HV enable/disable | Yes | Yes |
| Model number | Yes | Yes |
| Firmware version | Yes | Yes |
| Pump size readback | Yes | Yes |
| Setpoint read/write | Yes (1 per pump) | Yes (per setpoint) |
| Set pressure units | -- | Yes (`0x0E`) |
| Set pump size | -- | Yes (`0x12`) |
| Pump name readback | -- | Yes (`0xED`) |
| HV enabled query | -- | Yes (`0x61`) |
| Setpoint write error feedback | -- | Yes (via `@mismatch`) |
| Pump-off pressure sentinel | Yes (sets `9.9e9` when V<1000 and I<1e-6) | -- |
| Derived pressure from I and pump size | Yes | -- |
| Consecutive error tracking with alarm escalation | Yes | -- |

### Architecture

The **digitel record** packs all functionality into a single custom record type. One record instance handles all reads (11 commands sent sequentially in a single scan cycle) and all writes for one pump. All values are stored as fields within the record.

The **streamDevice database** uses 27 standard EPICS records per pump (ai, ao, bi, bo, stringin, mbbi, mbbo, calcout, scalcout). Each record independently sends its own command when scanned. This is more modular but creates more PVs per pump.

### When to Use Which

- Use the **digitel record** when you need tight integration with the existing ion pump GUI displays (Pump.adl, QPCsingle_pump.adl), alarm handling through the record's built-in alarm fields, or compatibility with systems already using the digitel record for MPC or Digitel 500/1500 controllers.

- Use the **streamDevice database** when you need features like pump name readback, pressure unit control, or pump size configuration; when you prefer standard EPICS record types over a custom record; or when you want independent scan rates for different parameters.
