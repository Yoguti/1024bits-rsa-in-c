// encoder.c
#include "rsa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>

void encrypt_message(const char* message, mpz_t* ciphertext) {
    mpz_t message_num, n, e;
    mpz_init(message_num);

     // Setting n and e directly saves some memory operations:
    mpz_init_set(n, values.n); 
    mpz_init_set(e, values.e);

    size_t len = strlen(message);
    // The cast to (unsigned char *) is good practice here:
    unsigned char* message_bytes = (unsigned char*)message; 

    mpz_import(message_num, len, 1, sizeof(unsigned char), 0, 0, message_bytes);
    mpz_mod_exp(*ciphertext, message_num, e, n);
    

    mpz_clears(message_num, n, e, NULL);
}

int main() {
    // Initialize RSA values
    init_rsa_values();
    generate(); // This generates the keys and fills the 'values' struct

    const char* message = "Hello, RSA!";
    mpz_t ciphertext;
    mpz_init(ciphertext);

    encrypt_message(message, &ciphertext);

    gmp_printf("Encrypted message: %Zx\n", ciphertext); // Hex output

    mpz_clear(ciphertext);
     mpz_clears(values.p, values.q, values.n, values.d, values.totient, values.e, NULL); // Clean up RSA values
    return 0;
}
