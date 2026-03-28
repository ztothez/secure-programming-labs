# Secure Programming Labs
Multi-language implementations exploring secure programming practices, vulnerability patterns, and defensive techniques in low-level and system-level code.

## Overview
This project demonstrates how common programming mistakes introduce security vulnerabilities, and how these risks can be mitigated through secure coding practices.

The modules cover memory safety, input validation, cryptography, file handling, and system-level protections.

## Modules
- `insecure-patterns/` — Common unsafe C patterns and vulnerabilities  
- `crypto-and-auth/` — AES encryption, PBKDF2 authentication, input filtering  
- `file-processing/` — Controlled file handling and transformation  
- `system-hardening/` — Core dump handling and system protections  
- `safe-calculation/` — Defensive input handling and safe operations  
- `ci-pipeline/` — Jenkins pipeline for automation  

## Tech Stack
- C / C++
- Python
- Cryptographic primitives (AES, PBKDF2)
- Jenkins (CI/CD)

## Usage
Each module can be compiled or executed independently.

```bash
gcc -o program program.c
./program
```
Refer to individual module READMEs for detailed usage.

## Implementation Notes
These implementations focus on illustrating secure programming concepts and defensive techniques rather than production-ready solutions.

## Background
Originally developed as part of a secure programming course, focused on understanding and mitigating common vulnerability patterns.