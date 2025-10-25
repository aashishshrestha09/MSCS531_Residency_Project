/**
 * Stress Test Workload for Phase 3
 * Healthcare IoT Microprocessor Performance Analysis
 * 
 * This workload tests worst-case scenarios and system limits:
 * - Maximum computational load
 * - Cache thrashing patterns
 * - Maximum memory bandwidth utilization
 * - Sustained peak power consumption
 * - Thermal stress conditions
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define STRESS_TEST_DURATION 40
#define MATRIX_SIZE 32              // 32x32 matrix operations
#define ARRAY_SIZE 4096             // Large array for cache stress
#define HASH_TABLE_SIZE 512         // Hash operations

typedef struct {
    uint32_t total_operations;
    uint32_t matrix_multiplications;
    uint32_t array_sorts;
    uint32_t hash_operations;
    uint32_t memory_accesses;
} StressTestMetrics;

// Matrix multiplication (computationally intensive)
void matrix_multiply(int16_t A[][MATRIX_SIZE], int16_t B[][MATRIX_SIZE], int16_t C[][MATRIX_SIZE]) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            int32_t sum = 0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = (int16_t)(sum & 0xFFFF);
        }
    }
}

// Bubble sort (cache stress with many memory accesses)
void bubble_sort(uint16_t *array, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (array[j] > array[j + 1]) {
                uint16_t temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

// Hash function for stress testing
uint32_t hash_function(uint32_t key) {
    key = ((key >> 16) ^ key) * 0x45d9f3b;
    key = ((key >> 16) ^ key) * 0x45d9f3b;
    key = (key >> 16) ^ key;
    return key;
}

// Hash table operations (pointer chasing, cache misses)
uint32_t hash_table_stress(uint32_t *hash_table, int size, uint32_t iterations) {
    uint32_t checksum = 0;
    
    for (uint32_t i = 0; i < iterations; i++) {
        uint32_t key = i;
        uint32_t hash = hash_function(key) % size;
        
        // Simulate hash table operations
        hash_table[hash] ^= key;
        checksum += hash_table[hash];
        
        // Chain lookups (pointer chasing)
        uint32_t next_hash = hash_table[hash] % size;
        checksum ^= hash_table[next_hash];
    }
    
    return checksum;
}

// Memory bandwidth stress (large data transfers)
void memory_bandwidth_stress(uint8_t *src, uint8_t *dst, int size) {
    // Block copy with processing
    for (int i = 0; i < size; i += 4) {
        uint32_t data = *(uint32_t *)&src[i];
        data = (data << 1) | (data >> 31); // Rotate
        *(uint32_t *)&dst[i] = data;
    }
}

// Random memory access pattern (worst case for cache)
uint32_t random_memory_access(uint16_t *array, int size, uint32_t iterations) {
    uint32_t checksum = 0;
    uint32_t index = 0;
    
    for (uint32_t i = 0; i < iterations; i++) {
        // Pseudo-random index generation
        index = (index * 1103515245 + 12345) % size;
        checksum += array[index];
        array[index] = (uint16_t)(checksum & 0xFFFF);
    }
    
    return checksum;
}

// Recursive function stress (stack usage)
uint32_t fibonacci_recursive(uint32_t n) {
    if (n <= 1) return n;
    return fibonacci_recursive(n - 1) + fibonacci_recursive(n - 2);
}

// Combined stress test
void combined_stress_iteration(StressTestMetrics *metrics) {
    // Allocate large data structures
    static int16_t matrixA[MATRIX_SIZE][MATRIX_SIZE];
    static int16_t matrixB[MATRIX_SIZE][MATRIX_SIZE];
    static int16_t matrixC[MATRIX_SIZE][MATRIX_SIZE];
    static uint16_t sort_array[ARRAY_SIZE];
    static uint32_t hash_table[HASH_TABLE_SIZE];
    static uint8_t mem_src[ARRAY_SIZE];
    static uint8_t mem_dst[ARRAY_SIZE];
    
    // Initialize matrices with pseudo-random data
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrixA[i][j] = (i * j) & 0xFF;
            matrixB[i][j] = (i + j) & 0xFF;
        }
    }
    
    // Initialize arrays
    for (int i = 0; i < ARRAY_SIZE; i++) {
        sort_array[i] = (ARRAY_SIZE - i) & 0xFFFF;
        mem_src[i] = i & 0xFF;
    }
    
    // Initialize hash table
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hash_table[i] = i;
    }
    
    // Task 1: Matrix multiplication (ALU intensive)
    matrix_multiply(matrixA, matrixB, matrixC);
    metrics->matrix_multiplications++;
    metrics->total_operations += MATRIX_SIZE * MATRIX_SIZE * MATRIX_SIZE;
    
    // Task 2: Sorting (memory intensive, many comparisons)
    bubble_sort(sort_array, ARRAY_SIZE / 4); // Partial sort to save time
    metrics->array_sorts++;
    metrics->memory_accesses += ARRAY_SIZE * ARRAY_SIZE / 16;
    
    // Task 3: Hash operations (cache thrashing)
    uint32_t hash_checksum = hash_table_stress(hash_table, HASH_TABLE_SIZE, 1000);
    metrics->hash_operations += 1000;
    metrics->total_operations += 1000;
    
    // Task 4: Memory bandwidth stress
    memory_bandwidth_stress(mem_src, mem_dst, ARRAY_SIZE);
    metrics->memory_accesses += ARRAY_SIZE;
    
    // Task 5: Random access pattern (worst-case cache)
    uint32_t random_checksum = random_memory_access(sort_array, ARRAY_SIZE, 500);
    metrics->memory_accesses += 500;
    
    // Task 6: Recursive computation (stack stress)
    uint32_t fib_result = fibonacci_recursive(15);
    
    // Verify results to prevent optimization elimination
    volatile uint32_t verification = matrixC[0][0] + hash_checksum + random_checksum + fib_result;
}

int main() {
    printf("=== Stress Test Workload ===\n");
    printf("Duration: %d iterations\n", STRESS_TEST_DURATION);
    printf("Matrix Size: %dx%d\n", MATRIX_SIZE, MATRIX_SIZE);
    printf("Array Size: %d elements\n", ARRAY_SIZE);
    printf("Hash Table Size: %d entries\n\n", HASH_TABLE_SIZE);
    
    printf("WARNING: This test generates maximum computational load\n");
    printf("         and sustained peak power consumption.\n\n");
    
    StressTestMetrics metrics = {0, 0, 0, 0, 0};
    
    printf("Starting stress test...\n\n");
    
    for (int iteration = 0; iteration < STRESS_TEST_DURATION; iteration++) {
        printf("Iteration %d/%d: Running combined stress...\n", 
               iteration + 1, STRESS_TEST_DURATION);
        
        combined_stress_iteration(&metrics);
        
        if ((iteration + 1) % 10 == 0) {
            printf("  Progress: %d%% complete\n", 
                   (iteration + 1) * 100 / STRESS_TEST_DURATION);
            printf("  Total Operations: %u\n", metrics.total_operations);
            printf("  Memory Accesses: %u\n\n", metrics.memory_accesses);
        }
    }
    
    printf("\n=== Stress Test Complete ===\n");
    printf("Total Operations: %u\n", metrics.total_operations);
    printf("Matrix Multiplications: %u\n", metrics.matrix_multiplications);
    printf("Array Sorts: %u\n", metrics.array_sorts);
    printf("Hash Operations: %u\n", metrics.hash_operations);
    printf("Memory Accesses: %u\n", metrics.memory_accesses);
    printf("Average Operations per Iteration: %u\n", 
           metrics.total_operations / STRESS_TEST_DURATION);
    printf("Average Memory Accesses per Iteration: %u\n", 
           metrics.memory_accesses / STRESS_TEST_DURATION);
    
    printf("\nThis workload validates:\n");
    printf("  - Sustained peak computational load\n");
    printf("  - Cache behavior under stress\n");
    printf("  - Memory bandwidth limits\n");
    printf("  - Maximum power consumption\n");
    printf("  - Thermal management capability\n");
    
    return 0;
}
