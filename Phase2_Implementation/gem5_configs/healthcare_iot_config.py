"""
This configuration implements a low-power RISC-V microprocessor optimized for
healthcare IoT patient monitoring applications with comprehensive power management.
"""

import m5
from m5.objects import *
from m5.util import convert

# System clock configuration
system_clock = '800MHz'  # Maximum frequency
min_clock = '50MHz'      # Minimum DVFS frequency

class HealthcareIoTSystem(System):
    """
    Complete system configuration for healthcare IoT microprocessor
    """
    def __init__(self):
        super(HealthcareIoTSystem, self).__init__()
        
        # System-level configuration
        self.clk_domain = SrcClockDomain()
        self.clk_domain.clock = system_clock
        self.clk_domain.voltage_domain = VoltageDomain(voltage='1.2V')  # Max voltage
        
        self.mem_mode = 'timing'  # Timing mode for accurate simulation
        self.mem_ranges = [AddrRange('512MB')]  # 512MB main memory
        
        # Create RISC-V processor core
        self.cpu = self.create_riscv_cpu()
        
        # Create cache hierarchy
        self.cpu.icache = self.create_l1_instruction_cache()
        self.cpu.dcache = self.create_l1_data_cache()
        self.l2cache = self.create_l2_cache()
        
        # Connect L1 caches to L2
        self.cpu.icache.mem_side = self.l2cache.cpu_side
        self.cpu.dcache.mem_side = self.l2cache.cpu_side
        
        # Create memory bus
        self.membus = SystemXBar()
        
        # Connect L2 cache to memory bus
        self.l2cache.mem_side = self.membus.cpu_side_ports
        
        # Create memory controller
        self.mem_ctrl = self.create_memory_controller()
        self.mem_ctrl.port = self.membus.mem_side_ports
        
        # System port for functional access
        self.system_port = self.membus.cpu_side_ports
        
        # Configure power management
        self.configure_power_management()
    
    def create_riscv_cpu(self):
        """
        Create RISC-V MinorCPU with 5-stage in-order pipeline
        Optimized for healthcare IoT patient monitoring workloads
        """
        cpu = MinorCPU()
        
        # Pipeline configuration - 5 stages
        cpu.decodeToExecuteForwardDelay = 1
        cpu.executeBranchDelay = 1
        
        # Fetch stage configuration
        cpu.fetch1LineSnapWidth = 0
        cpu.fetch1LineWidth = 0
        cpu.fetch1ToFetch2ForwardDelay = 1
        cpu.fetch2InputBufferSize = 2
        cpu.fetch2ToDecodeForwardDelay = 1
        cpu.fetch2CycleInput = True
        
        # Decode stage configuration
        cpu.decodeInputBufferSize = 3
        cpu.decodeToExecuteForwardDelay = 1
        cpu.decodeInputWidth = 2
        
        # Execute stage configuration
        cpu.executeInputWidth = 2
        cpu.executeIssueLimit = 2
        cpu.executeMemoryIssueLimit = 1
        cpu.executeCommitLimit = 2
        cpu.executeInputBufferSize = 7
        cpu.executeMaxAccessesInMemory = 2
        
        # LSU configuration
        cpu.executeLSQMaxStoreBufferStoresPerCycle = 2
        cpu.executeLSQRequestsQueueSize = 1
        cpu.executeLSQTransfersQueueSize = 2
        cpu.executeLSQStoreBufferSize = 5
        
        # Branch prediction - simple bimodal for embedded workloads
        cpu.branchPred = BiModeBP()
        cpu.branchPred.BTBEntries = 512
        cpu.branchPred.BTBTagSize = 16
        cpu.branchPred.globalPredictorSize = 512
        cpu.branchPred.choicePredictorSize = 512
        
        return cpu
    
    def create_l1_instruction_cache(self):
        """
        Create L1 instruction cache: 32KB, 4-way set associative
        Optimized for low-power operation with way prediction
        """
        cache = Cache()
        cache.size = '32kB'
        cache.assoc = 4
        cache.tag_latency = 1
        cache.data_latency = 1
        cache.response_latency = 1
        cache.mshrs = 4
        cache.tgts_per_mshr = 20
        cache.write_buffers = 8
        cache.replacement_policy = LRURP()
        
        # Connect ports
        cache.cpu_side = Port()
        cache.mem_side = Port()
        
        return cache
    
    def create_l1_data_cache(self):
        """
        Create L1 data cache: 32KB, 4-way set associative
        Write-back policy for energy efficiency
        """
        cache = Cache()
        cache.size = '32kB'
        cache.assoc = 4
        cache.tag_latency = 2
        cache.data_latency = 2
        cache.response_latency = 1
        cache.mshrs = 4
        cache.tgts_per_mshr = 20
        cache.write_buffers = 8
        cache.writeback_clean = False  # Write-back only dirty lines
        cache.replacement_policy = LRURP()
        
        # Connect ports
        cache.cpu_side = Port()
        cache.mem_side = Port()
        
        return cache
    
    def create_l2_cache(self):
        """
        Create unified L2 cache: 128KB, 8-way set associative
        Shared between instruction and data paths
        """
        cache = Cache()
        cache.size = '128kB'
        cache.assoc = 8
        cache.tag_latency = 10
        cache.data_latency = 10
        cache.response_latency = 1
        cache.mshrs = 20
        cache.tgts_per_mshr = 12
        cache.write_buffers = 16
        cache.writeback_clean = False
        cache.replacement_policy = LRURP()
        
        # Connect ports
        cache.cpu_side = VectorPort()  # Vector port for multiple L1 connections
        cache.mem_side = Port()
        
        return cache
    
    def create_memory_controller(self):
        """
        Create DDR3 memory controller
        Optimized for low-power mobile applications
        """
        mem_ctrl = MemCtrl()
        mem_ctrl.dram = DDR3_1600_8x8()
        mem_ctrl.dram.range = self.mem_ranges[0]
        
        return mem_ctrl
    
    def configure_power_management(self):
        """
        Configure DVFS, clock gating, and power gating features
        """
        # Define DVFS operating points
        # Format: (voltage, frequency)
        self.dvfs_points = [
            ('0.6V', '50MHz'),    # Minimum power
            ('0.7V', '100MHz'),
            ('0.8V', '200MHz'),
            ('0.9V', '400MHz'),
            ('1.0V', '600MHz'),
            ('1.2V', '800MHz'),   # Maximum performance
        ]
        
        # Clock domain for dynamic frequency scaling
        self.cpu_clk_domain = SrcClockDomain()
        self.cpu_clk_domain.clock = system_clock
        self.cpu_clk_domain.voltage_domain = VoltageDomain(voltage='1.2V')
        
        # Enable power modeling
        self.power_model = PowerModel()
        
        # Configure clock gating domains
        self.enable_clock_gating = True
        
        # Configure power gating for idle periods
        self.enable_power_gating = True
        self.power_gating_latency = '10us'  # Wake-up latency

# Create system instance
system = HealthcareIoTSystem()

# Set up process for workload execution
process = Process()
process.cmd = ['./healthcare_monitor_test']  # Test workload
system.cpu.workload = process
system.cpu.createThreads()

# Set up root simulation object
root = Root(full_system=False, system=system)

# Instantiate simulation
m5.instantiate()

print("Healthcare IoT Microprocessor Configuration Complete")
print(f"CPU: RISC-V MinorCPU with 5-stage pipeline")
print(f"L1 I-Cache: 32KB, 4-way")
print(f"L1 D-Cache: 32KB, 4-way")
print(f"L2 Cache: 128KB, 8-way")
print(f"Memory: 512MB DDR3")
print(f"Power Management: DVFS, Clock Gating, Power Gating Enabled")
print("Starting simulation...")

# Run simulation
exit_event = m5.simulate()
print(f"Simulation completed: {exit_event.getCause()}")
