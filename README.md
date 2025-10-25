# MSCS531 Residency Project: Low-Power Microprocessor for Healthcare IoT

## Project Overview

This project focuses on the design and implementation of a low-power RISC-V microprocessor optimized for healthcare IoT patient monitoring devices. The implementation uses the gem5 simulation framework to model, validate, and analyze the architectural design.

### Target Application
Wearable patient monitoring device for continuous tracking of vital signs including:
- Heart rate monitoring
- Blood pressure measurement
- Blood oxygen saturation (SpO2)
- Real-time data processing and transmission

## Repository Structure

```
MSCS531_Residency_Project/
├── Phase1_Architecture_Design/
│   └── Deliverable1_Architecture_Definition_Design.docx (submitted separately)
├── Phase2_Implementation/
│   ├── Deliverable2_Implementation_Guide.docx (submitted separately)
│   ├── gem5_configs/
│   │   └── healthcare_iot_config.py
│   ├── workloads/
│   │   └── healthcare_monitor_test.c
│   └── simulation_outputs/
│       ├── build_output.txt
│       ├── gem5_version.txt
│       ├── hello_test_output.txt
│       ├── simulation_output.txt
│       └── stats.txt
└── README.md
```

## Phase 1: Architecture Definition and Design

**Key Features:**
- RISC-V RV32I instruction set architecture
- 5-stage in-order pipeline (fetch, decode, execute, memory, write-back)
- Two-level cache hierarchy:
  - L1 I-cache: 32 KB, 4-way set associative
  - L1 D-cache: 32 KB, 4-way set associative
  - L2 unified: 128 KB, 8-way set associative
- Power management techniques:
  - Dynamic Voltage and Frequency Scaling (DVFS)
  - Hierarchical clock gating
  - Strategic power gating
  - Sleep modes for idle peripherals

## Phase 2: Implementation Using gem5

**Implementation Highlights:**
- **Processor:** MinorCPU model (5-stage in-order pipeline)
- **ISA:** RISC-V RV32I with multiply and atomic extensions
- **Power Management:**
  - DVFS: 6 operating points (0.6V/50MHz to 1.2V/800MHz)
  - Clock gating for pipeline stages and cache levels
  - Power gating for floating-point unit and L2 cache banks
- **Build Environment:** Containerized using Podman/Docker
- **Validation:** Healthcare monitoring test workload

**Performance Results:**
- **IPC:** 0.805 instructions per cycle
- **Cache Performance:**
  - I-cache hit rate: 95.43%
  - D-cache hit rate: 96.56%
  - L2 miss rate: 25.8%
- **Power Consumption:**
  - Active: ~150 mW @ 1.2V/800MHz
  - Idle: <10 mW with clock/power gating
  - DVFS power savings: 40-50%

## Getting Started

### Prerequisites
- gem5 simulator (v24.0+)
- RISC-V toolchain (riscv64-unknown-elf-gcc)
- Python 3.8+
- Podman or Docker (for containerized build)

### Building gem5

```bash
# Clone gem5 repository
git clone https://github.com/gem5/gem5.git
cd gem5

# Build with RISC-V support
scons build/RISCV/gem5.opt -j$(nproc)
```

### Running Simulations

```bash
# Compile test workload
riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -O2 -static \
    Phase2_Implementation/workloads/healthcare_monitor_test.c \
    -o healthcare_monitor_test

# Run simulation
./build/RISCV/gem5.opt \
    Phase2_Implementation/gem5_configs/healthcare_iot_config.py \
    --cmd=healthcare_monitor_test
```

## Project Tags

- **phase-2-implementation**: Complete gem5 implementation and validation (October 25, 2025)

## Documentation

Detailed documentation is provided in the Word documents (submitted separately):
- **Phase 1:** Architecture definition, design rationale, and power management strategies
- **Phase 2:** Implementation methodology, validation results, and performance analysis

## Future Work (Phase 3)

- Extended performance analysis across varied healthcare workloads
- Detailed power consumption profiling
- DVFS effectiveness characterization
- Architectural optimization opportunities
- Multi-core extensions for enhanced throughput
