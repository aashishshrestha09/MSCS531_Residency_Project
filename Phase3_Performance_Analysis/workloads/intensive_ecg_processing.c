/**
 * Intensive ECG Processing Workload for Phase 3
 * Healthcare IoT Microprocessor Performance Analysis
 * 
 * This workload simulates continuous ECG waveform analysis including:
 * - Real-time QRS complex detection
 * - Heart rate variability calculation
 * - R-R interval measurement
 * - Arrhythmia pattern detection
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define SAMPLING_RATE 360        // Hz (standard ECG sampling rate)
#define BUFFER_SIZE 2048         // Samples
#define QRS_THRESHOLD 150        // mV threshold for QRS detection
#define RR_INTERVAL_MIN 240      // ms (250 BPM max)
#define RR_INTERVAL_MAX 2000     // ms (30 BPM min)
#define NUM_ITERATIONS 50        // Extended processing cycles

// ECG data structure
typedef struct {
    uint16_t amplitude;          // Signal amplitude in mV
    uint32_t timestamp;          // Timestamp in ms
    uint8_t qrs_detected;        // QRS complex flag
} ECGSample;

// Heart rate metrics
typedef struct {
    uint16_t heart_rate;         // BPM
    uint16_t rr_interval;        // ms
    uint16_t hrv_sdnn;          // Heart rate variability (SDNN)
    uint8_t arrhythmia_flag;    // Abnormal rhythm detected
} HeartMetrics;

// Simulate ECG signal generation (sin wave + noise)
uint16_t generate_ecg_sample(uint32_t sample_index) {
    // Base ECG signal: simulate cardiac cycle
    double t = (double)sample_index / SAMPLING_RATE;
    double signal = 100.0 * sin(2.0 * 3.14159 * 1.2 * t); // 72 BPM base
    
    // Add QRS complex spike every ~0.83 seconds (72 BPM)
    if ((sample_index % 300) < 10) {
        signal += 200.0; // QRS spike
    }
    
    // Add small noise
    signal += (sample_index % 7) - 3;
    
    return (uint16_t)(signal + 1024); // Offset to positive range
}

// Moving average filter for noise reduction
uint16_t moving_average_filter(uint16_t *buffer, int start, int window_size) {
    uint32_t sum = 0;
    for (int i = 0; i < window_size; i++) {
        sum += buffer[(start + i) % BUFFER_SIZE];
    }
    return (uint16_t)(sum / window_size);
}

// Derivative filter for QRS detection (emphasizes slope changes)
int16_t derivative_filter(uint16_t *buffer, int index) {
    int idx_curr = index % BUFFER_SIZE;
    int idx_prev = (index - 1 + BUFFER_SIZE) % BUFFER_SIZE;
    return (int16_t)(buffer[idx_curr] - buffer[idx_prev]);
}

// Detect QRS complex using derivative and threshold
uint8_t detect_qrs_complex(uint16_t *ecg_buffer, int index) {
    // Apply derivative filter
    int16_t derivative = derivative_filter(ecg_buffer, index);
    
    // Square to emphasize high slopes
    int32_t squared = derivative * derivative;
    
    // Check against threshold
    if (squared > (QRS_THRESHOLD * QRS_THRESHOLD)) {
        return 1; // QRS detected
    }
    return 0;
}

// Calculate R-R interval between consecutive QRS complexes
uint16_t calculate_rr_interval(uint32_t *qrs_timestamps, int count) {
    if (count < 2) return 0;
    
    uint32_t interval = qrs_timestamps[count - 1] - qrs_timestamps[count - 2];
    return (uint16_t)interval;
}

// Calculate heart rate from R-R interval
uint16_t calculate_heart_rate(uint16_t rr_interval) {
    if (rr_interval == 0) return 0;
    return (uint16_t)(60000 / rr_interval); // BPM
}

// Calculate heart rate variability (SDNN - standard deviation of NN intervals)
uint16_t calculate_hrv_sdnn(uint16_t *rr_intervals, int count) {
    if (count < 2) return 0;
    
    // Calculate mean
    uint32_t sum = 0;
    for (int i = 0; i < count; i++) {
        sum += rr_intervals[i];
    }
    uint16_t mean = sum / count;
    
    // Calculate variance
    uint32_t variance_sum = 0;
    for (int i = 0; i < count; i++) {
        int32_t diff = rr_intervals[i] - mean;
        variance_sum += diff * diff;
    }
    
    // Return standard deviation approximation
    return (uint16_t)(variance_sum / count);
}

// Detect arrhythmia based on RR interval irregularity
uint8_t detect_arrhythmia(uint16_t *rr_intervals, int count) {
    if (count < 3) return 0;
    
    // Check for irregular intervals (>20% variation)
    for (int i = 1; i < count; i++) {
        int32_t diff = abs(rr_intervals[i] - rr_intervals[i-1]);
        if (diff > (rr_intervals[i-1] / 5)) {
            return 1; // Irregular rhythm detected
        }
    }
    return 0;
}

// Process ECG data and extract heart metrics
void process_ecg_batch(ECGSample *samples, int sample_count, HeartMetrics *metrics) {
    uint16_t ecg_buffer[BUFFER_SIZE];
    uint32_t qrs_timestamps[100];
    uint16_t rr_intervals[100];
    int qrs_count = 0;
    int rr_count = 0;
    
    // Process each sample
    for (int i = 0; i < sample_count; i++) {
        ecg_buffer[i % BUFFER_SIZE] = samples[i].amplitude;
        
        // Apply filtering and QRS detection
        if (i >= 10) { // Need buffer history
            uint16_t filtered = moving_average_filter(ecg_buffer, i - 5, 5);
            samples[i].qrs_detected = detect_qrs_complex(ecg_buffer, i);
            
            if (samples[i].qrs_detected && qrs_count < 100) {
                qrs_timestamps[qrs_count++] = samples[i].timestamp;
                
                // Calculate RR interval
                if (qrs_count >= 2 && rr_count < 100) {
                    rr_intervals[rr_count++] = calculate_rr_interval(qrs_timestamps, qrs_count);
                }
            }
        }
    }
    
    // Calculate final metrics
    if (rr_count > 0) {
        metrics->rr_interval = rr_intervals[rr_count - 1];
        metrics->heart_rate = calculate_heart_rate(metrics->rr_interval);
        metrics->hrv_sdnn = calculate_hrv_sdnn(rr_intervals, rr_count);
        metrics->arrhythmia_flag = detect_arrhythmia(rr_intervals, rr_count);
    }
}

int main() {
    printf("=== Intensive ECG Processing Workload ===\n");
    printf("Sampling Rate: %d Hz\n", SAMPLING_RATE);
    printf("Buffer Size: %d samples\n", BUFFER_SIZE);
    printf("Processing Iterations: %d\n\n", NUM_ITERATIONS);
    
    ECGSample *samples = (ECGSample *)malloc(BUFFER_SIZE * sizeof(ECGSample));
    HeartMetrics metrics = {0, 0, 0, 0};
    
    uint32_t total_qrs_detected = 0;
    uint32_t total_arrhythmias = 0;
    
    // Main processing loop
    for (int iteration = 0; iteration < NUM_ITERATIONS; iteration++) {
        // Generate ECG samples
        for (int i = 0; i < BUFFER_SIZE; i++) {
            samples[i].amplitude = generate_ecg_sample(iteration * BUFFER_SIZE + i);
            samples[i].timestamp = (iteration * BUFFER_SIZE + i) * (1000 / SAMPLING_RATE);
            samples[i].qrs_detected = 0;
        }
        
        // Process batch
        process_ecg_batch(samples, BUFFER_SIZE, &metrics);
        
        // Count detections
        for (int i = 0; i < BUFFER_SIZE; i++) {
            if (samples[i].qrs_detected) {
                total_qrs_detected++;
            }
        }
        
        if (metrics.arrhythmia_flag) {
            total_arrhythmias++;
        }
        
        // Report every 10 iterations
        if ((iteration + 1) % 10 == 0) {
            printf("Iteration %d/%d: HR=%d BPM, RR=%d ms, HRV=%d, Arrhythmia=%s\n",
                   iteration + 1, NUM_ITERATIONS,
                   metrics.heart_rate, metrics.rr_interval, metrics.hrv_sdnn,
                   metrics.arrhythmia_flag ? "DETECTED" : "Normal");
        }
    }
    
    printf("\n=== Processing Complete ===\n");
    printf("Total QRS complexes detected: %u\n", total_qrs_detected);
    printf("Total arrhythmias detected: %u\n", total_arrhythmias);
    printf("Final Heart Rate: %u BPM\n", metrics.heart_rate);
    printf("Final RR Interval: %u ms\n", metrics.rr_interval);
    printf("Heart Rate Variability (SDNN): %u\n", metrics.hrv_sdnn);
    
    free(samples);
    return 0;
}
