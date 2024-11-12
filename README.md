# 1024-Bit RSA in C with Custom Random Number Generator

This project implements a 1024-bit RSA encryption system in C using a handmade random number generator. It relies on the GNU MP (GMP) library to handle large integer arithmetic essential for RSA encryption and decryption operations.

## Features

Entropy Sources:

- 1024-bit RSA Key Generation: Generates RSA keys with 1024-bit security, suitable for basic encryption tasks.
- Custom Random Number Generator: Uses a handmade random number generator (in entropy_generator.h) for generating cryptographic primes.
- Manual Primality Testing: Implements Fermat and Miller-Rabin primality tests to verify prime candidates.
- Modular Arithmetic Functions: Includes custom modular exponentiation and concatenation of hash values for creating large numbers suitable for cryptographic applications.
