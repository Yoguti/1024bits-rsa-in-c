#include "rsa.h"
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <string.h>
#include <time.h>

// Print helper to make testing output readable
void print_mpz(const char* name, const mpz_t value) {
    gmp_printf("%s = %Zd\n", name, value);
}

void test_fermat_test() {
    mpz_t candidate;
    mpz_init_set_str(candidate, "104729", 10);  // Example prime number
    
    printf("\n=== Testing fermat_test ===\n");
    print_mpz("Input candidate", candidate);
    
    int result = fermat_test(&candidate);
    printf("fermat_test result: %d\n", result);
    
    mpz_clear(candidate);
}

void test_miller_rabin_test() {
    mpz_t candidate;
    mpz_init_set_str(candidate, "104729", 10);  // Example prime number
    
    printf("\n=== Testing miller_rabin_test ===\n");
    print_mpz("Input candidate", candidate);
    
    int result = miller_rabin_test(&candidate, 40);
    printf("miller_rabin_test result: %d\n", result);
    
    mpz_clear(candidate);
}

void test_concatenate_hashes() {
    printf("\n=== Testing concatenate_hashes ===\n");
    mpz_t* concatenated = concatenate_hashes();
    
    if (concatenated) {
        print_mpz("concatenated_hashes result", *concatenated);
        mpz_clear(*concatenated);
    } else {
        printf("concatenate_hashes failed\n");
    }
}

void test_generate_candidate() {
    printf("\n=== Testing generate_candidate ===\n");
    mpz_t* candidate = concatenate_hashes();
    mpz_t* result = generate_candidate(candidate);

    if (result) {
        print_mpz("generate_candidate result", *result);
        mpz_clear(*result);
    } else {
        printf("generate_candidate failed\n");
    }
    
    free(candidate);
}

void test_generate() {
    printf("\n=== Testing generate ===\n");
    generate();
    
    print_mpz("Generated p", values.p);
    print_mpz("Generated q", values.q);
    print_mpz("Generated n", values.n);
    print_mpz("Generated d", values.d);
    print_mpz("Generated totient", values.totient);
    print_mpz("Generated e", values.e);
}

int main() {
    init_rsa_values();
    
    
    printf("\n=== Running RSA Test Suite (with Miller-Rabin) ===\n");
    test_concatenate_hashes();
    test_generate_candidate();
    test_fermat_test();
    test_miller_rabin_test();
    test_generate();
    
    return 0;
}
