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

```text
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
├── Phase3_Performance_Analysis/
│   ├── Deliverable3_Report_APA7.docx
│   ├── PHASE3_REQUIREMENTS_CHECKLIST.md
│   ├── analyze_results.py
│   ├── requirements.txt
│   ├── figures/ (6 PNG files)
│   ├── workloads/ (6 C programs)
│   ├── gem5_simulation_outputs/
│   ├── simulation_results/
│   └── README.md
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
- **phase-3-performance**: Complete performance analysis and optimization (Current)

## Documentation

Detailed documentation is provided in the Word documents (submitted separately):

- **Phase 1:** Architecture definition, design rationale, and power management strategies
- **Phase 2:** Implementation methodology, validation results, and performance analysis
- **Phase 3:** Performance evaluation, DVFS characterization, and optimization strategies

## Phase 3: Performance Analysis and Optimization

**Comprehensive Analysis:**

- **Workloads:** 6 representative healthcare algorithms (sensor processing, filtering, ECG, data aggregation, encryption, FFT)
- **DVFS Sweep:** 36 configurations (6 workloads × 6 voltage-frequency points)
- **Metrics:** IPC, power consumption, energy efficiency, cache performance, execution time

**Key Findings:**

- **Optimal Point:** 0.9V/400MHz at 7.89 µJ/instruction
- **Performance:** 25-35% better energy efficiency than commercial processors
- **Battery Life:** 9.7 hours (40% improvement over baseline)
- **Bottlenecks:** L2 cache misses, I-cache performance, branch prediction
- **Optimizations:** 30-75% energy savings through targeted improvements

**Analysis Tools:** Python-based visualization (analyze_results.py) generating performance charts
