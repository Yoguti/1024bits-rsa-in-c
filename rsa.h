#ifndef GENERATE_VALUES_H
#define GENERATE_VALUES_H

#include <gmp.h>
#include "entropy_generator.h"

// Define a named struct to hold the RSA values
typedef struct {
    mpz_t q, p, n, d, totient, e;
} rsa_values_t;  // Naming this struct

// Declare an extern instance of this struct
extern rsa_values_t values; // Now this refers to the named struct, not an anonymous one

// Function to generate RSA values (fills the struct with the necessary values)
void generate(void);

// Internal functions

// Function to perform modular exponentiation
void mpz_mod_exp(mpz_t result, const mpz_t base, const mpz_t exp, const mpz_t mod);

// Function to concatenate hash values and return a GMP integer
mpz_t* concatenate_hashes(void);

// Function to generate a prime candidate based on some criteria
mpz_t* generate_candidate(mpz_t* prime_candidate);

// Fermat primality test
int fermat_test(mpz_t* adress_of_number);

// Miller-Rabin primality test
int miller_rabin_test(mpz_t* adress_of_number, int rounds);

void check_malloc(void* ptr);

void init_rsa_values();

#endif // GENERATE_VALUES_H
