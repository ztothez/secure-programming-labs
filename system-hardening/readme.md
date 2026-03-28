# Process Hardening – Core Dump Control
Demonstrates how to disable core dumps in a process using `setrlimit()` as a basic system hardening measure.

## Overview
Core dumps may contain sensitive memory data, including credentials, keys, or application state. Restricting their creation is a common defensive practice in production environments.

This module shows how to programmatically disable core dumps for a running process.

## Implementation
- `disable_core_dumps.c` — Sets the core dump size limit to zero using `setrlimit()`

## Key Concepts Demonstrated
- Process-level resource limits (`setrlimit`)  
- System hardening techniques  
- Protection against unintended memory disclosure  

## Usage
```bash
gcc -o disable_core_dumps disable_core_dumps.c
./disable_core_dumps
```

## Security Considerations
* Core dumps can expose sensitive in-memory data
* Disabling them reduces the risk of data leakage in failure scenarios
* This is a basic mitigation and should be combined with other hardening practices

## Purpose
Part of a broader exploration of platform security and system-level behavior using C.