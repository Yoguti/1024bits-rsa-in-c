#include "rsa.h"
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <string.h>

// Function to check if memory allocation was successful
void check_malloc(void* ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
}

rsa_values_t values;  // Use the named struct here

void init_rsa_values() {
    mpz_init(values.p);
    mpz_init(values.q);
    mpz_init(values.n);
    mpz_init(values.d);
    mpz_init(values.totient);
    mpz_init(values.e);
}

mpz_t* concatenate_hashes() {
    size_t seed_size = SHA256_DIGEST_LENGTH; // 32 bytes for SHA-256 output

    // Get the generated seed hashes
    unsigned char* hash1 = seed_rng(&seed_size);
    check_malloc(hash1);
    unsigned char* hash2 = seed_rng(&seed_size);
    check_malloc(hash2);
    unsigned char* hash3 = seed_rng(&seed_size);
    check_malloc(hash3);
    unsigned char* hash4 = seed_rng(&seed_size);
    check_malloc(hash4);

    // Allocate memory for the concatenated result (4 * SHA256_DIGEST_LENGTH bytes)
    unsigned char* concatenated_result = (unsigned char*)malloc(4 * SHA256_DIGEST_LENGTH);
    check_malloc(concatenated_result);

    // Concatenate them into the result array
    memcpy(concatenated_result, hash1, SHA256_DIGEST_LENGTH);
    memcpy(concatenated_result + SHA256_DIGEST_LENGTH, hash2, SHA256_DIGEST_LENGTH);
    memcpy(concatenated_result + 2 * SHA256_DIGEST_LENGTH, hash3, SHA256_DIGEST_LENGTH);
    memcpy(concatenated_result + 3 * SHA256_DIGEST_LENGTH, hash4, SHA256_DIGEST_LENGTH);

    // Initialize a GMP integer (mpz_t)
    mpz_t* result = (mpz_t*)malloc(sizeof(mpz_t));
    check_malloc(result);
    mpz_init(*result);

    // Set the concatenated result into the GMP integer
    mpz_import(*result, 4 * SHA256_DIGEST_LENGTH, 1, sizeof(unsigned char), 0, 0, concatenated_result);

    // Free the raw hash arrays and the concatenated result memory
    free(hash1);
    free(hash2);
    free(hash3);
    free(hash4);
    free(concatenated_result);
    return result;
}

int fermat_test(mpz_t* prime_candidate) {
    if (prime_candidate == NULL) {
        fprintf(stderr, "Null pointer passed to fermat_test\n");
        exit(EXIT_FAILURE);
    }

    mpz_t base, exponent, result;
    mpz_init_set_ui(base, 2);               // base = 2
    mpz_init(result);                       // to store the result
    mpz_init(exponent);                     // to store prime_candidate - 1

    // Calculate exponent = prime_candidate - 1
    mpz_sub_ui(exponent, *prime_candidate, 1);

    // Calculate 2^(prime_candidate - 1) mod prime_candidate
    mpz_powm(result, base, exponent, *prime_candidate);

    // Result check and cleanup
    int is_probably_prime = (mpz_cmp_ui(result, 1) == 0);

    // Clear GMP variables to prevent memory leaks
    mpz_clear(base);
    mpz_clear(exponent);
    mpz_clear(result);

    return is_probably_prime;
}

void mpz_mod_exp(mpz_t result, const mpz_t base, const mpz_t exp, const mpz_t mod) {
    mpz_t x, exp_temp;
    mpz_init(x);
    mpz_init_set(exp_temp, exp);
    mpz_set_ui(x, 1);

    mpz_t base_temp;
    mpz_init_set(base_temp, base);

    while (mpz_cmp_ui(exp_temp, 0) > 0) {
        if (mpz_odd_p(exp_temp)) {
            mpz_mul(x, x, base_temp);
            mpz_mod(x, x, mod);
        }
        mpz_mul(base_temp, base_temp, base_temp);
        mpz_mod(base_temp, base_temp, mod);
        mpz_tdiv_q_2exp(exp_temp, exp_temp, 1);
    }

    mpz_set(result, x);
    mpz_clear(x);
    mpz_clear(exp_temp);
    mpz_clear(base_temp);
}

