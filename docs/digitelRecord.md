---
layout: default
title: digitel Record
nav_order: 7
---

# digitel Record Reference
{: .no_toc}

## Table of contents
{: .no_toc .text-delta }

- TOC
{:toc}

The `digitel` record type supports ion pump controllers. It handles pressure readback, voltage, current, operating mode control, up to four setpoints, bakeout control, cooldown, pump identification, and alarm checking. The record communicates with hardware through the `devDigitelPump` device support driver using ASYN.

Device support types (DTYP): `devDigitel` (MPC), `devDigitelD500` (Digitel 500), `devDigitelD1500` (Digitel 1500), `devDigitelQPC` (QPC).

## Device Identity

| Field | Type | Description | Access |
|-------|------|-------------|--------|
| `INP` | INLINK | Device specification (ASYN port, address) | Config |
| `TYPE` | MENU | Controller type | Config |
| `MODL` | STRING(13) | Device model number (MPC/QPC only) | Read-only |
| `VERS` | STRING(10) | Device firmware version (MPC/QPC only) | Read-only |

### Controller Type Menu (TYPE)

| Value | String | Device |
|-------|--------|--------|
| 0 | MPC | MPC, MPC-II, LPC |
| 1 | D500 | Digitel 500 |
| 2 | D1500 | Digitel 1500 |
| 3 | QPC | QPC, QPCe, SPCe |

## Pressure and Electrical Readback

| Field | Type | Description | Access |
|-------|------|-------------|--------|
| `VAL` | DOUBLE | Pressure (linear) | Read-only |
| `LVAL` | DOUBLE | Pressure (log10 form) | Read-only |
| `CRNT` | DOUBLE | Current | Read-only |
| `VOLT` | DOUBLE | Voltage | Read-only |
| `TONL` | ULONG | Time online | Read-only |
| `ACCW` | DOUBLE | Accumulated power | Read-only |
| `ACCI` | DOUBLE | Accumulated current | Read-only |

## Operating Controls

| Field | Type | Description | Access | Menu Values |
|-------|------|-------------|--------|-------------|
| `DSPL` | MENU | Display mode | Read/Write | VOLTS, CURR, PRES |
| `KLCK` | MENU | Keyboard lock | Read/Write | Unlocked, Locked |
| `MODS` | MENU | Mode set | Read/Write | STBY, OPER |
| `MODR` | MENU | Mode readback | Read-only | STBY, OPER, CONN, COOL, PERR, LOCK |
| `BAKS` | MENU | Bake set | Read/Write | Disabled, Enabled |
| `BAKR` | MENU | Bake readback | Read-only | Disabled, Enabled |
| `BKIN` | MENU | Bake installed | Read-only | Absent, Installed |
| `COOL` | DOUBLE | Cooldown time | Read-only | |
| `CMOR` | MENU | Cooldown mode | Read-only | Off, On |

### Mode Readback Values (MODR)

| Value | String | Meaning |
|-------|--------|---------|
| 0 | STBY | Standby |
| 1 | OPER | Operating |
| 2 | CONN | Connecting |
| 3 | COOL | Cooldown |
| 4 | PERR | Pump Error |
| 5 | LOCK | Locked |

## Pump Type

| Field | Type | Description | Access |
|-------|------|-------------|--------|
| `PTYP` | MENU | Pump type/size | Read-only |

### Pump Type Menu (PTYP)

| Value | String |
|-------|--------|
| 0 | 30 Liter/sec |
| 1 | 60 Liter/sec |
| 2 | 120 Liter/sec |
| 3 | 220 Liter/sec |
| 4 | 400 Liter/sec |
| 5 | 700 Liter/sec |
| 6 | 1200 Liter/sec |

## Setpoint 1

