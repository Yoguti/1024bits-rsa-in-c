#ifndef ENTROPY_COLLECTOR_H
#define ENTROPY_COLLECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

// Structure to store entropy data
struct entropy_data {
    uint64_t user_time;
    uint64_t system_time;
    uint64_t idle_time;
};

// Global variable to store entropy data
extern struct entropy_data entropy;

// Failure flag for fallback
extern int failure;

// Function declarations
int get_failure();
void set_failure(int value);
void fallback();
void cpu_noise();
unsigned long long int get_clock_jitter();
unsigned long long int get_hardware_counter();
void thermal_entropy(unsigned long long *entropy_value);
void system_load_entropy(unsigned long long *entropy_value);
unsigned long long int collect_entropy();
unsigned char* seed_rng(size_t* seed_size);

#endif // ENTROPY_COLLECTOR_H
