// decoder.c
#include "rsa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>

void decrypt_message(mpz_t* ciphertext, char* decrypted_message, size_t bufsize) { // Add bufsize
    mpz_t message_num, n, d;
    mpz_init(message_num);
    mpz_init_set(n, values.n);
    mpz_init_set(d, values.d);
    

    mpz_mod_exp(message_num, *ciphertext, d, n);

    size_t len;
    unsigned char* message_bytes = malloc(mpz_sizeinbase(message_num, 2) / 8 + 1); // Allocate memory

    if (message_bytes == NULL) {
        perror("malloc failed");
        exit(1);
    }
    

    mpz_export(message_bytes, &len, 1, sizeof(unsigned char), 0, 0, message_num);


    if (len + 1 > bufsize) {       // Check buffer size before copying
        fprintf(stderr, "Buffer too small for decrypted message.\n");
        free(message_bytes);
        mpz_clears(message_num, n, d, NULL); 
        exit(1);    
    }


    strncpy(decrypted_message, (char*)message_bytes, len);
    decrypted_message[len] = '\0'; // Ensure null termination
    free(message_bytes);             // Free allocated memory



    mpz_clears(message_num, n, d, NULL); 
}

int main() {
    init_rsa_values();
    generate();

    mpz_t ciphertext;
    mpz_init(ciphertext);

    if(mpz_set_str(ciphertext, "f5e53a7c2c", 16) == -1) {
        fprintf(stderr, "Ciphertext not set correctly.\n");
    }
    
    char decrypted_message[256]; 
    decrypt_message(&ciphertext, decrypted_message, sizeof(decrypted_message));

    printf("Decrypted message: %s\n", decrypted_message);

    mpz_clear(ciphertext);
    mpz_clears(values.p, values.q, values.n, values.d, values.totient, values.e, NULL); // Clean up RSA values
    return 0;
}