| Field | Type | Description | Access |
|-------|------|-------------|--------|
| `SET1` | MENU | Setpoint 1 status | Read-only (Off/On) |
| `SP1S` | DOUBLE | SP1 setpoint value | Read/Write |
| `SP1R` | DOUBLE | SP1 readback | Read-only |
| `S1HS` | DOUBLE | SP1 hysteresis set | Read/Write |
| `S1HR` | DOUBLE | SP1 hysteresis readback | Read-only |
| `S1MS` | MENU | SP1 mode | Read/Write (Pressure/Current) |
| `S1MR` | MENU | SP1 mode readback | Read-only (Pressure/Current) |
| `S1VS` | MENU | SP1 HV interlock | Read/Write (Off/On) |
| `S1VR` | MENU | SP1 HV interlock readback | Read-only (Off/On) |

## Setpoint 2

| Field | Type | Description | Access |
|-------|------|-------------|--------|
| `SET2` | MENU | Setpoint 2 status | Read-only (Off/On) |
| `SP2S` | DOUBLE | SP2 setpoint value | Read/Write |
| `SP2R` | DOUBLE | SP2 readback | Read-only |
| `S2HS` | DOUBLE | SP2 hysteresis set | Read/Write |
| `S2HR` | DOUBLE | SP2 hysteresis readback | Read-only |
| `S2MS` | MENU | SP2 mode | Read/Write (Pressure/Current) |
| `S2MR` | MENU | SP2 mode readback | Read-only (Pressure/Current) |
| `S2VS` | MENU | SP2 HV interlock | Read/Write (Off/On) |
| `S2VR` | MENU | SP2 HV interlock readback | Read-only (Off/On) |

## Setpoint 3

| Field | Type | Description | Access |
|-------|------|-------------|--------|
| `SET3` | MENU | Setpoint 3 status | Read-only (Off/On) |
| `SP3S` | DOUBLE | SP3 setpoint value | Read/Write |
| `SP3R` | DOUBLE | SP3 readback | Read-only |
| `S3HS` | DOUBLE | SP3 hysteresis set | Read/Write |
| `S3HR` | DOUBLE | SP3 hysteresis readback | Read-only |
| `S3MS` | MENU | SP3 mode | Read/Write (Pressure/Current) |
| `S3MR` | MENU | SP3 mode readback | Read-only (Pressure/Current) |
| `S3VS` | MENU | SP3 HV interlock | Read/Write (Off/On) |
| `S3VR` | MENU | SP3 HV interlock readback | Read-only (Off/On) |

### Setpoint 3 Bake Time

| Field | Type | Description | Access |
|-------|------|-------------|--------|
| `S3BS` | MENU | Bake time mode set | Read/Write (Real Time / Heat On Time) |
| `S3BR` | MENU | Bake time mode readback | Read-only (Real Time / Heat On Time) |
| `S3TS` | DOUBLE | Bake time set | Read/Write |
| `S3TR` | DOUBLE | Bake time readback | Read-only |

## Setpoint 4

Setpoint 4 was added for the QPC device.

| Field | Type | Description | Access |
|-------|------|-------------|--------|
| `SET4` | MENU | Setpoint 4 status | Read-only (Off/On) |
| `SP4S` | DOUBLE | SP4 setpoint value | Read/Write |
| `SP4R` | DOUBLE | SP4 readback | Read-only |
| `S4HS` | DOUBLE | SP4 hysteresis set | Read/Write |
| `S4HR` | DOUBLE | SP4 hysteresis readback | Read-only |
| `S4MS` | MENU | SP4 mode | Read/Write (Pressure/Current) |
| `S4MR` | MENU | SP4 mode readback | Read-only (Pressure/Current) |
| `S4VS` | MENU | SP4 HV interlock | Read/Write (Off/On) |
| `S4VR` | MENU | SP4 HV interlock readback | Read-only (Off/On) |

## Alarm Fields

Standard pressure alarm fields. The record checks the `VAL` field against these limits.

