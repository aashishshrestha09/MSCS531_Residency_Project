# Phase 3: Performance and Power Simulation Analysis

Healthcare IoT RISC-V Microprocessor - Complete Performance Analysis and Optimization

## Project Overview

This directory contains the complete Phase 3 implementation: performance and power simulation analysis of a healthcare IoT RISC-V microprocessor using gem5 simulator. The project systematically characterizes 36 experimental configurations across 6 workloads and 6 DVFS operating points, achieving 25-35% better efficiency than commercial processors.

## Quick Start

### Generate Performance Figures

```bash
source venv/bin/activate  # If using virtual environment
python analyze_results.py
```

This creates all 6 performance figures (300 DPI PNG format) from simulation results.

## Project Structure

```text
Phase3_Performance_Analysis/
├── Deliverable3_Report_APA7.docx      # Final report (Word format, APA 7)
├── PHASE3_REQUIREMENTS_CHECKLIST.md   # Complete requirements verification
├── README.md                          # This file
│
├── Python Scripts:
│   ├── analyze_results.py             # Generate 6 performance figures
│   └── requirements.txt               # Python dependencies
│
├── figures/                           # 6 PNG performance charts (300 DPI)
│   ├── figure1_dvfs_power_performance.png  (241KB)
│   ├── figure2_energy_efficiency.png       (111KB)
│   ├── figure3_ipc_comparison.png          (123KB)
│   ├── figure4_cache_performance.png       (130KB)
│   ├── figure5_power_breakdown.png         (136KB)
│   └── figure6_dvfs_efficiency.png         (130KB)
│
├── workloads/                         # 6 RISC-V C source files
│   ├── healthcare_monitor_test.c
│   ├── intensive_ecg_processing.c
│   ├── burst_transmission.c
│   ├── mixed_workload.c
│   ├── idle_scenario.c
│   └── stress_test.c
│
├── gem5_simulation_outputs/           # Sample gem5 statistics files
├── simulation_results/                # DVFS sweep results (JSON)
│   └── dvfs_sweep/
│       └── dvfs_sweep_results.json    # Complete 36-config results
└── venv/                              # Python virtual environment
```

## Experimental Design

### 36 Total Configurations

- **6 Workloads** × **6 DVFS Operating Points** = **36 Experiments**

### Six Healthcare Workloads

| Workload | Purpose | Key Features |
|----------|---------|--------------|
| **healthcare_monitor_test** | Baseline monitoring | 100Hz sensor sampling, anomaly detection |
| **intensive_ecg_processing** | Real-time ECG analysis | Pan-Tompkins QRS detection, R-R intervals |
| **burst_transmission** | IoT communication | 60-second idle periods, CRC, compression |
| **mixed_workload** | Realistic operation | Combined monitoring, ECG, transmission |
| **idle_scenario** | Power gating validation | Multiple sleep durations |
| **stress_test** | Performance bounds | Matrix operations, sorting, hashing |

### Six DVFS Operating Points

| Voltage | Frequency | Target Application | Power Budget |
|---------|-----------|-------------------|--------------|
| 0.6V | 50 MHz | Extended idle monitoring | < 100 mW |
| 0.7V | 100 MHz | Lightweight sampling | < 300 mW |
| 0.8V | 200 MHz | Moderate monitoring | < 800 mW |
| 0.9V | 400 MHz | Real-time processing | < 2.5 W |
| 1.0V | 600 MHz | Intensive ECG analysis | < 5 W |
| 1.2V | 800 MHz | Maximum load | < 10 W |

## Key Results

### Performance Metrics

- **Optimal Operating Point:** 0.9V/400MHz
- **Energy Efficiency:** 7.89 µJ/instruction (optimized)
- **IPC Range:** 0.499-0.928 across configurations
- **Cache Hit Rates:** >96% (all levels)
- **Power Range:** 98.5mW to 9758mW (98-fold dynamic range)

### Optimization Achievements

- **30% energy savings:** Adaptive DVFS
- **72% energy savings:** Predictive power gating (idle scenarios)
- **2-3% energy savings:** Compiler instruction scheduling
- **Combined:** 30-75% total savings depending on usage

### Battery Life

- **9.7 hours** continuous operation (40% improvement over baseline)
- **41 hours** for transmission-intensive scenarios with power gating

### Commercial Comparison

- **25-35% better efficiency** than Texas Instruments CC2640 and Nordic nRF52832
- Competitive with SiFive E31 and Pulp Platform

## Figures and Tables

### 10 Figures Total

1. gem5 Build Command and Output
2. Workload Compilation Commands (all 6)
3. gem5 Execution Command Template
4. gem5 Statistics at Optimal Point
5. Power-Performance Chart (PNG)
6. Energy Efficiency Curve (PNG)
7. IPC Comparison (PNG)
8. Cache Performance (PNG)
9. Power Breakdown (PNG)
10. Optimization Impact (PNG)

### 2 Tables

1. DVFS Operating Points Specification
2. Performance Metrics Across DVFS Points (optimized)

## System Configuration

### gem5 Simulator

- **Version:** gem5 (compiled from source for RISC-V)
- **CPU Model:** TimingSimpleCPU (5-stage in-order pipeline)
- **L1 I-Cache:** 32KB, 4-way set-associative
- **L1 D-Cache:** 32KB, 4-way set-associative
- **L2 Cache:** 128KB, 8-way set-associative
- **Memory:** 512MB DDR3-1600
- **ISA:** RISC-V RV64G

### Power Models

- **Dynamic Power:** P = CV²fα
- **Static Power:** P = VI_leak
- **Energy/Instruction:** E = P_avg × t_exec / N_inst

## Python Dependencies

```bash
# Install dependencies
pip install -r requirements.txt
```

**Required packages:**

- numpy >= 1.24.0
- matplotlib >= 3.7.0
- pandas >= 2.0.0
- scipy >= 1.10.0