int miller_rabin_test(mpz_t* probable_prime, int rounds) {
    if (mpz_cmp_ui(*probable_prime, 2) < 0)  // Check if under 2
        return 0;
    if (mpz_cmp_ui(*probable_prime, 2) == 0)  // Check for 2
        return 1;
    if (mpz_even_p(*probable_prime))  // Check if even
        return 0;

    // Step 1: Write n-1 as 2^s * d
    mpz_t d, n_minus_1;
    unsigned long s = 0;
    mpz_init(d);
    mpz_init(n_minus_1);
    
    mpz_sub_ui(n_minus_1, *probable_prime, 1);
    mpz_set(d, n_minus_1);
    
    while (mpz_even_p(d)) {
        mpz_divexact_ui(d, d, 2);
        s++;
    }

    // Initialize random state
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));

    // Perform rounds of testing
    printf("starting rounds of testing\n");

    for (int i = 0; i < rounds; i++) {
        mpz_t a, x, y;
        mpz_init(a);
        mpz_init(x);
        mpz_init(y);

        // Generate random base 'a' in range [2, n-2]
        do {
            mpz_urandomm(a, state, n_minus_1);
        } while (mpz_cmp_ui(a, 2) < 0);

        // Compute x = a^d mod n
        mpz_mod_exp(x, a, d, *probable_prime);

        if (mpz_cmp_ui(x, 1) != 0 && mpz_cmp(x, n_minus_1) != 0) {
            int witnessed = 1;
            for (unsigned long r = 1; r < s; r++) {
                mpz_mul(x, x, x);
                mpz_mod(x, x, *probable_prime);
                
                if (mpz_cmp_ui(x, 1) == 0) {
                    // Clean up and return composite
                    printf("was composite returning\n");

                    mpz_clear(a);
                    mpz_clear(x);
                    mpz_clear(y);
                    mpz_clear(d);
                    mpz_clear(n_minus_1);
                    gmp_randclear(state);
                    return 0;
                }
                if (mpz_cmp(x, n_minus_1) == 0) {
                    witnessed = 0;
                    break;
                }
            }
            printf("was composite returning\n");

            if (witnessed) {
                // Clean up and return composite
                mpz_clear(a);
                mpz_clear(x);
                mpz_clear(y);
                mpz_clear(d);
                mpz_clear(n_minus_1);
                gmp_randclear(state);
                return 0;
            }
        }
        
        mpz_clear(a);
        mpz_clear(x);
        mpz_clear(y);
    }

    // Clean up and return probably prime
    printf("miller returnig prime\n");

    mpz_clear(d);
    mpz_clear(n_minus_1);
    gmp_randclear(state);
    return 1;
}

mpz_t* generate_candidate(mpz_t* prime_candidate) {
    if (prime_candidate == NULL) {
        fprintf(stderr, "Null pointer passed to generate_candidate\n");
        exit(EXIT_FAILURE);
    }

    const int MAX_ATTEMPTS = 1000;
    int attempts = 0;

    // Ensure the starting number is odd
    if (mpz_even_p(*prime_candidate)) {
        printf("number was even, make it odd\n");
        mpz_add_ui(*prime_candidate, *prime_candidate, 1);
    }

    while (attempts < MAX_ATTEMPTS) {
        attempts++;
        if (fermat_test(prime_candidate)) {
            printf("passed fermat, going to miler rabin test\n");
            if (miller_rabin_test(prime_candidate, 40)) {
                return prime_candidate;
            }
        }
        
        mpz_add_ui(*prime_candidate, *prime_candidate, 2);
    }

    fprintf(stderr, "Failed to find prime after %d attempts\n", MAX_ATTEMPTS);
    return NULL;
}

void generate() {
    mpz_t p_minus_1, q_minus_1, q, p, n, d, totient, e;
    mpz_init(p_minus_1);
    mpz_init(q_minus_1);
    mpz_init(q);
    mpz_init(p);
    mpz_init(totient);
    mpz_init(d);
    mpz_init(e);
    mpz_init(n);

    mpz_set_ui(e, 65537);

    // Generate p and q
    mpz_t* p_candidate = generate_candidate(concatenate_hashes());
    if (p_candidate == NULL) {
        fprintf(stderr, "Failed to generate p\n");
        // Clean up
        mpz_clear(p_minus_1);
        mpz_clear(q_minus_1);
        mpz_clear(q);
        mpz_clear(p);
        mpz_clear(n);
        mpz_clear(d);
        mpz_clear(totient);
        mpz_clear(e);
        exit(EXIT_FAILURE);
    }

    mpz_t* q_candidate = generate_candidate(concatenate_hashes());
    if (q_candidate == NULL) {
        fprintf(stderr, "Failed to generate q\n");
        // Clean up
        free(p_candidate);
        mpz_clear(p_minus_1);
        mpz_clear(q_minus_1);
        mpz_clear(q);
        mpz_clear(p);
        mpz_clear(n);
        mpz_clear(d);
        mpz_clear(totient);
        mpz_clear(e);
        exit(EXIT_FAILURE);
    }

    // Set p and q
    mpz_set(p, *p_candidate);
    mpz_set(q, *q_candidate);
    
    // Calculate p-1 and q-1
    mpz_sub_ui(p_minus_1, p, 1);
    mpz_sub_ui(q_minus_1, q, 1);
    
    // Free the candidate pointers
    free(p_candidate);
    free(q_candidate);

    // Calculate n and totient
    mpz_mul(n, p, q);
    mpz_mul(totient, p_minus_1, q_minus_1);

    // Calculate d
    if (mpz_invert(d, e, totient) == 0) {
        fprintf(stderr, "Error: e and totient are not coprime\n");
        // Clean up
        mpz_clear(p_minus_1);
        mpz_clear(q_minus_1);
        mpz_clear(p);
        mpz_clear(q);
        mpz_clear(n);
        mpz_clear(d);
        mpz_clear(totient);
        mpz_clear(e);
        exit(EXIT_FAILURE);
    }

    // Set values in the struct
    mpz_set(values.p, p);
    mpz_set(values.q, q);
    mpz_set(values.n, n);
    mpz_set(values.d, d);
    mpz_set(values.totient, totient);
    mpz_set(values.e, e);

    // Clean up
    mpz_clear(p_minus_1);
    mpz_clear(q_minus_1);
    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(n);
    mpz_clear(d);
    mpz_clear(totient);
    mpz_clear(e);
}