| Field | Type | Description | Default |
|-------|------|-------------|---------|
| `HIHI` | DOUBLE | Pressure hihi alarm limit | 1e-06 |
| `HIGH` | DOUBLE | Pressure high alarm limit | 1e-07 |
| `LOW` | DOUBLE | Pressure low alarm limit | 2e-12 |
| `LOLO` | DOUBLE | Pressure lolo alarm limit | 1e-12 |
| `HHSV` | MENU | Hihi alarm severity | NO_ALARM |
| `HSV` | MENU | High alarm severity | NO_ALARM |
| `LSV` | MENU | Low alarm severity | NO_ALARM |
| `LLSV` | MENU | Lolo alarm severity | NO_ALARM |
| `HYST` | DOUBLE | Alarm deadband | 0 |
| `LALM` | DOUBLE | Last value alarmed | (internal) |

## Display Range Fields

These fields set the display range for GUI screens.

| Field | Type | Description | Default |
|-------|------|-------------|---------|
| `HOPR` | FLOAT | Pressure display high | 0.0001 |
| `LOPR` | FLOAT | Pressure display low | 1e-11 |
| `HCTR` | FLOAT | Current display high | 0.5 |
| `LCTR` | FLOAT | Current display low | 1e-09 |
| `HVTR` | FLOAT | Voltage display high | 7000 |
| `LVTR` | FLOAT | Voltage display low | 0 |
| `HLPR` | FLOAT | Log pressure display high | -4 |
| `LLPR` | FLOAT | Log pressure display low | -11 |

## Simulation Fields

| Field | Type | Description |
|-------|------|-------------|
| `SIML` | INLINK | Sim mode location |
| `SIMM` | MENU | Sim mode value (Yes/No) |
| `SLMO` | INLINK | Sim location for mode |
| `SVMO` | MENU | Sim value for mode |
| `SLS1` | INLINK | Sim location for SP1 |
| `SVS1` | MENU | Sim value for SP1 |
| `SLS2` | INLINK | Sim location for SP2 |
| `SVS2` | MENU | Sim value for SP2 |
| `SLCR` | INLINK | Sim location for current |
| `SVCR` | DOUBLE | Sim value for current |

## Internal Fields

These fields are used internally by the record and device support for change detection and initialization. They should not be modified directly.

| Field | Type | Description |
|-------|------|-------------|
| `FLGS` | ULONG | Modification flags (bitmask for pending writes) |
| `SPFG` | ULONG | Setpoint modification flags |
| `CYCL` | LONG | Cycle count |
| `ERR` | SHORT | Error count |
| `IVAL`, `ILVA` | DOUBLE | Init shadow for pressure |
| `IMOD` | MENU | Init shadow for mode |
| `IBAK` | MENU | Init shadow for bake |
| `ICOL` | DOUBLE | Init shadow for cooldown |
| `ISP1`--`ISP4` | MENU | Init shadow for setpoint status |
| `IS1`--`IS4` | DOUBLE | Init shadow for setpoint values |
| `IH1`--`IH4` | DOUBLE | Init shadow for hysteresis |
| `IM1`--`IM4` | MENU | Init shadow for setpoint modes |
| `II1`--`II4` | MENU | Init shadow for HV interlock |
| `IB3` | MENU | Init shadow for SP3 bake time mode |
| `IT3` | DOUBLE | Init shadow for SP3 bake time |
| `ITON` | ULONG | Init shadow for time online |
| `ICRN` | DOUBLE | Init shadow for current |
| `IVOL` | DOUBLE | Init shadow for voltage |
| `IACW` | DOUBLE | Init shadow for accumulated power |
| `IACI` | DOUBLE | Init shadow for accumulated current |
| `IPTY` | MENU | Init shadow for pump type |
| `IBKN` | MENU | Init shadow for bake installed |
| `IERR` | SHORT | Init shadow for error count |

## Debug Variable

The `recDigitelDebug` variable can be set from the iocsh to enable debug output:

```
var recDigitelDebug 1
```
