# ESP32 ↔ AT9036E over SPI — Energy Management System

This project demonstrates a clean, reproducible implementation of SPI communication between an ESP32 and the AT9036E energy metering IC. It focuses on the crucial step of reading 2‑byte measurement registers over SPI, extracting bytes correctly, applying scaling (÷100) to obtain decimal values, and exposing meaningful metrics: frequency, voltage, temperature, and power. The project targets building Energy Management System (EMS) use cases.

## Key Features
- ESP-IDF (v5.x) based SPI master driver on ESP32
- Deterministic 2‑byte register reads from AT9036E
- Byte extraction and unit scaling (value ÷ 100)
- Metrics: Frequency, Voltage, Temperature, Power
- VS Code + PowerShell workflow on Windows
- Ready to integrate into a broader EMS application

## System Overview
- Microcontroller: ESP32
- Metering IC: AT9036E (SPI interface)
- Transport: SPI (Mode, Clock, and Polarity per AT9036E datasheet)
- Output Data: Raw 16‑bit register values converted to decimal by dividing by 100

## Hardware
- ESP32 Dev Board (any with SPI pins exposed)
- AT9036E-based metering board/module
- SPI wiring (example — adjust for your board and PCB layout):
  - MOSI → AT9036E SDI
  - MISO → AT9036E SDO
  - SCLK → AT9036E SCLK
  - CS   → AT9036E CS (active low)
  - GND  ↔ GND, 3V3/5V as per the AT9036E module requirements (check datasheet)

ESP32 default SPI host used: VSPI (FSPI on some chips). Pins can be customized in code/menuconfig.

## SPI Transaction Model
1) Configure SPI host and device:
   - Host: VSPI with your chosen GPIO pins
   - Device: SPI mode and clock per AT9036E timing (consult datasheet)
2) Register read command phase (address/command format as per AT9036E)
3) Read 2 bytes
4) Extract high/low bytes → 16‑bit value
5) Convert to decimal by dividing by 100

Example scaling:
- Raw register: 0x1324 = 4900 → 4900 / 100 = 49.00 units
- The unit depends on the register (e.g., V, Hz, °C, W). Use the AT9036E register map.

## Data Points
- Frequency: 2‑byte register → Hz = raw/100
- Voltage: 2‑byte register → V = raw/100
- Temperature: 2‑byte register → °C = raw/100
- Power: 2‑byte register → W = raw/100

Note: Register addresses and exact units depend on the AT9036E datasheet. Adjust addresses and scaling constants as required for your board calibration.

## Repository Structure
```
EspIDF/
├─ CMakeLists.txt          # Top-level CMake
├─ main/
│  ├─ CMakeLists.txt       # Component CMake
│  └─ main.c               # SPI init + 2-byte read + scaling pipeline
├─ sdkconfig               # Project configuration
└─ README.md               # Project documentation (this file)
```

## Build and Run (Windows + VS Code PowerShell)
1) Open VS Code Terminal (PowerShell)
2) Initialize ESP-IDF in the SAME terminal session:
   - Per-session export:
     ```powershell
     & "C:\Espressif\frameworks\esp-idf-v5.5.1\export.ps1"
     ```
   - Or global init script:
     ```powershell
     C:\Espressif\idf_cmd_init.ps1
     ```
3) Go to the project:
   ```powershell
   cd "C:\Users\navee\OneDrive\Documents\Personal Projects\esp-idf\examples\EspIDF"
   ```
4) Verify tools and target:
   ```powershell
   idf.py --version
   idf.py set-target esp32
   ```
5) Build:
   ```powershell
   idf.py build
   ```
6) Flash & Monitor (replace COMx):
   ```powershell
   idf.py -p COMx flash monitor
   ```
   - Exit monitor: `Ctrl+]`

## Implementation Notes (Portfolio Focus)
- SPI configuration
  - Host: VSPI
  - Mode: per AT9036E timing (e.g., Mode 0 or 1 — check datasheet)
  - Clock: select within device max (balance stability vs. throughput)
  - Queue size: 1–4 is sufficient for periodic reads
- 2‑byte read sequence
  - Compose a read frame with the register address and R/W bit as specified
  - Transmit address/command, then read two data bytes
- Byte extraction
  - Combine: `uint16_t raw = ((uint16_t)hi << 8) | lo;`
- Scaling to engineering units
  - `float value = raw / 100.0f;`
  - Apply per-register unit semantics
- Scheduling
  - Periodic polling timer/task for frequency, voltage, temperature, power
  - Debounce/median filter if needed to stabilize readings

## Extending the Project
- Add a component for AT9036E registers and helpers (address map, scaling)
- Abstract SPI device layer for easier unit testing and mocking
- Add calibration constants via Kconfig (menuconfig)
- Output data via UART/JSON, MQTT, or REST to a building EMS backend

## Troubleshooting
- idf.py not found: run the export/init script in the same terminal session
- SPI returns zeros/0xFFFF: verify CS wiring, mode, and register address format
- Noisy readings: lower SPI clock, add decoupling, add averaging filter
- Wrong units: confirm scaling (÷100) and specific register semantics from datasheet

## Roadmap
- Componentize AT9036E driver with register map and unit tests
- Add multi-register burst reads for efficiency
- Implement data publishing (MQTT/REST) for EMS dashboards
- Integrate NVS for calibration and device provisioning

## License
For learning and portfolio use. Add an explicit license (e.g., MIT) if distributing.

## Author
Naveed — Embedded/IoT developer
- Focus: ESP32, SPI drivers, energy metering, EMS
- Tools: ESP-IDF, VS Code, C, CMake, PowerShell
