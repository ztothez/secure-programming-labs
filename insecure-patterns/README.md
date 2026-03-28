# Insecure C Patterns
Demonstrates common memory safety vulnerabilities in C through intentionally unsafe implementations.

## Overview
This module illustrates how small mistakes in memory handling can lead to serious security vulnerabilities. The examples highlight how incorrect assumptions in low-level code can result in exploitable conditions.

## Implementation
- `insecure_c_patterns.c` — Interactive program demonstrating multiple vulnerability patterns via a menu-driven interface  

## Vulnerability Cases
- Stack buffer overflow  
- Off-by-one errors  
- Heap overflow  
- Function pointer corruption  
- Format string vulnerability  

## Usage
```bash
gcc -o insecure_patterns insecure_c_patterns.c
./insecure_patterns
```

The program provides a simple menu to trigger and observe each vulnerability scenario.

## Security Implications
* Memory safety issues can lead to arbitrary code execution
* Subtle bugs (e.g. off-by-one) can introduce exploitable behavior
* Improper input handling can expose format string vulnerabilities
* Function pointer misuse can result in control flow hijacking

## Notes
These implementations are intentionally unsafe and are provided solely for demonstration and analysis purposes.

## Purpose
Part of a broader exploration of secure programming and vulnerability analysis in low-level systems.
