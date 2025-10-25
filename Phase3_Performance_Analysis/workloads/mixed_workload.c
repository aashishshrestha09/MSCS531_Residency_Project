/**
 * Mixed Workload for Phase 3
 * Healthcare IoT Microprocessor Performance Analysis
 * 
 * This workload simulates realistic healthcare IoT operation with mixed activities:
 * - Continuous background monitoring
 * - Periodic ECG analysis bursts
 * - Intermittent data transmission
 * - Variable computational intensity
 * - Idle periods with power management
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define SIMULATION_CYCLES 30
#define BACKGROUND_MONITOR_FREQ 100   // Check sensors every 100 iterations
#define ECG_ANALYSIS_FREQ 500         // Analyze ECG every 500 iterations
#define TRANSMISSION_FREQ 1000        // Transmit every 1000 iterations

// Sensor types
typedef enum {
    SENSOR_HEART_RATE,
    SENSOR_SPO2,
    SENSOR_TEMPERATURE,
    SENSOR_BLOOD_PRESSURE,
    SENSOR_ACTIVITY,
    SENSOR_COUNT
} SensorType;

// Sensor reading structure
typedef struct {
    SensorType type;
    uint16_t value;
    uint32_t timestamp;
    uint8_t alert_flag;
} SensorReading;

// System state tracking
typedef struct {
    uint32_t monitoring_samples;
    uint32_t ecg_analyses;
    uint32_t transmissions;
    uint32_t alerts_triggered;
    uint32_t idle_periods;
    uint16_t current_power_mode; // 0=active, 1=idle, 2=sleep
} SystemState;

// Generate simulated sensor reading
SensorReading read_sensor(SensorType type, uint32_t timestamp) {
    SensorReading reading;
    reading.type = type;
    reading.timestamp = timestamp;
    reading.alert_flag = 0;
    
    switch (type) {
        case SENSOR_HEART_RATE:
            reading.value = 65 + (timestamp % 30);
            if (reading.value > 90 || reading.value < 50) {
                reading.alert_flag = 1;
            }
            break;
        case SENSOR_SPO2:
            reading.value = 94 + (timestamp % 6);
            if (reading.value < 95) {
                reading.alert_flag = 1;
            }
            break;
        case SENSOR_TEMPERATURE:
            reading.value = 365 + (timestamp % 15);
            if (reading.value > 375 || reading.value < 360) {
                reading.alert_flag = 1;
            }
            break;
        case SENSOR_BLOOD_PRESSURE:
            reading.value = 120 + (timestamp % 20);
            if (reading.value > 140 || reading.value < 90) {
                reading.alert_flag = 1;
            }
            break;
        case SENSOR_ACTIVITY:
            reading.value = timestamp % 100;
            break;
        default:
            reading.value = 0;
    }
    
    return reading;
}

// Background monitoring task (lightweight)
uint8_t background_monitoring(SensorReading *readings, uint32_t timestamp) {
    uint8_t alerts = 0;
    
    // Read all sensors
    for (SensorType s = 0; s < SENSOR_COUNT; s++) {
        readings[s] = read_sensor(s, timestamp);
        if (readings[s].alert_flag) {
            alerts++;
        }
    }
    
    return alerts;
}

// Simple moving average for trend detection
uint16_t calculate_moving_average(uint16_t *buffer, int size) {
    uint32_t sum = 0;
    for (int i = 0; i < size; i++) {
        sum += buffer[i];
    }
    return (uint16_t)(sum / size);
}

// ECG analysis task (computationally intensive)
void analyze_ecg_segment(uint16_t *ecg_data, int length, uint16_t *hr_out, uint8_t *anomaly_out) {
    // Simulate QRS detection algorithm
    uint16_t peak_count = 0;
    uint16_t threshold = 150;
    
    for (int i = 1; i < length - 1; i++) {
        // Detect peaks
        if (ecg_data[i] > ecg_data[i-1] && 
            ecg_data[i] > ecg_data[i+1] && 
            ecg_data[i] > threshold) {
            peak_count++;
        }
    }
    
    // Calculate heart rate (assuming 5-second window)
    *hr_out = peak_count * 12; // Convert to BPM
    
    // Check for anomalies
    *anomaly_out = (*hr_out > 100 || *hr_out < 50) ? 1 : 0;
}

// Data transmission task (moderate intensity with bursts)
uint16_t transmit_sensor_data(SensorReading *readings, int count) {
    uint16_t bytes_transmitted = 0;
    
    // Packetize sensor data
    uint8_t packet[128];
    int packet_idx = 0;
    
    for (int i = 0; i < count; i++) {
        packet[packet_idx++] = (readings[i].type & 0xFF);
        packet[packet_idx++] = (readings[i].value >> 8) & 0xFF;
        packet[packet_idx++] = readings[i].value & 0xFF;
        packet[packet_idx++] = readings[i].alert_flag;
        
        if (packet_idx >= 120) {
            // Transmit packet
            bytes_transmitted += packet_idx;
            packet_idx = 0;
            
            // Simulate transmission delay
            volatile uint32_t delay = 0;
            for (int j = 0; j < 500; j++) {
                delay += j;
            }
        }
    }
    
    // Transmit remaining data
    if (packet_idx > 0) {
        bytes_transmitted += packet_idx;
    }
    
    return bytes_transmitted;
}

// Idle period with minimal activity (enables power gating)
void idle_period(uint32_t duration) {
    volatile uint32_t counter = 0;
    for (uint32_t i = 0; i < duration; i++) {
        counter++;
        if ((counter % 100) == 0) {
            counter = 0;
        }
    }
}

int main() {
    printf("=== Mixed Workload Simulation ===\n");
    printf("Simulation Cycles: %d\n", SIMULATION_CYCLES);
    printf("Background Monitor Frequency: Every %d iterations\n", BACKGROUND_MONITOR_FREQ);
    printf("ECG Analysis Frequency: Every %d iterations\n", ECG_ANALYSIS_FREQ);
    printf("Transmission Frequency: Every %d iterations\n\n", TRANSMISSION_FREQ);
    
    SystemState state = {0, 0, 0, 0, 0, 0};
    SensorReading readings[SENSOR_COUNT];
    uint16_t ecg_buffer[360]; // 1-second ECG data at 360Hz
    uint16_t hr_history[10];
    int hr_idx = 0;
    
    // Initialize ECG buffer with baseline
    for (int i = 0; i < 360; i++) {
        ecg_buffer[i] = 1024 + (i % 50);
    }
    
    // Main simulation loop
    for (int cycle = 0; cycle < SIMULATION_CYCLES; cycle++) {
        uint32_t iteration = cycle * 1000; // Each cycle = 1000 iterations
        
        printf("--- Cycle %d/%d (t=%u) ---\n", cycle + 1, SIMULATION_CYCLES, iteration);
        
        // Determine workload mix for this cycle
        uint8_t do_monitoring = (iteration % BACKGROUND_MONITOR_FREQ) == 0;
        uint8_t do_ecg_analysis = (iteration % ECG_ANALYSIS_FREQ) == 0;
        uint8_t do_transmission = (iteration % TRANSMISSION_FREQ) == 0;
        
        if (do_monitoring) {
            // Background monitoring (frequent, lightweight)
            uint8_t alerts = background_monitoring(readings, iteration);
            state.monitoring_samples += SENSOR_COUNT;
            state.alerts_triggered += alerts;
            
            printf("  Monitoring: %d sensors, %d alerts\n", SENSOR_COUNT, alerts);
            state.current_power_mode = 0; // Active
        }
        
        if (do_ecg_analysis) {
            // ECG analysis (periodic, intensive)
            uint16_t heart_rate;
            uint8_t anomaly;
            
            // Generate new ECG segment
            for (int i = 0; i < 360; i++) {
                ecg_buffer[i] = 1024 + 200 * ((i % 60 < 10) ? 1 : 0) + (iteration % 10);
            }
            
            analyze_ecg_segment(ecg_buffer, 360, &heart_rate, &anomaly);
            hr_history[hr_idx++ % 10] = heart_rate;
            state.ecg_analyses++;
            
            uint16_t avg_hr = calculate_moving_average(hr_history, (hr_idx < 10) ? hr_idx : 10);
            printf("  ECG Analysis: HR=%u BPM (avg=%u), Anomaly=%s\n", 
                   heart_rate, avg_hr, anomaly ? "YES" : "NO");
            
            if (anomaly) {
                state.alerts_triggered++;
            }
            state.current_power_mode = 0; // Active
        }
        
        if (do_transmission) {
            // Data transmission (infrequent, burst activity)
            uint16_t bytes_sent = transmit_sensor_data(readings, SENSOR_COUNT);
            state.transmissions++;
            
            printf("  Transmission: %u bytes sent\n", bytes_sent);
            state.current_power_mode = 0; // Active
        }
        
        // If no activity, enter idle mode
        if (!do_monitoring && !do_ecg_analysis && !do_transmission) {
            printf("  Idle period (power saving)\n");
            idle_period(500);
            state.idle_periods++;
            state.current_power_mode = 2; // Sleep
        }
        
        // Small delay between cycles
        idle_period(100);
        
        printf("\n");
    }
    
    // Final statistics
    printf("=== Simulation Complete ===\n");
    printf("Total Monitoring Samples: %u\n", state.monitoring_samples);
    printf("Total ECG Analyses: %u\n", state.ecg_analyses);
    printf("Total Transmissions: %u\n", state.transmissions);
    printf("Total Alerts Triggered: %u\n", state.alerts_triggered);
    printf("Total Idle Periods: %u\n", state.idle_periods);
    printf("Active Duty Cycle: %.1f%%\n", 
           (float)(SIMULATION_CYCLES - state.idle_periods) / SIMULATION_CYCLES * 100.0);
    
    return 0;
}
