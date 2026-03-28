# Safe CLI Calculator
Implements a command-line calculator in C++ with strict input validation and defensive handling of numeric operations.

## Overview
This module demonstrates how improper input handling in arithmetic operations can lead to undefined behavior, incorrect results, or security issues. The implementation applies defensive programming techniques to ensure safe parsing and execution.

## Implementation
- `safe_calculator.cpp` — CLI calculator with validated input and controlled arithmetic operations  

## Key Concepts Demonstrated
- Safe numeric parsing using `strtold`  
- Validation of operators and input format  
- Handling of edge cases (NaN, infinity, invalid input)  
- Division-by-zero protection  
- Verification of finite computation results  
- Structured error handling via exit codes  

## Usage
```bash
g++ -o safe_calculator safe_calculator.cpp
./safe_calculator <number1> <operator> <number2>
```

### Example
```bash
./safe_calculator 10 / 2
```

## Security Considerations
* Prevents unsafe numeric conversions and undefined behavior
* Rejects invalid or non-finite inputs (NaN, infinity)
* Ensures predictable execution under malformed input
* Demonstrates defensive validation for CLI-based applications

## Purpose
Part of a broader exploration of secure programming and defensive system design using C++.
