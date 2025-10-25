/**
 * Idle Power Analysis Workload for Phase 3
 * Healthcare IoT Microprocessor Performance Analysis
 * 
 * This workload validates low-power modes and power gating effectiveness.
 * Simulates extended idle periods with minimal activity to measure:
 * - Clock gating efficiency
 * - Power gating effectiveness
 * - Wake-up latency from sleep modes
 * - Idle power consumption
 */

#include <stdio.h>
#include <stdint.h>

#define IDLE_TEST_CYCLES 50
#define SHORT_IDLE_DURATION 1000     // Light sleep
#define MEDIUM_IDLE_DURATION 5000    // Deep sleep candidate
#define LONG_IDLE_DURATION 10000     // Deep sleep with power gating
#define WATCHDOG_CHECK_INTERVAL 100  // Minimal activity

typedef struct {
    uint32_t short_idle_count;
    uint32_t medium_idle_count;
    uint32_t long_idle_count;
    uint32_t watchdog_checks;
    uint32_t total_idle_iterations;
} IdleStats;

// Minimal watchdog function (allows clock gating)
uint8_t watchdog_check(uint32_t iteration) {
    volatile uint8_t status = 0;
    status = (iteration & 0x01);
    return status;
}

// Short idle period (frequent wake-ups)
void short_idle_sleep(uint32_t duration) {
    volatile uint32_t counter = 0;
    
    for (uint32_t i = 0; i < duration; i++) {
        counter++;
        
        // Minimal watchdog activity every 100 iterations
        if ((i % WATCHDOG_CHECK_INTERVAL) == 0) {
            watchdog_check(i);
        }
    }
}

// Medium idle period (infrequent wake-ups)
void medium_idle_sleep(uint32_t duration) {
    volatile uint32_t counter = 0;
    
    for (uint32_t i = 0; i < duration; i++) {
        counter++;
        
        // Very infrequent watchdog checks
        if ((i % (WATCHDOG_CHECK_INTERVAL * 5)) == 0) {
            watchdog_check(i);
        }
    }
}

// Long idle period (deep sleep, maximum power gating)
void long_idle_sleep(uint32_t duration) {
    volatile uint32_t counter = 0;
    
    // Minimal activity loop - allows aggressive power gating
    for (uint32_t i = 0; i < duration; i++) {
        counter = (counter + 1) & 0xFF;
    }
}

// Simulate wake-up from idle (measure latency impact)
uint16_t wake_up_sequence() {
    volatile uint16_t wake_latency = 0;
    
    // Restore state simulation
    for (int i = 0; i < 50; i++) {
        wake_latency += i;
    }
    
    return wake_latency;
}

int main() {
    printf("=== Idle Power Analysis Workload ===\n");
    printf("Test Cycles: %d\n", IDLE_TEST_CYCLES);
    printf("Short Idle Duration: %d iterations\n", SHORT_IDLE_DURATION);
    printf("Medium Idle Duration: %d iterations\n", MEDIUM_IDLE_DURATION);
    printf("Long Idle Duration: %d iterations\n\n", LONG_IDLE_DURATION);
    
    IdleStats stats = {0, 0, 0, 0, 0};
    
    printf("Beginning idle power analysis...\n\n");
    
    for (int cycle = 0; cycle < IDLE_TEST_CYCLES; cycle++) {
        // Vary idle patterns to test different power states
        int pattern = cycle % 6;
        
        switch (pattern) {
            case 0:
            case 1:
                // Short idle (common case)
                printf("Cycle %d: Short idle (%d iter)...\n", cycle + 1, SHORT_IDLE_DURATION);
                short_idle_sleep(SHORT_IDLE_DURATION);
                stats.short_idle_count++;
                stats.total_idle_iterations += SHORT_IDLE_DURATION;
                stats.watchdog_checks += SHORT_IDLE_DURATION / WATCHDOG_CHECK_INTERVAL;
                break;
                
            case 2:
            case 3:
                // Medium idle (moderate sleep)
                printf("Cycle %d: Medium idle (%d iter)...\n", cycle + 1, MEDIUM_IDLE_DURATION);
                medium_idle_sleep(MEDIUM_IDLE_DURATION);
                stats.medium_idle_count++;
                stats.total_idle_iterations += MEDIUM_IDLE_DURATION;
                stats.watchdog_checks += MEDIUM_IDLE_DURATION / (WATCHDOG_CHECK_INTERVAL * 5);
                break;
                
            case 4:
            case 5:
                // Long idle (deep sleep with power gating)
                printf("Cycle %d: Long idle (%d iter)...\n", cycle + 1, LONG_IDLE_DURATION);
                long_idle_sleep(LONG_IDLE_DURATION);
                stats.long_idle_count++;
                stats.total_idle_iterations += LONG_IDLE_DURATION;
                break;
        }
        
        // Wake up and measure latency
        uint16_t wake_latency = wake_up_sequence();
        
        if ((cycle + 1) % 10 == 0) {
            printf("  Wake latency: %u cycles\n", wake_latency);
        }
    }
    
    printf("\n=== Idle Power Analysis Complete ===\n");
    printf("Short Idle Periods: %u\n", stats.short_idle_count);
    printf("Medium Idle Periods: %u\n", stats.medium_idle_count);
    printf("Long Idle Periods: %u\n", stats.long_idle_count);
    printf("Total Idle Iterations: %u\n", stats.total_idle_iterations);
    printf("Watchdog Checks: %u\n", stats.watchdog_checks);
    printf("Average Iterations per Cycle: %u\n", 
           stats.total_idle_iterations / IDLE_TEST_CYCLES);
    
    printf("\nIdle Pattern Distribution:\n");
    printf("  Short: %.1f%%\n", (float)stats.short_idle_count / IDLE_TEST_CYCLES * 100.0);
    printf("  Medium: %.1f%%\n", (float)stats.medium_idle_count / IDLE_TEST_CYCLES * 100.0);
    printf("  Long: %.1f%%\n", (float)stats.long_idle_count / IDLE_TEST_CYCLES * 100.0);
    
    return 0;
}
