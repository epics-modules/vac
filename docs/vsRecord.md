---
layout: default
title: vs Record
nav_order: 8
---

# vs Record Reference
{: .no_toc}

## Table of contents
{: .no_toc .text-delta }

- TOC
{:toc}

The `vs` (vacuum sensor) record type supports vacuum gauge controllers. It handles ion gauge pressure readback, convectron gauge pressures, setpoint status and values, ion gauge on/off control, degas control, and alarm checking. The record communicates with hardware through the `devVacSen` device support driver using ASYN.

Device support types (DTYP): `devGP307` (GP307), `devGP350` (GP350), `devMM200` (Televac MM200), `devCC10` (Televac CC10), `devMX200` (Televac MX200).

## Configuration

| Field | Type | Description | Access |
|-------|------|-------------|--------|
| `INP` | INLINK | Device specification (ASYN port, address) | Config |
| `TYPE` | MENU | Controller type | Config |
| `PREC` | SHORT | Display precision | Config (default: 1) |
| `ERR` | SHORT | Controller error count | Read/Write (default: 5) |

### Controller Type Menu (TYPE)

| Value | String | Device |
|-------|--------|--------|
| 0 | GP307 | Granville-Phillips 307 |
| 1 | GP350 | Granville-Phillips 350 |
| 2 | MM200 | Televac MM200 |
| 3 | CC10 | Televac CC10 |
| 4 | MX200 | Televac MX200 |

## Ion Gauge Control

| Field | Type | Description | Access |
|-------|------|-------------|--------|
| `IG1S` | MENU | Ion gauge 1 set (command) | Read/Write (Off/On) |
| `IG1R` | MENU | Ion gauge 1 readback (status) | Read-only (Off/On) |
| `IG2S` | MENU | Ion gauge 2 set (command) | Read/Write (Off/On) |
| `IG2R` | MENU | Ion gauge 2 readback (status) | Read-only (Off/On) |
| `DGSS` | MENU | Degas set (command) | Read/Write (Off/On) |
| `DGSR` | MENU | Degas readback (status) | Read-only (Off/On) |
| `FLTR` | MENU | Fault readback | Read-only (Off/On) |

## Pressure Readback

### Linear Pressure

| Field | Type | Description | Access |
|-------|------|-------------|--------|
| `VAL` | DOUBLE | Ion gauge pressure | Read-only |
| `PRES` | DOUBLE | Ion gauge pressure (duplicate) | Read-only |
| `CGAP` | DOUBLE | Convectron gauge A pressure | Read-only |
| `CGBP` | DOUBLE | Convectron gauge B pressure | Read-only |

### Log10 Pressure

| Field | Type | Description | Access |
|-------|------|-------------|--------|
| `LPRS` | DOUBLE | Ion gauge log10 pressure | Read-only |
| `LCAP` | DOUBLE | Convectron A log10 pressure | Read-only |
| `LCBP` | DOUBLE | Convectron B log10 pressure | Read-only |

## Setpoint Status

| Field | Type | Description | Access |
|-------|------|-------------|--------|
| `SP1` | MENU | Setpoint 1 status | Read-only (Off/On) |
| `SP2` | MENU | Setpoint 2 status | Read-only (Off/On) |
| `SP3` | MENU | Setpoint 3 status | Read-only (Off/On) |
| `SP4` | MENU | Setpoint 4 status | Read-only (Off/On) |
| `SP5` | MENU | Setpoint 5 status | Read-only (Off/On) |
| `SP6` | MENU | Setpoint 6 status | Read-only (Off/On) |

## Setpoint Values

| Field | Type | Description | Access |
|-------|------|-------------|--------|
| `SP1S` | DOUBLE | Setpoint 1 value set | Read/Write |
| `SP1R` | DOUBLE | Setpoint 1 readback | Read-only |
| `SP2S` | DOUBLE | Setpoint 2 value set | Read/Write |
| `SP2R` | DOUBLE | Setpoint 2 readback | Read-only |
| `SP3S` | DOUBLE | Setpoint 3 value set | Read/Write |
| `SP3R` | DOUBLE | Setpoint 3 readback | Read-only |
| `SP4S` | DOUBLE | Setpoint 4 value set | Read/Write |
| `SP4R` | DOUBLE | Setpoint 4 readback | Read-only |

## Alarm Fields

The record checks the `VAL` field (ion gauge pressure) against these limits.

| Field | Type | Description | Default |
|-------|------|-------------|---------|
| `HIHI` | FLOAT | IG pressure hihi alarm limit | 1e-06 |
| `HIGH` | FLOAT | IG pressure high alarm limit | 1e-07 |
| `LOW` | FLOAT | IG pressure low alarm limit | 2e-12 |
| `LOLO` | FLOAT | IG pressure lolo alarm limit | 1e-12 |
| `HHSV` | MENU | Hihi alarm severity | NO_ALARM |
| `HSV` | MENU | High alarm severity | NO_ALARM |
| `LSV` | MENU | Low alarm severity | NO_ALARM |
| `LLSV` | MENU | Lolo alarm severity | NO_ALARM |
| `HYST` | DOUBLE | Alarm deadband | 0 |
| `LALM` | DOUBLE | Last value alarmed | (internal) |

## Display Range Fields

These fields set the display range for GUI screens.

### Ion Gauge

| Field | Type | Description | Default |
|-------|------|-------------|---------|
| `HOPR` | FLOAT | IG pressure display high | 0.0001 |
| `LOPR` | FLOAT | IG pressure display low | 1e-12 |
| `HLPR` | FLOAT | IG log10 pressure display high | -4 |
| `LLPR` | FLOAT | IG log10 pressure display low | -12 |

### Convectron Gauge A

| Field | Type | Description | Default |
|-------|------|-------------|---------|
| `HAPR` | FLOAT | CGA pressure display high | 1000 |
| `LAPR` | FLOAT | CGA pressure display low | 0.0001 |
| `HALR` | FLOAT | CGA log10 pressure display high | 3 |
| `LALR` | FLOAT | CGA log10 pressure display low | -4 |

### Convectron Gauge B

| Field | Type | Description | Default |
|-------|------|-------------|---------|
| `HBPR` | FLOAT | CGB pressure display high | 1000 |
| `LBPR` | FLOAT | CGB pressure display low | 0.0001 |
| `HBLR` | FLOAT | CGB log10 pressure display high | 3 |
| `LBLR` | FLOAT | CGB log10 pressure display low | -4 |

## Internal Fields

These fields are used internally by the record for change detection. They store previous values and should not be modified directly.

| Field | Type | Description |
|-------|------|-------------|
| `CHGC` | USHORT | Changed control flags |
| `PI1S`, `PI2S` | MENU | Previous ion gauge set commands |
| `PDSS` | MENU | Previous degas set command |
| `PIG1`, `PIG2` | MENU | Previous ion gauge readback |
| `PDGS` | MENU | Previous degas readback |
| `PFLT` | MENU | Previous fault readback |
| `PSP1`--`PSP6` | MENU | Previous setpoint status |
| `PS1S`--`PS4S` | DOUBLE | Previous setpoint set values |
| `PS1R`--`PS4R` | DOUBLE | Previous setpoint readbacks |
| `PVAL`, `PPRE` | DOUBLE | Previous gauge pressure |
| `PCGA`, `PCGB` | DOUBLE | Previous convectron pressures |
| `PLPE` | DOUBLE | Previous IG log10 pressure |
| `PLCA`, `PLCB` | DOUBLE | Previous convectron log10 pressures |
