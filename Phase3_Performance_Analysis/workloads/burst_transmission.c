/**
 * Burst Data Transmission Workload for Phase 3
 * Healthcare IoT Microprocessor Performance Analysis
 * 
 * This workload simulates periodic burst transmission patterns typical of
 * healthcare IoT devices transmitting vital signs data to cloud servers.
 * Tests power consumption during:
 * - Idle monitoring periods
 * - Data burst preparation (compression, packetization)
 * - Transmission bursts (high activity)
 * - Post-transmission idle recovery
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define TRANSMISSION_CYCLES 20       // Number of burst cycles
#define IDLE_DURATION 5000           // Idle loop iterations between bursts
#define BURST_PACKET_SIZE 256        // Bytes per packet
#define PACKETS_PER_BURST 8          // Packets per transmission
#define CRC_POLYNOMIAL 0x1021        // CRC-16-CCITT

// Sensor data packet structure
typedef struct {
    uint8_t device_id;
    uint8_t sensor_type;
    uint16_t timestamp;
    uint16_t heart_rate;
    uint16_t spo2;
    uint16_t temperature;
    uint32_t crc;
} SensorDataPacket;

// Transmission statistics
typedef struct {
    uint32_t total_bytes_sent;
    uint32_t total_packets_sent;
    uint32_t total_idle_cycles;
    uint32_t total_burst_cycles;
    uint16_t avg_packet_size;
} TransmissionStats;

// Generate simulated sensor data
SensorDataPacket generate_sensor_data(uint16_t sequence) {
    SensorDataPacket packet;
    packet.device_id = 0x42;
    packet.sensor_type = 0x01; // Vital signs sensor
    packet.timestamp = sequence * 100;
    
    // Simulate physiological variations
    packet.heart_rate = 70 + (sequence % 20);
    packet.spo2 = 95 + (sequence % 5);
    packet.temperature = 365 + (sequence % 10);
    
    packet.crc = 0; // Will be calculated
    return packet;
}

// CRC-16-CCITT calculation for data integrity
uint16_t calculate_crc16(uint8_t *data, uint16_t length) {
    uint16_t crc = 0xFFFF;
    
    for (uint16_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ CRC_POLYNOMIAL;
            } else {
                crc <<= 1;
            }
        }
    }
    
    return crc;
}

// Simple run-length encoding for data compression
uint16_t compress_data(uint8_t *input, uint16_t input_len, uint8_t *output, uint16_t max_output_len) {
    uint16_t out_idx = 0;
    uint16_t in_idx = 0;
    
    while (in_idx < input_len && out_idx < max_output_len - 2) {
        uint8_t current = input[in_idx];
        uint8_t count = 1;
        
        // Count consecutive identical bytes
        while (in_idx + count < input_len && 
               input[in_idx + count] == current && 
               count < 255) {
            count++;
        }
        
        output[out_idx++] = count;
        output[out_idx++] = current;
        in_idx += count;
    }
    
    return out_idx;
}

// Packetize data into fixed-size transmission units
uint16_t packetize_data(uint8_t *data, uint16_t data_len, uint8_t *packet_buffer, uint16_t packet_size) {
    uint16_t packets_created = 0;
    uint16_t offset = 0;
    
    while (offset < data_len) {
        uint16_t chunk_size = (data_len - offset) < packet_size ? (data_len - offset) : packet_size;
        
        // Copy data chunk to packet buffer
        for (uint16_t i = 0; i < chunk_size; i++) {
            packet_buffer[packets_created * packet_size + i] = data[offset + i];
        }
        
        // Pad remaining packet space
        for (uint16_t i = chunk_size; i < packet_size; i++) {
            packet_buffer[packets_created * packet_size + i] = 0x00;
        }
        
        offset += chunk_size;
        packets_created++;
    }
    
    return packets_created;
}

// Simulate transmission with error checking
uint8_t transmit_packet(uint8_t *packet, uint16_t size, uint16_t packet_num) {
    // Calculate CRC for packet
    uint16_t crc = calculate_crc16(packet, size - 2);
    packet[size - 2] = (crc >> 8) & 0xFF;
    packet[size - 1] = crc & 0xFF;
    
    // Simulate transmission delay (busy-wait)
    volatile uint32_t delay = 0;
    for (uint32_t i = 0; i < 1000; i++) {
        delay += i;
    }
    
    // Simulate successful transmission (99% success rate)
    if ((packet_num % 100) != 42) {
        return 1; // Success
    }
    return 0; // Failure
}

// Idle monitoring with minimal activity
void idle_monitoring_period(uint32_t duration) {
    volatile uint32_t idle_work = 0;
    
    // Light-weight idle loop (allows clock/power gating)
    for (uint32_t i = 0; i < duration; i++) {
        idle_work = (idle_work + 1) & 0xFFFF;
        
        // Occasional watchdog check
        if ((i % 1000) == 0) {
            idle_work ^= 0xAAAA;
        }
    }
}

int main() {
    printf("=== Burst Data Transmission Workload ===\n");
    printf("Transmission Cycles: %d\n", TRANSMISSION_CYCLES);
    printf("Idle Duration: %d iterations\n", IDLE_DURATION);
    printf("Packets per Burst: %d\n", PACKETS_PER_BURST);
    printf("Packet Size: %d bytes\n\n", BURST_PACKET_SIZE);
    
    TransmissionStats stats = {0, 0, 0, 0, 0};
    uint8_t *raw_buffer = (uint8_t *)malloc(BURST_PACKET_SIZE * PACKETS_PER_BURST);
    uint8_t *compressed_buffer = (uint8_t *)malloc(BURST_PACKET_SIZE * PACKETS_PER_BURST);
    uint8_t *packet_buffer = (uint8_t *)malloc(BURST_PACKET_SIZE * PACKETS_PER_BURST);
    
    // Main transmission cycle loop
    for (int cycle = 0; cycle < TRANSMISSION_CYCLES; cycle++) {
        printf("--- Cycle %d/%d ---\n", cycle + 1, TRANSMISSION_CYCLES);
        
        // Phase 1: Idle monitoring period (low power)
        printf("  Phase 1: Idle monitoring...\n");
        idle_monitoring_period(IDLE_DURATION);
        stats.total_idle_cycles += IDLE_DURATION;
        
        // Phase 2: Generate sensor data burst
        printf("  Phase 2: Generating sensor data...\n");
        uint16_t raw_data_len = 0;
        for (int i = 0; i < PACKETS_PER_BURST; i++) {
            SensorDataPacket sensor_data = generate_sensor_data(cycle * PACKETS_PER_BURST + i);
            uint8_t *data_ptr = (uint8_t *)&sensor_data;
            for (uint16_t j = 0; j < sizeof(SensorDataPacket); j++) {
                raw_buffer[raw_data_len++] = data_ptr[j];
            }
        }
        
        // Phase 3: Compress data
        printf("  Phase 3: Compressing data...\n");
        uint16_t compressed_len = compress_data(raw_buffer, raw_data_len, 
                                                 compressed_buffer, BURST_PACKET_SIZE * PACKETS_PER_BURST);
        float compression_ratio = (float)raw_data_len / compressed_len;
        printf("  Compression: %d -> %d bytes (%.2fx)\n", raw_data_len, compressed_len, compression_ratio);
        
        // Phase 4: Packetize compressed data
        printf("  Phase 4: Packetizing...\n");
        uint16_t num_packets = packetize_data(compressed_buffer, compressed_len, 
                                               packet_buffer, BURST_PACKET_SIZE);
        
        // Phase 5: Transmit packets (high activity burst)
        printf("  Phase 5: Transmitting %d packets...\n", num_packets);
        uint16_t successful_transmissions = 0;
        for (uint16_t i = 0; i < num_packets; i++) {
            if (transmit_packet(&packet_buffer[i * BURST_PACKET_SIZE], 
                               BURST_PACKET_SIZE, i)) {
                successful_transmissions++;
                stats.total_bytes_sent += BURST_PACKET_SIZE;
                stats.total_packets_sent++;
            }
        }
        stats.total_burst_cycles++;
        
        printf("  Transmitted: %d/%d packets (%.1f%% success)\n", 
               successful_transmissions, num_packets,
               (float)successful_transmissions / num_packets * 100.0);
        
        // Phase 6: Brief post-transmission idle
        idle_monitoring_period(IDLE_DURATION / 2);
        stats.total_idle_cycles += IDLE_DURATION / 2;
        
        printf("\n");
    }
    
    // Final statistics
    stats.avg_packet_size = (stats.total_packets_sent > 0) ? 
                            (stats.total_bytes_sent / stats.total_packets_sent) : 0;
    
    printf("=== Transmission Complete ===\n");
    printf("Total Bytes Transmitted: %u\n", stats.total_bytes_sent);
    printf("Total Packets Sent: %u\n", stats.total_packets_sent);
    printf("Average Packet Size: %u bytes\n", stats.avg_packet_size);
    printf("Total Idle Cycles: %u\n", stats.total_idle_cycles);
    printf("Total Burst Cycles: %u\n", stats.total_burst_cycles);
    printf("Idle/Active Ratio: %.2f\n", 
           (float)stats.total_idle_cycles / (stats.total_idle_cycles + stats.total_burst_cycles * 1000));
    
    free(raw_buffer);
    free(compressed_buffer);
    free(packet_buffer);
    
    return 0;
}
