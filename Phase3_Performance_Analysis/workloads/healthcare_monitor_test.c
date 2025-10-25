/**
 * This program simulates basic patient monitoring tasks to validate
 * the gem5 implementation and test power management features.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define SAMPLE_RATE 256      // Samples per second
#define BUFFER_SIZE 1024     // Sample buffer size
#define NUM_ITERATIONS 10    // Number of processing cycles

// Simulated sensor data structure
typedef struct {
    uint16_t heart_rate;         // BPM
    uint16_t blood_pressure_sys; // mmHg
    uint16_t blood_pressure_dia; // mmHg
    uint16_t oxygen_saturation;  // Percentage
    uint32_t timestamp;          // Milliseconds
} PatientData;

// Simple moving average filter (common in signal processing)
uint16_t moving_average(uint16_t *buffer, int size) {
    uint32_t sum = 0;
    for (int i = 0; i < size; i++) {
        sum += buffer[i];
    }
    return (uint16_t)(sum / size);
}

// Detect anomaly in heart rate
int detect_anomaly(uint16_t heart_rate) {
    // Normal resting heart rate: 60-100 BPM
    if (heart_rate < 40 || heart_rate > 120) {
        return 1;  // Anomaly detected
    }
    return 0;
}

// Simulate data acquisition and processing
void process_patient_data() {
    PatientData samples[BUFFER_SIZE];
    uint16_t hr_buffer[16];  // Heart rate filter buffer
    int anomaly_count = 0;
    
    printf("Starting patient monitoring simulation...\n");
    
    for (int iter = 0; iter < NUM_ITERATIONS; iter++) {
        printf("Iteration %d: Acquiring sensor data...\n", iter + 1);
        
        // Simulate sensor data acquisition
        for (int i = 0; i < BUFFER_SIZE; i++) {
            // Generate pseudo-random but realistic vital signs
            samples[i].heart_rate = 70 + (rand() % 20);  // 70-90 BPM
            samples[i].blood_pressure_sys = 110 + (rand() % 20);  // 110-130 mmHg
            samples[i].blood_pressure_dia = 70 + (rand() % 15);   // 70-85 mmHg
            samples[i].oxygen_saturation = 95 + (rand() % 5);     // 95-100%
            samples[i].timestamp = iter * 1000 + i;
        }
        
        // Process data - apply filtering
        for (int i = 0; i < BUFFER_SIZE - 16; i++) {
            // Fill filter buffer
            for (int j = 0; j < 16; j++) {
                hr_buffer[j] = samples[i + j].heart_rate;
            }
            
            // Apply moving average filter
            uint16_t filtered_hr = moving_average(hr_buffer, 16);
            
            // Detect anomalies
            if (detect_anomaly(filtered_hr)) {
                anomaly_count++;
            }
        }
        
        // Simulate idle period between measurements (tests clock/power gating)
        printf("Iteration %d complete. Entering idle state...\n", iter + 1);
        
        // In real system, would enter low-power mode here
        // For testing, just a brief computation pause
        for (volatile int j = 0; j < 1000; j++);
    }
    
    printf("\nMonitoring complete!\n");
    printf("Total samples processed: %d\n", BUFFER_SIZE * NUM_ITERATIONS);
    printf("Anomalies detected: %d\n", anomaly_count);
}

int main() {
    printf("========================================\n");
    printf("Healthcare IoT Patient Monitoring Test\n");
    printf("MSCS-531 Residency Project - Phase 2\n");
    printf("========================================\n\n");
    
    process_patient_data();
    
    printf("\nTest workload completed successfully!\n");
    return 0;
}
