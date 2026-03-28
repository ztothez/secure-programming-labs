# Secure File Mixing Utility
Implements a defensive file processing utility in Python, focused on safe file handling and predictable behavior.

## Overview
This module reads two input files, interleaves their contents line by line, and writes the result to a new output file using strict safety constraints.

The implementation emphasizes controlled file operations and safe handling of edge cases to prevent unintended data loss or misuse.

## Implementation
- `mix_files.py` — Interleaves two input files and writes output using exclusive file creation mode (`'x'`)  

## Key Concepts Demonstrated
- Safe file handling and controlled output creation  
- Use of exclusive file mode (`'x'`) to prevent overwrites  
- Validation of input file existence  
- Exception handling for file operations  
- Predictable behavior under failure conditions  

## Usage
```bash
python mix_files.py <file1> <file2> <output>
```

### Example
```bash
python mix_files.py input1.txt input2.txt output.txt
```

## Security Considerations
* Prevents overwriting existing files through exclusive creation mode
* Ensures input files exist before processing
* Avoids accidental modification of source files
* Handles file-related errors safely and predictably

## Purpose
Part of a broader exploration of secure programming and defensive file handling in Python.
