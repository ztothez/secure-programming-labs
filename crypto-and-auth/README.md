# Cryptography and Authentication
Implements cryptographic operations and authentication mechanisms with a focus on secure data handling and defensive design.

## Overview
This module demonstrates how sensitive data can be protected using modern cryptographic techniques and secure programming practices.

It covers encryption, password-based authentication, input validation, and secure randomness generation, highlighting common pitfalls and defensive approaches.

## Components
- `aes_gcm_file_crypt.cpp` — File encryption and decryption using AES-256-GCM with authenticated encryption :contentReference[oaicite:0]{index=0}  
- `pbkdf2_user_auth.py` — Password-based authentication using PBKDF2 with salted hashing :contentReference[oaicite:1]{index=1}  
- `ascii_whitelist_filter.py` — Input filtering with strict ASCII whitelist validation :contentReference[oaicite:2]{index=2}  
- `secure_random_dump.cpp` — Secure random byte generation using OS-provided entropy sources :contentReference[oaicite:3]{index=3}  

## Key Concepts Demonstrated
- Authenticated encryption (AES-GCM)  
- Key derivation with PBKDF2 and salting  
- Secure password storage and verification  
- Input validation and sanitization  
- Cryptographically secure random number generation  
- Defensive handling of file and user input  

## Usage
Each component can be compiled or executed independently.

### AES-GCM File Encryption
```bash
g++ -o crypto aes_gcm_file_crypt.cpp -lcrypto
./crypto enc <input> <output> <password>
./crypto dec <input> <output> <password>
```

### PBKDF2 Authentication
```bash
python pbkdf2_user_auth.py register <username>
python pbkdf2_user_auth.py login <username>
```

### ASCII Whitelist Filter
```bash
python ascii_whitelist_filter.py <inputfile>
```

### Secure Random Generator
```bash
g++ -o rand secure_random_dump.cpp
./rand <outputfile> [bytes]
```

## Security Considerations
* Uses authenticated encryption to ensure confidentiality and integrity
* Applies strong key derivation (PBKDF2 with high iteration count)
* Prevents unsafe file paths through input sanitization
* Uses OS-level secure randomness (`getrandom`, `/dev/urandom`, or platform APIs)
* Avoids common pitfalls such as weak randomness or improper input handling

## Implementation Notes
These implementations focus on demonstrating secure patterns and defensive techniques rather than production-ready cryptographic systems.

## Purpose
Part of a broader exploration of secure programming, cryptographic practices, and system-level security.